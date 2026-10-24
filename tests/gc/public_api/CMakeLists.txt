cmake_minimum_required(VERSION 3.20)

add_gc_test(allocation_test allocation_test.cpp stella::cheney_baker_gc)

add_gc_test(collection_test collection_test.cpp stella::cheney_baker_gc)

add_gc_test(diagnostics_test diagnostics_test.cpp stella::cheney_baker_gc)

add_gc_test(field_access_test field_access_test.cpp stella::cheney_baker_gc)

add_gc_test(lifecycle_test lifecycle_test.cpp stella::cheney_baker_gc)

add_gc_test(root_test root_test.cpp stella::cheney_baker_gc)

add_gc_test(update_object_test update_object_test.cpp stella::cheney_baker_gc)

add_gc_test(small_heap_stress_test small_heap_stress_test.cpp stella::cheney_baker_gc)