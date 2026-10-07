cmake_minimum_required(VERSION 3.20)

project(stella_gc
    VERSION 1.0.0
    LANGUAGES C CXX
)

# language standarts
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# compile options
# add_compile_options(
#     $<$<COMPILE_LANGUAGE:C>:-Wall>
#     $<$<COMPILE_LANGUAGE:C>:-Wextra>
#     $<$<COMPILE_LANGUAGE:C>:-Wpedantic>
# )

# add_compile_options(
#     $<$<COMPILE_LANGUAGE:CXX>:-Wall>
#     $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
#     $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
# )

# common vars
set(STELLA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(STELLA_GC_IMPL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gc)

# build stella runtime
add_library(stella_runtime STATIC
    runtime/runtime.c
    runtime/scalar.c
)

target_include_directories(stella_runtime
    PUBLIC
        ${STELLA_INCLUDE_DIR}
)

# build gc stub
add_library(stella_gc_stub STATIC
    ${STELLA_GC_IMPL_DIR}/gc.c
)

target_include_directories(stella_gc_stub
    PRIVATE
        ${STELLA_INCLUDE_DIR}
)

# build semi-dfs + incremental extension gc
add_library(stella_gc_semi_dfs STATIC
    ${STELLA_GC_IMPL_DIR}/gc-semi-dfs.cpp
)

target_include_directories(stella_gc_semi_dfs
    PRIVATE
        ${STELLA_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# build stella example
function(add_stella_example name)

    # 1. implementation example
    add_executable(${name}
        runtime/main.c
        examples/${name}.c
    )

    target_include_directories(${name}
        PRIVATE
            ${STELLA_INCLUDE_DIR}
    )

    target_link_libraries(${name}
        PRIVATE
            stella_runtime
            stella_gc_semi_dfs
    )

    set_target_properties(${name}
        PROPERTIES
            LINKER_LANGUAGE CXX
    )

    # 2. stub example
    add_executable(${name}_stub
        runtime/main.c
        examples/${name}.c
    )

    target_include_directories(${name}_stub
        PRIVATE
            ${STELLA_INCLUDE_DIR}
    )

    target_link_libraries(${name}_stub
        PRIVATE
            stella_runtime
            stella_gc_stub
    )

endfunction()

# examples
add_stella_example(chat)

# TODO: gtest