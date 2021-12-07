@echo off

where msbuild >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipBuild

cl /nologo /D_CRT_SECURE_NO_WARNINGS /O2 runner.c

if not exist "debug" mkdir debug
echo Debug Builds
pushd debug
cl /nologo /Zi ../threads_0_creating_threads.c
cl /nologo /Zi ../threads_1_waiting.c
cl /nologo /Zi ../threads_2_args.c
cl /nologo /Zi ../threads_2_invalid_arg.c
cl /nologo /Zi ../threads_3_mt_add_attempt0.c
cl /nologo /Zi ../threads_3_mt_add_attempt1.c
cl /nologo /Zi ../threads_3_mt_add_attempt2.c
cl /nologo /Zi ../threads_4_impl_mutex.c
cl /nologo /Zi ../threads_4_impl_mutex_volatile.c
cl /nologo /Zi ../threads_5_custom_mutex.c
cl /nologo /Zi ../threads_6_no_mutex.c
cl /nologo /Zi ../threads_7_semaphore.c
cl /nologo /Zi ../threads_8_deadlock.c
popd

if not exist "optimized" mkdir optimized
echo Optimized Builds
pushd optimized
cl /nologo /O2 /Zi ../threads_0_creating_threads.c
cl /nologo /O2 /Zi ../threads_1_waiting.c
cl /nologo /O2 /Zi ../threads_2_args.c
cl /nologo /O2 /Zi ../threads_2_invalid_arg.c
cl /nologo /O2 /Zi ../threads_3_mt_add_attempt0.c
cl /nologo /O2 /Zi ../threads_3_mt_add_attempt1.c
cl /nologo /O2 /Zi ../threads_3_mt_add_attempt2.c
cl /nologo /O2 /Zi ../threads_4_impl_mutex.c
cl /nologo /O2 /Zi ../threads_4_impl_mutex_volatile.c
cl /nologo /O2 /Zi ../threads_5_custom_mutex.c
cl /nologo /O2 /Zi ../threads_6_no_mutex.c
cl /nologo /O2 /Zi ../threads_7_semaphore.c
cl /nologo /O2 /Zi ../threads_8_deadlock.c
popd

goto Finished

:SkipBuild
echo "Compiler not present! Run using developer prompt"

:Finished
