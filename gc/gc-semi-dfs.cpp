#include <bindings/gc.hpp>

namespace hw::stella::gc {
// 
void init(const StellaGcConfig *config) {}
void shutdown(void) {}

// root handling
void push_root(StellaValue *root) {}
void pop_root(StellaValue *root) {}
void register_permanent_root(StellaValue *root) {}

// alloc
StellaValue alloc(const StellaObjectDescriptor *descriptor) { return {}; }

// init
void object_init_managed(StellaValue object, size_t index, StellaValue value) {}
void object_init_primitive(StellaValue object, size_t index,
                           StellaPrimitive value) {}

// read/write barriers
StellaValue read_managed(StellaValue *object_root, size_t index) { return {}; }
void write_managed(StellaValue *object_root, size_t index,
                   StellaValue *value_root) {}

// read/write primitives
StellaPrimitive object_read_primitive(StellaValue object, size_t index) {
  return {};
}
void object_write_primitive(StellaValue object, size_t index,
                            StellaPrimitive value) {}

void update_object(StellaValue *object_root,
                   const StellaObjectDescriptor *new_descriptor,
                   StellaValue *const new_managed_roots[],
                   const StellaPrimitive new_primitives[]) {}

// diagnostics
void print_statistics(FILE *output) {}
void print_state(FILE *output) {}
void print_roots(FILE *output) {}
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