cmake_minimum_required(VERSION 3.20)

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