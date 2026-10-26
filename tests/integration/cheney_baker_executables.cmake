cmake_minimum_required(VERSION 3.20)

# stella executables
# examples
create_stella_executable(chat_cheney_baker 
    ${CMAKE_SOURCE_DIR}/examples/chat.c 
    stella::cheney_baker_gc
)

create_stella_executable(double_cheney_baker 
    ${CMAKE_SOURCE_DIR}/examples/double.c 
    stella::cheney_baker_gc
)

create_stella_executable(history_cheney_baker 
    ${CMAKE_SOURCE_DIR}/examples/history.c 
    stella::cheney_baker_gc
)

create_stella_executable(playlist_cheney_baker 
    ${CMAKE_SOURCE_DIR}/examples/playlist.c 
    stella::cheney_baker_gc
)

# examples/extra

create_stella_executable(01_large_then_small_cheney_baker 
    ${CMAKE_SOURCE_DIR}/examples/extra/01_large_then_small.c
    stella::cheney_baker_gc
)

create_stella_executable(02_replacement_churn_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/02_replacement_churn.c
    stella::cheney_baker_gc
)

create_stella_executable(03_two_live_lists_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/03_two_live_lists.c
    stella::cheney_baker_gc
)

create_stella_executable(04_live_list_traversal_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/04_live_list_traversal.c
    stella::cheney_baker_gc
)

create_stella_executable(05_shared_alias_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/05_shared_alias.c
    stella::cheney_baker_gc
)

create_stella_executable(07_keep_tail_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/07_keep_tail.c
    stella::cheney_baker_gc
)

create_stella_executable(08_nested_lists_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/08_nested_lists.c
    stella::cheney_baker_gc
)

create_stella_executable(09_closure_capture_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/09_closure_capture.c
    stella::cheney_baker_gc
)

create_stella_executable(10_keep_one_tree_branch_cheney_baker
    ${CMAKE_SOURCE_DIR}/examples/extra/10_keep_one_tree_branch.c
    stella::cheney_baker_gc
)
