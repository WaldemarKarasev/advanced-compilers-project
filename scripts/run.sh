#!/usr/bin/env bash

set -u 

EXECUTABLE_NAME=$1
ARG=$2

run_stella_executable() {
    excutable=$1
    arg=$2
    heap=$3
    for budget in 1 2 4; do
    echo "Heap size=$heap, budget=$budget"
    STELLA_GC_STATS=1 STELLA_GC_STEP_BUDGET=$budget \
        $excutable $arg $heap
    done
    echo
    echo
    echo

}

run_stella_executable $EXECUTABLE_NAME $ARG $((2**12))
run_stella_executable $EXECUTABLE_NAME $ARG $((2**13))
run_stella_executable $EXECUTABLE_NAME $ARG $((2**14))
run_stella_executable $EXECUTABLE_NAME $ARG $((2**15))
run_stella_executable $EXECUTABLE_NAME $ARG $((2**16))