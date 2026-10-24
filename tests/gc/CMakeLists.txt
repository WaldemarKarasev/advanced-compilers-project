cmake_minimum_required(VERSION 3.20)

set(GC_UTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)

function(add_gc_test test_name src_files gc_lib)
    message(STATUS "Function add_gc_test(name=${test_name}, src_files={${src_files}}, gc_lib=${gc_lib})")

    add_executable(${test_name}
        ${src_files}
    )

    # common project includes
    target_include_directories(${test_name}
        PRIVATE
            ${STELLA_INCLUDE_DIR}
            ${PROJECT_INCLUDE_DIR}
    )

    # utils directory include
    target_include_directories(${test_name}
        PRIVATE
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/utils    
    )

    # stella runtime linking
    target_link_libraries(${test_name}
        PRIVATE
            ${gc_lib}
            ${STELLA_RUNTIME_LIB}
            GTest::gtest_main
    )

    set_target_properties(${test_name}
        PROPERTIES
            LINKER_LANGUAGE CXX
    )

    gtest_discover_tests(${test_name})
    
endfunction()

# smoke test
add_gc_test(smoke_test smoke_test.cpp stella::cheney_baker_gc)

add_subdirectory(gc_implementations)
add_subdirectory(public_api)