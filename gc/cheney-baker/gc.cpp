
#include <bindings/gc.hpp>
#include <cstddef>

#include "abi_errors.hpp"
#include "collection.hpp"
#include "gc_state.hpp"
#include "object.hpp"
#include "stella/runtime.h"

namespace hw::stella::gc {
//
void init(const StellaGcConfig *config) {
  if (get_gc_state().State() != Lifecycle::NotInit) {
    stella_abi_violation(errors::gc_already_init);
  }

  if (config == nullptr) {
    stella_abi_violation(errors::gc_null_config);
  }

  if (config->abi_version != STELLA_C_GC_ABI_VERSION) {
    stella_abi_violation(errors::gc_invalid_abi_version);
  }

  if (config->max_heap_bytes == 0) {
    stella_abi_violation(errors::gc_invalid_config_heap_size);
  }

  if (not get_gc_state().Init(config->max_heap_bytes)) {
    stella_gc_out_of_memory(config->max_heap_bytes);
  }
}

void shutdown(void) {
  require_running();

  auto &gc_state = get_gc_state();

  if (not gc_state.GetRoots().DynamicEmpty()) {
    stella_abi_violation(errors::gc_pr_shutdown_with_roots);
  }

  gc_state.Deinit();
}

// root handling
void push_root(StellaValue *root) {
  require_running();

  if (root == nullptr) {
    stella_abi_violation(errors::gc_pr_null_root);
  }

  auto &roots = get_gc_state().GetRoots();

  if (roots.IsRegistered(root)) {
    stella_abi_violation(errors::gc_pr_duplicate_root);
  }

  if (not roots.PushDynamic(root)) {
    stella_abi_violation(errors::gc_pr_root_storage_exhausted);
  }
}
void pop_root(StellaValue *root) {
  require_running();

  auto &roots = get_gc_state().GetRoots();

  if (roots.DynamicEmpty()) {
    stella_abi_violation(errors::gc_pr_root_stack_empty);
  }

  if (roots.DynamicTop() != root) {
    stella_abi_violation(errors::gc_pr_root_not_on_top);
  }

  roots.PopDynamic();
}
void register_permanent_root(StellaValue *root) {
  require_running();

  if (root == nullptr) {
    stella_abi_violation(errors::gc_pr_null_root);
  }

  auto &roots = get_gc_state().GetRoots();

  if (roots.IsRegistered(root)) {
    stella_abi_violation(errors::gc_pr_duplicate_root);
  }

  if (!roots.RegisterPermanent(root)) {
    stella_abi_violation(errors::gc_pr_root_storage_exhausted);
  }
}

// alloc
StellaValue alloc(const StellaObjectDescriptor *descriptor) {
  require_running();

  object::validate_allocation_descriptor(descriptor);

  const size_t size = object::object_size(descriptor);

  auto &gc_state = get_gc_state();

  if (size > gc_state.MaxHeapSize()) {
    stella_gc_out_of_memory(size);
  }

  std::byte *storage =
      gc_state.GetHeap().FromSpace().Allocate(size, alignof(StellaObject));

  if (storage == nullptr) {

    collection::Collect();

    storage =
        gc_state.GetHeap().FromSpace().Allocate(size, alignof(StellaObject));

    if (storage == nullptr) {
      stella_gc_out_of_memory(size);
    }
  }

  auto *object = reinterpret_cast<StellaObject *>(storage);

  object::initialize_object(object, descriptor);

  return object;
}

// init
void object_init_managed(StellaValue object, size_t index, StellaValue value) {
  require_running();

  if (object == nullptr) {
    stella_abi_violation(errors::gc_im_null_obj);
  }

  if (not get_gc_state().GetHeap().Owns(object)) {
    stella_abi_violation(errors::gc_im_obj_not_owned);
  }

  object::validate_descriptor(object->descriptor);

  if (index >= object->descriptor->managed_count) {
    stella_abi_violation(errors::gc_im_init_index_out_of_range);
  }

  object->fields[index].managed = value;
}

void object_init_primitive(StellaValue object, size_t index,
                           StellaPrimitive value) {
  require_running();
  if (object == nullptr) {
    stella_abi_violation(errors::gc_im_null_obj);
  }

  if (!get_gc_state().GetHeap().Owns(object)) {
    stella_abi_violation(errors::gc_im_obj_not_owned);
  }

  object::validate_descriptor(object->descriptor);

  if (index >= object->descriptor->primitive_count) {
    stella_abi_violation(errors::gc_im_init_index_out_of_range);
  }

  const std::size_t physical_index = object->descriptor->managed_count + index;

  object->fields[physical_index].primitive = value;
}

// read/write barriers
StellaValue read_managed(StellaValue *object_root, size_t index) {
  require_running();

  if (object_root == nullptr) {
    stella_abi_violation(errors::gc_null_object_root);
  }

  StellaValue value = *object_root;

  object::validate_managed_index(value, index);

  return value->fields[index].managed;
}

void write_managed(StellaValue *object_root, size_t index,
                   StellaValue *value_root) {
  require_running();

  if (object_root == nullptr) {
    stella_abi_violation(errors::gc_null_object_root);
  }

  if (value_root == nullptr) {
    stella_abi_violation(errors::gc_null_value_root);
  }

  StellaValue object = *object_root;

  object::validate_managed_index(object, index);

  if ((object->descriptor->flags & STELLA_DESCRIPTOR_STATIC) != 0) {
    stella_abi_violation(errors::gc_write_static);
  }

  object->fields[index].managed = *value_root;
}

// read/write primitives
StellaPrimitive object_read_primitive(StellaValue object, size_t index) {
  require_running();

  object::validate_primitive_index(object, index);

  const size_t physical_index = object->descriptor->managed_count + index;

  return object->fields[physical_index].primitive;
}
void object_write_primitive(StellaValue object, size_t index,
                            StellaPrimitive value) {
  require_running();

  object::validate_primitive_index(object, index);

  if ((object->descriptor->flags & STELLA_DESCRIPTOR_STATIC) != 0) {
    stella_abi_violation(errors::obj_write_static);
  }

  const std::size_t physical_index = object->descriptor->managed_count + index;

  object->fields[physical_index].primitive = value;
}

void update_object(StellaValue *object_root,
                   const StellaObjectDescriptor *new_descriptor,
                   StellaValue *const new_managed_roots[],
                   const StellaPrimitive new_primitives[]) {
  require_running();
  if (object_root == nullptr) {
    stella_abi_violation(errors::gc_null_object_root);
  }

  StellaValue object = *object_root;

  object::validate_object(object);
  object::validate_descriptor(new_descriptor);

  const auto *old_descriptor = object->descriptor;

  if ((old_descriptor->flags & STELLA_DESCRIPTOR_UPDATABLE) == 0) {
    stella_abi_violation(errors::obj_not_updatable);
  }

  if ((new_descriptor->flags & STELLA_DESCRIPTOR_STATIC) != 0) {
    stella_abi_violation(errors::obj_update_static_descriptor);
  }

  if (old_descriptor->slot_capacity != new_descriptor->slot_capacity) {
    stella_abi_violation(errors::obj_update_capacity_mismatch);
  }

  if (new_descriptor->managed_count > 0 && new_managed_roots == nullptr) {
    stella_abi_violation(errors::obj_null_managed_roots);
  }

  if (new_descriptor->primitive_count > 0 && new_primitives == nullptr) {
    stella_abi_violation(errors::obj_null_primitives);
  }

  object = *object_root;

  const std::size_t m = new_descriptor->managed_count;

  const std::size_t p = new_descriptor->primitive_count;

  const std::size_t c = new_descriptor->slot_capacity;

  // Managed fields.
  for (std::size_t i = 0; i < m; ++i) {
    if (new_managed_roots[i] == nullptr) {
      stella_abi_violation(errors::gc_null_value_root);
    }

    object->fields[i].managed = *new_managed_roots[i];
  }

  // primitive fields
  for (std::size_t i = 0; i < p; ++i) {
    object->fields[m + i].primitive = new_primitives[i];
  }

  // inactive part
  for (std::size_t i = m + p; i < c; ++i) {
    object->fields[i].primitive = 0;
  }

  object->descriptor = new_descriptor;
}

// diagnostics
void print_statistics(FILE *output) { require_running(); }
void print_state(FILE *output) { require_running(); }
void print_roots(FILE *output) { require_running(); }
} // namespace hw::stella::gc

extern "C" {

void stella_gc_init(const StellaGcConfig *config) {
  hw::stella::gc::init(config);
}

void stella_gc_shutdown(void) { hw::stella::gc::shutdown(); }

void stella_gc_push_root(StellaValue *root) { hw::stella::gc::push_root(root); }

void stella_gc_pop_root(StellaValue *root) { hw::stella::gc::pop_root(root); }

void stella_gc_register_permanent_root(StellaValue *root) {
  hw::stella::gc::register_permanent_root(root);
}

StellaValue stella_gc_alloc(const StellaObjectDescriptor *descriptor) {
  return hw::stella::gc::alloc(descriptor);
}

void stella_object_init_managed(StellaValue object, size_t index,
                                StellaValue value) {
  hw::stella::gc::object_init_managed(object, index, value);
}

void stella_object_init_primitive(StellaValue object, size_t index,
                                  StellaPrimitive value) {
  hw::stella::gc::object_init_primitive(object, index, value);
}

StellaValue stella_gc_read_managed(StellaValue *object_root, size_t index) {
  return hw::stella::gc::read_managed(object_root, index);
}

void stella_gc_write_managed(StellaValue *object_root, size_t index,
                             StellaValue *value_root) {
  hw::stella::gc::write_managed(object_root, index, value_root);
}

StellaPrimitive stella_object_read_primitive(StellaValue object, size_t index) {
  return hw::stella::gc::object_read_primitive(object, index);
}

void stella_object_write_primitive(StellaValue object, size_t index,
                                   StellaPrimitive value) {
  hw::stella::gc::object_write_primitive(object, index, value);
}

void stella_gc_update_object(StellaValue *object_root,
                             const StellaObjectDescriptor *new_descriptor,
                             StellaValue *const new_managed_roots[],
                             const StellaPrimitive new_primitives[]) {
  hw::stella::gc::update_object(object_root, new_descriptor, new_managed_roots,
                                new_primitives);
}

void stella_gc_print_statistics(FILE *output) {
  hw::stella::gc::print_statistics(output);
}

void stella_gc_print_state(FILE *output) {
  hw::stella::gc::print_state(output);
}
void stella_gc_print_roots(FILE *output) {
  hw::stella::gc::print_roots(output);
}
} // extern "C"