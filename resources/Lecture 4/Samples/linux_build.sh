#!/bin/bash

which gcc &> /dev/null
if [ $? == 0 ]; then
    COMPILER=gcc
else
    which clang &> /dev/null
    if [ $? == 0 ]; then
        COMPILER=clang
    else
        echo Error: Both GCC and CLANG compiler not detected.
        exit 
    fi
fi

echo Building runner
${COMPILER} ./runner.c -o runner -O2

####################################################################################################

mkdir -p debug

echo Building Debug ./threads_0_creating_threads.c
${COMPILER} ./threads_0_creating_threads.c -o ./debug/threads_0_creating_threads -lpthread

echo Building Debug ./threads_1_waiting.c
${COMPILER} ./threads_1_waiting.c -o ./debug/threads_1_waiting -lpthread

echo Building Debug ./threads_2_args.c
${COMPILER} ./threads_2_args.c -o ./debug/threads_2_args -lpthread

echo Building Debug ./threads_2_invalid_arg.c
${COMPILER} ./threads_2_invalid_arg.c -o ./debug/threads_2_invalid_arg -lpthread

echo Building Debug ./threads_3_mt_add_attempt0.c
${COMPILER} ./threads_3_mt_add_attempt0.c -o ./debug/threads_3_mt_add_attempt0 -lpthread

echo Building Debug ./threads_3_mt_add_attempt1.c
${COMPILER} ./threads_3_mt_add_attempt1.c -o ./debug/threads_3_mt_add_attempt1 -lpthread

echo Building Debug ./threads_3_mt_add_attempt2.c
${COMPILER} ./threads_3_mt_add_attempt2.c -o ./debug/threads_3_mt_add_attempt2 -lpthread

echo Building Debug ./threads_4_impl_mutex.c
${COMPILER} ./threads_4_impl_mutex.c -o ./debug/threads_4_impl_mutex -lpthread

echo Building Debug ./threads_4_impl_mutex_volatile.c
${COMPILER} ./threads_4_impl_mutex_volatile.c -o ./debug/threads_4_impl_mutex_volatile -lpthread

echo Building Debug ./threads_5_custom_mutex.c
${COMPILER} ./threads_5_custom_mutex.c -o ./debug/threads_5_custom_mutex -lpthread

echo Building Debug ./threads_6_no_mutex.c
${COMPILER} ./threads_6_no_mutex.c -o ./debug/threads_6_no_mutex -lpthread

echo Building Debug ./threads_7_semaphore.c
${COMPILER} ./threads_7_semaphore.c -o ./debug/threads_7_semaphore -lpthread

echo Building Debug ./threads_8_deadlock.c
${COMPILER} ./threads_8_deadlock.c -o ./debug/threads_8_deadlock -lpthread

#############################################################################################

mkdir -p optimized

echo Building Optimized ./threads_0_creating_threads.c
${COMPILER} ./threads_0_creating_threads.c -o ./optimized/threads_0_creating_threads -O2 -lpthread

echo Building Optimized ./threads_1_waiting.c
${COMPILER} ./threads_1_waiting.c -o ./optimized/threads_1_waiting -O2 -lpthread

echo Building Optimized ./threads_2_args.c
${COMPILER} ./threads_2_args.c -o ./optimized/threads_2_args -O2 -lpthread

echo Building Optimized ./threads_2_invalid_arg.c
${COMPILER} ./threads_2_invalid_arg.c -o ./optimized/threads_2_invalid_arg -O2 -lpthread

echo Building Optimized ./threads_3_mt_add_attempt0.c
${COMPILER} ./threads_3_mt_add_attempt0.c -o ./optimized/threads_3_mt_add_attempt0 -O2 -lpthread

echo Building Optimized ./threads_3_mt_add_attempt1.c
${COMPILER} ./threads_3_mt_add_attempt1.c -o ./optimized/threads_3_mt_add_attempt1 -O2 -lpthread

echo Building Optimized ./threads_3_mt_add_attempt2.c
${COMPILER} ./threads_3_mt_add_attempt2.c -o ./optimized/threads_3_mt_add_attempt2 -O2 -lpthread

echo Building Optimized ./threads_4_impl_mutex.c
${COMPILER} ./threads_4_impl_mutex.c -o ./optimized/threads_4_impl_mutex -O2 -lpthread

echo Building Optimized ./threads_4_impl_mutex_volatile.c
${COMPILER} ./threads_4_impl_mutex_volatile.c -o ./optimized/threads_4_impl_mutex_volatile -O2 -lpthread

echo Building Optimized ./threads_5_custom_mutex.c
${COMPILER} ./threads_5_custom_mutex.c -o ./optimized/threads_5_custom_mutex -O2 -lpthread

echo Building Optimized ./threads_6_no_mutex.c
${COMPILER} ./threads_6_no_mutex.c -o ./optimized/threads_6_no_mutex -O2 -lpthread

echo Building Optimized ./threads_7_semaphore.c
${COMPILER} ./threads_7_semaphore.c -o ./optimized/threads_7_semaphore -O2 -lpthread

echo Building Debug ./threads_8_deadlock.c
${COMPILER} ./threads_8_deadlock.c -o ./debug/threads_8_deadlock -O2 -lpthread
