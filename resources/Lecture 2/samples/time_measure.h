#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H

// code works on linux and windows only, need a volunteer for mac lol
#ifdef __linux__

#include <time.h>
#include <stdio.h>

typedef struct timespec n_time;

static inline n_time get_current_time(){
    n_time s;
    if(clock_gettime(CLOCK_MONOTONIC, &s) == -1){
        perror("Couldn't measure time, something went wrong");
    };
    return s;
}

static inline double t_diff(n_time start, n_time end){
    return end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) * 1e-9;
}

#endif // __linux__

#ifdef _WIN32
#include <Windows.h>

typedef LARGE_INTEGER n_time;

static inline n_time get_current_time(){
	n_time s;
	QueryPerformanceCounter(&s);
	return s;
}

static inline double t_diff(n_time start, n_time end){
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return ((1000000.0 * (double)(end.QuadPart - start.QuadPart)) / (double)frequency.QuadPart) / 1000000.0;
}

#endif //_WIN32

#endif // TIME_MEASURE_H