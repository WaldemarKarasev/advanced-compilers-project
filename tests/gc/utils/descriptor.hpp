#pragma once

#include <bindings/gc.hpp>

#include <cstddef>
#include <cstdint>

namespace hw::stella::gc::test {

inline StellaObjectDescriptor make_descriptor(
    size_t managed = 0,
    size_t primitive = 0,
    size_t capacity = 0,
    uint32_t flags = 0
) {
    return {
        .abi_version = STELLA_C_GC_ABI_VERSION,
        .kind = STELLA_OBJECT_DATA,
        .flags = flags,
        .entry_arity = 0,
        .managed_count = managed,
        .primitive_count = primitive,
        .slot_capacity = capacity,
        .entry_code = nullptr,
        .debug_name = "test-object",
    };
}

} // namespace hw::stella::gc::test