#pragma once

namespace hw::stella::gc::errors {

// running contract message
inline constexpr char gc_not_runnint_msg[] = "GC is not running!";

// init error messages
inline constexpr char gc_already_init[] = "GC is already initialized!";
inline constexpr char gc_null_config[] = "StellaGcConfig is nullptr!";
inline constexpr char gc_invalid_abi_version[] = "Unsupported GC abi version!";
inline constexpr char gc_invalid_config_heap_size[] = "max_heap_bytes equals 0!";

} // namespace hw::stella::gc::errors
