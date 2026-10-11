#include "object.hpp"

#include <cstddef>
#include <limits>

#include "abi_errors.hpp"

namespace hw::stella::gc::object {

namespace {

bool add_overflows(size_t lhs, size_t rhs) {
  return lhs > std::numeric_limits<size_t>::max() - rhs;
}

bool multiply_overflows(size_t lhs, size_t rhs) {
  return lhs > std::numeric_limits<size_t>::max() / rhs;
}

} // namespace

void validate_descriptor(const StellaObjectDescriptor *descriptor) {
  if (descriptor == nullptr) {
    stella_abi_violation(errors::obj_null_obj_descriptor);
  }

  if (descriptor->abi_version != STELLA_C_GC_ABI_VERSION) {
    stella_abi_violation(errors::obj_invalid_descriptor_abi_version);
  }

  if (descriptor->kind < STELLA_OBJECT_DATA ||
      descriptor->kind > STELLA_OBJECT_INDIRECTION) {
    stella_abi_violation(errors::obj_unknown_obj_kind);
  }

  constexpr uint32_t known_flags =
      STELLA_DESCRIPTOR_STATIC | STELLA_DESCRIPTOR_UPDATABLE;

  if ((descriptor->flags & ~known_flags) != 0) {
    stella_abi_violation(errors::obj_unknown_descriptor_flag);
  }

  if (add_overflows(descriptor->managed_count, descriptor->primitive_count)) {
    stella_abi_violation(errors::obj_descriptor_fields_count_overflow);
  }

  if (descriptor->managed_count + descriptor->primitive_count >
      descriptor->slot_capacity) {
    stella_abi_violation(errors::obj_active_fields_exceed_slot_capacity);
  }
}

void validate_allocation_descriptor(const StellaObjectDescriptor *descriptor) {
  validate_descriptor(descriptor);

  if ((descriptor->flags & STELLA_DESCRIPTOR_STATIC) != 0) {
    stella_abi_violation(errors::obj_heap_allocation_for_static_descriptor);
  }
}

std::size_t object_size(const StellaObjectDescriptor *descriptor) {
  if (multiply_overflows(descriptor->slot_capacity, sizeof(StellaSlot))) {
    stella_abi_violation(errors::obj_slot_cap_overflow);
  }

  const auto slots = descriptor->slot_capacity * sizeof(StellaSlot);

  if (add_overflows(sizeof(StellaObject), slots)) {
    stella_abi_violation(errors::obj_size_overflow);
  }

  return sizeof(StellaObject) + slots;
}

void initialize_object(StellaValue object,
                       const StellaObjectDescriptor *descriptor) {
  object->descriptor = descriptor;
  object->gc_word = 0;

  object->fields = descriptor->slot_capacity == 0
                       ? nullptr
                       : reinterpret_cast<StellaSlot *>(object + 1);

  for (std::size_t i = 0; i < descriptor->managed_count; ++i) {
    object->fields[i].managed = nullptr;
  }

  for (std::size_t i = descriptor->managed_count; i < descriptor->slot_capacity;
       ++i) {
    object->fields[i].primitive = 0;
  }
}

} // namespace hw::stella::gc::object