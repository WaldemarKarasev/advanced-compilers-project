cmake_minimum_required(VERSION 3.20)

add_gc_test(cheney_baker_copying_test copying_test.cpp stella::cheney_baker_gc)
# cheney_baker_copying_test tests internal functionality => add path to internal includes
target_include_directories(cheney_baker_copying_test 
    PRIVATE
        ${PROJECT_INCLUDE_DIR}/gc/cheney-baker
)