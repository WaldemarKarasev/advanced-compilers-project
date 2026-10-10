cmake_minimum_required(VERSION 3.20)

# build cheney-baker gc
add_library(stella_gc_cheney_baker STATIC
    ${STELLA_GC_IMPL_DIR}/cheney-baker/gc.cpp
)

target_include_directories(stella_gc_cheney_baker
    PRIVATE
        ${STELLA_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)


create_stella_executable(chat_cheney_baker ${CMAKE_SOURCE_DIR}/examples/chat.c stella_gc_cheney_baker)