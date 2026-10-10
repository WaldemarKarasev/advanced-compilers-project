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
set(STELLA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
set(STELLA_GC_IMPL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gc)
set(STELLA_RUNTIME_LIB stella_runtime)

# build stella runtime
add_library(${STELLA_RUNTIME_LIB} STATIC
    runtime/runtime.c
    runtime/scalar.c
)

target_include_directories(${STELLA_RUNTIME_LIB}
    PUBLIC
        ${STELLA_INCLUDE_DIR}
)

# build stella executable
function(create_stella_executable name c_src gc_name)

    add_executable(${name}
        ${CMAKE_SOURCE_DIR}/runtime/main.c
        ${c_src}
    )

    target_include_directories(${name}
        PRIVATE
            ${STELLA_INCLUDE_DIR}
    )

    target_link_libraries(${name}
        PRIVATE
            ${STELLA_RUNTIME_LIB}
            ${gc_name}
    )

    set_target_properties(${name}
        PROPERTIES
            LINKER_LANGUAGE CXX
    )

endfunction()

add_subdirectory(gc)

# examples
# add_stella_example(chat)

# TODO: gtest
