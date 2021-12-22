@echo off

where msbuild >nul 2>nul
if %ERRORLEVEL% neq 0 goto SkipBuild

cl /nologo /D_CRT_SECURE_NO_WARNINGS /DUNICODE /Zi /O2 mandelbrot.c

goto Finished

:SkipBuild
echo "Compiler not present! Run using developer prompt"

:Finished
