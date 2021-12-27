@echo off

where clang >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipBuild

echo Building mandebrot
clang -D_CRT_SECURE_NO_WARNINGS -DUNICODE mandelbrot.c -O2 -o mandelbrot.exe

echo Building mandebrot simd
clang -D_CRT_SECURE_NO_WARNINGS -DUNICODE mandelbrot_simd.c -O2 -o mandelbrot_simd.exe

goto Finished

:SkipBuild
echo "Compiler not present! Run using developer prompt"

:Finished
