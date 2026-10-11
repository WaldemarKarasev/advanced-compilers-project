cmake_minimum_required(VERSION 3.20)

add_gc_test(smoke_test smoke_test.cpp stella::cheney_baker_gc)

add_gc_test(lifecycle_test lifecycle_test.cpp stella::cheney_baker_gc)
