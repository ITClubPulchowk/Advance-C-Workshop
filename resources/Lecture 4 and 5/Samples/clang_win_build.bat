@echo off

where clang >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipBuild

echo Building runner
clang -D_CRT_SECURE_NO_WARNINGS runner.c -O2 -o runner.exe

if not exist "debug" mkdir debug

echo Building Debug .\threads_0_creating_threads.c
clang .\threads_0_creating_threads.c -o .\debug\threads_0_creating_threads.exe

echo Building Debug .\threads_1_waiting.c
clang .\threads_1_waiting.c -o .\debug\threads_1_waiting.exe

echo Building Debug .\threads_2_args.c
clang .\threads_2_args.c -o .\debug\threads_2_args.exe

echo Building Debug .\threads_2_invalid_arg.c
clang .\threads_2_invalid_arg.c -o .\debug\threads_2_invalid_arg.exe

echo Building Debug .\threads_3_mt_add_attempt0.c
clang .\threads_3_mt_add_attempt0.c -o .\debug\threads_3_mt_add_attempt0.exe

echo Building Debug .\threads_3_mt_add_attempt1.c
clang .\threads_3_mt_add_attempt1.c -o .\debug\threads_3_mt_add_attempt1.exe

echo Building Debug .\threads_3_mt_add_attempt2.c
clang .\threads_3_mt_add_attempt2.c -o .\debug\threads_3_mt_add_attempt2.exe

echo Building Debug .\threads_4_impl_mutex.c
clang .\threads_4_impl_mutex.c -o .\debug\threads_4_impl_mutex.exe

echo Building Debug .\threads_4_impl_mutex_volatile.c
clang .\threads_4_impl_mutex_volatile.c -o .\debug\threads_4_impl_mutex_volatile.exe

echo Building Debug .\threads_5_custom_mutex.c
clang .\threads_5_custom_mutex.c -o .\debug\threads_5_custom_mutex.exe

echo Building Debug .\threads_6_no_mutex.c
clang .\threads_6_no_mutex.c -o .\debug\threads_6_no_mutex.exe

echo Building Debug .\threads_7_semaphore.c
clang .\threads_7_semaphore.c -o .\debug\threads_7_semaphore.exe

echo Building Debug .\threads_8_deadlock.c
clang .\threads_8_deadlock.c -o .\debug\threads_8_deadlock.exe

echo Building Debug .\threads_9_thread_local_motivation.c
clang .\threads_9_thread_local_motivation.c -o .\debug\threads_9_thread_local_motivation.exe

echo Building Debug .\threads_9_thread_local_usage.c
clang .\threads_9_thread_local_usage.c -o .\debug\threads_9_thread_local_usage.exe

echo Building Debug .\threads_10_mt_sort_thread_pool.c
clang .\threads_10_mt_sort_thread_pool.c -o .\debug\threads_10_mt_sort_thread_pool.exe

echo Building Debug .\joke.c
clang .\joke.c -o .\debug\joke.exe

Rem ##########################################################################

if not exist "optimized" mkdir optimized

echo Building Optimized .\threads_0_creating_threads.c
clang .\threads_0_creating_threads.c -o .\optimized\threads_0_creating_threads.exe -O2

echo Building Optimized .\threads_1_waiting.c
clang .\threads_1_waiting.c -o .\optimized\threads_1_waiting.exe -O2

echo Building Optimized .\threads_2_args.c
clang .\threads_2_args.c -o .\optimized\threads_2_args.exe -O2

echo Building Optimized .\threads_2_invalid_arg.c
clang .\threads_2_invalid_arg.c -o .\optimized\threads_2_invalid_arg.exe -O2

echo Building Optimized .\threads_3_mt_add_attempt0.c
clang .\threads_3_mt_add_attempt0.c -o .\optimized\threads_3_mt_add_attempt0.exe -O2

echo Building Optimized .\threads_3_mt_add_attempt1.c
clang .\threads_3_mt_add_attempt1.c -o .\optimized\threads_3_mt_add_attempt1.exe -O2

echo Building Optimized .\threads_3_mt_add_attempt2.c
clang .\threads_3_mt_add_attempt2.c -o .\optimized\threads_3_mt_add_attempt2.exe -O2

echo Building Optimized .\threads_4_impl_mutex.c
clang .\threads_4_impl_mutex.c -o .\optimized\threads_4_impl_mutex.exe -O2

echo Building Optimized .\threads_4_impl_mutex_volatile.c
clang .\threads_4_impl_mutex_volatile.c -o .\optimized\threads_4_impl_mutex_volatile.exe -O2

echo Building Optimized .\threads_5_custom_mutex.c
clang .\threads_5_custom_mutex.c -o .\optimized\threads_5_custom_mutex.exe -O2

echo Building Optimized .\threads_6_no_mutex.c
clang .\threads_6_no_mutex.c -o .\optimized\threads_6_no_mutex.exe -O2

echo Building Optimized .\threads_7_semaphore.c
clang .\threads_7_semaphore.c -o .\optimized\threads_7_semaphore.exe -O2

echo Building Optimized .\threads_8_deadlock.c
clang .\threads_8_deadlock.c -o .\optimized\threads_8_deadlock.exe -O2

echo Building Optimized .\threads_9_thread_local_motivation.c
clang .\threads_9_thread_local_motivation.c -o .\optimized\threads_9_thread_local_motivation.exe -O2

echo Building Optimized .\threads_9_thread_local_usage.c
clang .\threads_9_thread_local_usage.c -o .\optimized\threads_9_thread_local_usage.exe -O2

echo Building Optimized .\threads_10_mt_sort_thread_pool.c
clang .\threads_10_mt_sort_thread_pool.c -o .\optimized\threads_10_mt_sort_thread_pool.exe -O2

echo Building Optimized .\joke.c
clang .\joke.c -o .\optimized\joke.exe -O2

goto Finished

:SkipBuild
echo "Compiler not present! Run using developer prompt"

:Finished
