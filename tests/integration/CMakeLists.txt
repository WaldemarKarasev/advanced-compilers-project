cmake_minimum_required(VERSION 3.20)

# stella executables building from examples directory
include(cheney_baker_executables.cmake)

# default examples tests
function(add_chat_budget_test budget)
    set(test_name StellaChatInput10Budget${budget})

    add_test(
        NAME ${test_name}
        COMMAND $<TARGET_FILE:chat_cheney_baker> 10 4096
    )

    set_tests_properties(
        ${test_name}
        PROPERTIES
            ENVIRONMENT "STELLA_GC_STATS=1;STELLA_GC_STEP_BUDGET=${budget}"
            PASS_REGULAR_EXPRESSION "45"
            FAIL_REGULAR_EXPRESSION "completed cycles: 0"
    )
endfunction()

foreach(budget IN ITEMS 1 2 4)
    add_chat_budget_test(${budget})
endforeach()

add_test(
    NAME StellaDoubleInput5
    COMMAND $<TARGET_FILE:double_cheney_baker> 5
)
set_tests_properties(
    StellaDoubleInput5
    PROPERTIES PASS_REGULAR_EXPRESSION "10"
)

add_test(
    NAME StellaHistoryInput5
    COMMAND $<TARGET_FILE:history_cheney_baker> 5
)
set_tests_properties(
    StellaHistoryInput5
    PROPERTIES PASS_REGULAR_EXPRESSION "5"
)

add_test(
    NAME StellaPlaylistInput5
    COMMAND $<TARGET_FILE:playlist_cheney_baker> 5
)
set_tests_properties(
    StellaPlaylistInput5
    PROPERTIES PASS_REGULAR_EXPRESSION "15"
)

# examples/extra tests
add_test(
    NAME StellaExtraLargeThenSmall
    COMMAND $<TARGET_FILE:01_large_then_small_cheney_baker> 10 4096
)
set_tests_properties(
    StellaExtraLargeThenSmall
    PROPERTIES PASS_REGULAR_EXPRESSION "1"
)

add_test(
    NAME StellaExtraReplacementChurn
    COMMAND $<TARGET_FILE:02_replacement_churn_cheney_baker> 30 16384
)
set_tests_properties(
    StellaExtraReplacementChurn
    PROPERTIES
        ENVIRONMENT "STELLA_GC_STATS=1;STELLA_GC_STEP_BUDGET=1"
        PASS_REGULAR_EXPRESSION "30"
        FAIL_REGULAR_EXPRESSION "completed cycles: 0"
)

add_test(
    NAME StellaExtraTwoLiveLists
    COMMAND $<TARGET_FILE:03_two_live_lists_cheney_baker> 10 8192
)
set_tests_properties(
    StellaExtraTwoLiveLists
    PROPERTIES PASS_REGULAR_EXPRESSION "21"
)

add_test(
    NAME StellaExtraLiveListTraversal
    COMMAND $<TARGET_FILE:04_live_list_traversal_cheney_baker> 10 4096
)
set_tests_properties(
    StellaExtraLiveListTraversal
    PROPERTIES PASS_REGULAR_EXPRESSION "10"
)

add_test(
    NAME StellaExtraSharedAlias
    COMMAND $<TARGET_FILE:05_shared_alias_cheney_baker> 10 4096
)
set_tests_properties(
    StellaExtraSharedAlias
    PROPERTIES PASS_REGULAR_EXPRESSION "20"
)

add_test(
    NAME StellaExtraKeepTail
    COMMAND $<TARGET_FILE:07_keep_tail_cheney_baker> 30 16384
)
set_tests_properties(
    StellaExtraKeepTail
    PROPERTIES
        ENVIRONMENT "STELLA_GC_STATS=1;STELLA_GC_STEP_BUDGET=1"
        PASS_REGULAR_EXPRESSION "30"
        FAIL_REGULAR_EXPRESSION "completed cycles: 0"
)

add_test(
    NAME StellaExtraNestedLists
    COMMAND $<TARGET_FILE:08_nested_lists_cheney_baker> 10 16384
)
set_tests_properties(
    StellaExtraNestedLists
    PROPERTIES PASS_REGULAR_EXPRESSION "45"
)

add_test(
    NAME StellaExtraClosureCapture
    COMMAND $<TARGET_FILE:09_closure_capture_cheney_baker> 10 4096
)
set_tests_properties(
    StellaExtraClosureCapture
    PROPERTIES PASS_REGULAR_EXPRESSION "20"
)

add_test(
    NAME StellaExtraKeepOneTreeBranch
    COMMAND $<TARGET_FILE:10_keep_one_tree_branch_cheney_baker> 10 16384
)
set_tests_properties(
    StellaExtraKeepOneTreeBranch
    PROPERTIES
        ENVIRONMENT "STELLA_GC_STATS=1;STELLA_GC_STEP_BUDGET=1"
        PASS_REGULAR_EXPRESSION "8"
        FAIL_REGULAR_EXPRESSION "completed cycles: 0"
)