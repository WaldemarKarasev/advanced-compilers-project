#pragma once

#include <cstddef>

#include <bindings/gc.hpp>

namespace hw::stella::gc::object {

void validate_descriptor(const StellaObjectDescriptor *descriptor);

void validate_allocation_descriptor(const StellaObjectDescriptor *descriptor);

std::size_t object_size(const StellaObjectDescriptor *descriptor);

void initialize_object(StellaValue object,
                       const StellaObjectDescriptor *descriptor);

} // namespace hw::stella::gc::object