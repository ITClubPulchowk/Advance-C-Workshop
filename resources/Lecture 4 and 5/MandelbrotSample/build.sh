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

echo Building mandelbrot
${COMPILER} ./mandelbrot.c -o mandelbrot -O2 -lpthread

echo Building mandelbrot simd
${COMPILER} ./mandelbrot_simd.c -o mandelbrot_simd -O2 -lpthread
