#pragma once

#include <cstddef>

#include <bindings/gc.hpp>

namespace hw::stella::gc::object {

void validate_descriptor(const StellaObjectDescriptor *descriptor);

void validate_allocation_descriptor(const StellaObjectDescriptor *descriptor);

void validate_object(StellaValue object);

void validate_managed_index(StellaValue object, std::size_t index);

void validate_primitive_index(StellaValue object, std::size_t index);

std::size_t object_size(const StellaObjectDescriptor *descriptor);

void initialize_object(StellaValue object,
                       const StellaObjectDescriptor *descriptor);

} // namespace hw::stella::gc::object