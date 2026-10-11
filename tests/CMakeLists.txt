cmake_minimum_required(VERSION 3.20)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)



include(GoogleTest)

function(add_gc_test test_name src_files gc_lib)
    message(STATUS "Function add_gc_test(name=${test_name}, src_files={${src_files}}, gc_lib=${gc_lib})")

    add_executable(${test_name}
        ${src_files}
    )

    target_include_directories(${test_name}
        PRIVATE
            ${STELLA_INCLUDE_DIR}
            ${PROJECT_INCLUDE_DIR}
    )

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

add_subdirectory(gc)