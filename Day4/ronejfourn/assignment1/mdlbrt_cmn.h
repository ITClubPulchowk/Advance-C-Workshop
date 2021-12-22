#define BMPMALLOC(size) bump(size)

#include "bump.c"
#include "bmp_lekhak.h"

#define IM_WIDTH 1280
#define IM_HEIGHT 720

#define MN_X_SCALE_MIN -4.00
#define MN_X_SCALE_MAX 4.00
#define MN_Y_SCALE_MIN -2.25
#define MN_Y_SCALE_MAX 2.25
#define MAX_ITER 10000

typedef struct {
    float r, g, b;
} color;

static color palette[] = {
    (color) {0.258824, 0.117647, 0.058824},
    (color) {0.098039, 0.027451, 0.101961},
    (color) {0.035294, 0.003922, 0.184314},
    (color) {0.015686, 0.015686, 0.286275},
    (color) {0.000000, 0.027451, 0.392157},
    (color) {0.047059, 0.172549, 0.541176},
    (color) {0.094118, 0.321569, 0.694118},
    (color) {0.223529, 0.490196, 0.819608},
    (color) {0.525490, 0.709804, 0.898039},
    (color) {0.827451, 0.925490, 0.972549},
    (color) {0.945098, 0.913726, 0.749020},
    (color) {0.972549, 0.788235, 0.372549},
    (color) {1.000000, 0.666667, 0.000000},
    (color) {0.800000, 0.501961, 0.000000},
    (color) {0.600000, 0.341176, 0.000000},
    (color) {0.415686, 0.203922, 0.011765},
};

static inline uint32_t rgb_to_hex(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF << 24 | r << 16 | g << 8 | b;
}

static inline float map_range(float pmin, float pmax, float nmin, float nmax, float value) {
    float ret = (value - pmin) * (nmax - nmin) / (pmax - pmin) + nmin;
    return ret;
}

static inline float lerp (float v0, float v1, float t) {
    return (1 - t) * v0 + t * v1;
}

static inline uint32_t get_color(uint32_t iter) {
    uint8_t r = 0, g = 0, b = 0;
    float t = (float)iter / MAX_ITER;
    color clr1 = palette[(iter    ) & 15];
    color clr2 = palette[(iter + 1) & 15];
    r = lerp(clr1.r, clr2.r, t) * 255;
    g = lerp(clr1.g, clr2.g, t) * 255;
    b = lerp(clr1.b, clr2.b, t) * 255;
    return rgb_to_hex(r, g, b);
}

#if PF_LINUX
#include <unistd.h>
#include <time.h>
typedef struct timespec timer;

#define get_core_count() sysconf(_SC_NPROCESSORS_CONF)

static inline void start_timing(timer *st) {
    clock_gettime(CLOCK_MONOTONIC, st);
}

static inline uint32_t end_timing(timer st) {
    timer end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return end.tv_sec * 1000 - st.tv_sec * 1000 + (end.tv_nsec - st.tv_nsec) / 1000000;
}
#elif PF_WINDOWS
#define VC_EXTRALEAN
#include <Windows.h>
#include <sysinfoapi.h>
typedef LARGE_INTEGER timer;

long get_core_count() {
    SYSTEM_INFO tmp;
    GetSystemInfo(&tmp);
    return tmp.dwNumberOfProcessors;
}

static inline void start_timing(timer *StartingTime) {
    QueryPerformanceCounter(StartingTime);
}

static inline uint32_t end_timing (timer StartingTime) {
    LARGE_INTEGER Frequency, EndingTime, ElapsedMicroseconds;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    return ElapsedMicroseconds.QuadPart / 1000;
}
#endif

