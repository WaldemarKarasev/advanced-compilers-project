cmake_minimum_required(VERSION 3.20)

# build cheney-baker gc
add_library(cheney_baker_gc STATIC
    copying.cpp
    gc_state.cpp
    gc.cpp
    heap.cpp
    object.cpp
    roots.cpp
)

target_include_directories(cheney_baker_gc
    PRIVATE
        ${STELLA_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cheney_baker_gc
    PRIVATE
        ${STELLA_RUNTIME_LIB}
)

add_library(stella::cheney_baker_gc ALIAS cheney_baker_gc)

create_stella_executable(chat_cheney_baker ${CMAKE_SOURCE_DIR}/examples/chat.c stella::cheney_baker_gc)