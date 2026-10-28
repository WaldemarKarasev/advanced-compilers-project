cmake_minimum_required(VERSION 3.20)

project(stella_gc
    VERSION 1.0.0
    LANGUAGES C CXX
)

# language standarts
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# compile options
include(cmake/compiler_options.cmake)

# common vars
set(STELLA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
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

# gc sources
add_subdirectory(gc)

# gtest
include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()


# Logging of toolchain and build settings used for project
message(STATUS "")
message(STATUS "Stella GC build configuration")

# system
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")

# C
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C standard: ${CMAKE_C_STANDARD}")
# C++
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Executable linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
# compile options
list(JOIN C_COMPILE_OPTIONS " " STELLA_C_FLAGS_FOR_LOG)
list(JOIN CXX_COMPILE_OPTIONS " " STELLA_CXX_FLAGS_FOR_LOG)

message(STATUS "Compile options:")
message(STATUS "  C:   ${STELLA_C_FLAGS_FOR_LOG}")
message(STATUS "  C++: ${STELLA_CXX_FLAGS_FOR_LOG}")

# directories
message(STATUS "Build tests: ${BUILD_TESTING}")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")

