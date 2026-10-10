cmake_minimum_required(VERSION 3.20)

# build cheney-baker gc
add_library(cheney_baker_gc STATIC
    ${STELLA_GC_IMPL_DIR}/cheney-baker/gc.cpp
)

target_include_directories(cheney_baker_gc
    PRIVATE
        ${STELLA_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
)

add_library(stella::cheney_baker_gc ALIAS cheney_baker_gc)

create_stella_executable(chat_cheney_baker ${CMAKE_SOURCE_DIR}/examples/chat.c stella::cheney_baker_gc)