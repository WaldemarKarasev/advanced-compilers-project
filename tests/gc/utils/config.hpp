#pragma once

#include <bindings/gc.hpp>
#include <cstddef>

namespace hw::stella::gc::test {

using StellaAbiType = decltype(STELLA_C_GC_ABI_VERSION);
using HeapSizeType = size_t;

inline StellaGcConfig
make_config(HeapSizeType heap_size = 1024,
    StellaAbiType abi_version = STELLA_C_GC_ABI_VERSION) {
  return {
      .abi_version = abi_version,
      .max_heap_bytes = heap_size,
  };
}

} // namespace hw::stella::gc::test
