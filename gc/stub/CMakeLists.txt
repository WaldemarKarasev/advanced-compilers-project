cmake_minimum_required(VERSION 3.20)


# build gc stub
add_library(stella_gc_stub STATIC
    gc.c
)

target_include_directories(stella_gc_stub
    PRIVATE
        ${STELLA_INCLUDE_DIR}
)

create_stella_executable(chat_stub ${CMAKE_SOURCE_DIR}/examples/chat.c stella_gc_stub)
