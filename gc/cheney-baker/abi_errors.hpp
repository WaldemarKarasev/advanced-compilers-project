#pragma once

namespace hw::stella::gc::errors {

// running contract message
inline constexpr char gc_not_runnint_msg[] = "GC is not running!";

// gc
// init error messages
inline constexpr char gc_already_init[] = "GC is already initialized!";
inline constexpr char gc_null_config[] = "StellaGcConfig is nullptr!";
inline constexpr char gc_invalid_abi_version[] = "Unsupported GC abi version!";
inline constexpr char gc_invalid_config_heap_size[] = "max_heap_bytes equals 0!";

// init managed
inline constexpr char gc_im_null_obj[] = "Null object!";
inline constexpr char gc_im_obj_not_owned[] = "Object not owned by GC!";
inline constexpr char gc_im_init_index_out_of_range[] = "Initialized index out of range!";

// object messages
inline constexpr char obj_null_obj_descriptor[] = "null object descriptor!";
inline constexpr char obj_invalid_descriptor_abi_version[] = "incompatible descriptor ABI version!";
inline constexpr char obj_unknown_obj_kind[] = "unknown object kind!";
inline constexpr char obj_unknown_descriptor_flag[] = "unknown descriptor flag!";
inline constexpr char obj_descriptor_fields_count_overflow[] = "active field count overflows size_t!";
inline constexpr char obj_active_fields_exceed_slot_capacity[] = "active fields exceed slot capacity!";
inline constexpr char obj_heap_allocation_for_static_descriptor[] = "heap allocation requested with static descriptor!";
inline constexpr char obj_slot_cap_overflow[] = "object slot capacity overflows size_t!";
inline constexpr char obj_size_overflow[] = "object size overflows size_t!";

} // namespace hw::stella::gc::errors
