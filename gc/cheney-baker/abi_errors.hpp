#pragma once

namespace hw::stella::gc::errors {

// running contract message
inline constexpr char gc_not_runnint_msg[] = "GC is not running!";

// gc
// init error messages
inline constexpr char gc_already_init[] = "GC is already initialized!";
inline constexpr char gc_null_config[] = "StellaGcConfig is nullptr!";
inline constexpr char gc_invalid_abi_version[] = "Unsupported GC abi version!";
inline constexpr char gc_invalid_config_heap_size[] =
    "max_heap_bytes equals 0!";
inline constexpr char gc_invalid_step_budget[] =
    "STELLA_GC_STEP_BUDGET must be a positive integer!";
// init managed
inline constexpr char gc_im_null_obj[] = "Null object!";
inline constexpr char gc_im_obj_not_owned[] = "Object not owned by GC!";
inline constexpr char gc_im_init_index_out_of_range[] =
    "Initialized index out of range!";
// push root
inline constexpr char gc_pr_null_root[] = "Root slot is nullptr!";
inline constexpr char gc_pr_duplicate_root[] = "Root is already registered!";
inline constexpr char gc_pr_root_storage_exhausted[] =
    "Unable to store GC root!";
inline constexpr char gc_pr_root_stack_empty[] = "Dynamic root stack is empty!";
inline constexpr char gc_pr_root_not_on_top[] =
    "Root is not on top of dynamic root stack!";
inline constexpr char gc_pr_shutdown_with_roots[] =
    "GC shutdown with dynamic roots registered!";

// read/write managed and primitive
inline constexpr char gc_null_object_root[] = "Object root is nullptr!";
inline constexpr char gc_null_value_root[] = "Value root is nullptr!";
inline constexpr char gc_write_static[] = "Cannot write static object!";
inline constexpr char gc_null_output[] = "Output stream is nullptr!";

// object validation messages
inline constexpr char obj_null_obj_descriptor[] = "null object descriptor!";
inline constexpr char obj_invalid_descriptor_abi_version[] =
    "incompatible descriptor ABI version!";
inline constexpr char obj_unknown_obj_kind[] = "unknown object kind!";
inline constexpr char obj_unknown_descriptor_flag[] =
    "unknown descriptor flag!";
inline constexpr char obj_descriptor_fields_count_overflow[] =
    "active field count overflows size_t!";
inline constexpr char obj_active_fields_exceed_slot_capacity[] =
    "active fields exceed slot capacity!";
inline constexpr char obj_heap_allocation_for_static_descriptor[] =
    "heap allocation requested with static descriptor!";
inline constexpr char obj_slot_cap_overflow[] =
    "object slot capacity overflows size_t!";
inline constexpr char obj_size_overflow[] = "object size overflows size_t!";
inline constexpr char obj_null_object[] = "Null object!";
inline constexpr char obj_managed_index_out_of_range[] =
    "Managed object index out of range!";
inline constexpr char obj_primitive_index_out_of_range[] =
    "Primitive object index out of range!";
inline constexpr char obj_write_static[] = "Cannot write to static object!";

// update
inline constexpr char obj_not_updatable[] = "Object is not updatable!";
inline constexpr char obj_update_static_descriptor[] =
    "New descriptor is static!";
inline constexpr char obj_update_capacity_mismatch[] =
    "New descriptor has different slot capacity!";
inline constexpr char obj_null_managed_roots[] =
    "Managed roots array is nullptr!";
inline constexpr char obj_null_primitives[] = "Primitives array is nullptr!";

} // namespace hw::stella::gc::errors