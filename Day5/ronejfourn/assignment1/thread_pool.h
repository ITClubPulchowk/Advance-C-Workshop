#include "threading.h"
#include <stdio.h>
#include <stdlib.h>

#define SI static inline

#define PF_LINUX 0
#define PF_WINDOWS 0

#if defined(__linux__)
    #undef PF_LINUX
    #define PF_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
    #undef PF_WINDOWS
    #define PF_WINDOWS 1
#else
    #error "UNKNOWN"
#endif

#if PF_LINUX
    #include <unistd.h>
    #define get_core_count() sysconf(_SC_NPROCESSORS_CONF)
#elif PF_WINDOWS
    #define VC_EXTRALEAN
    #include <Windows.h>
    #include <sysinfoapi.h>

    long get_core_count() {
        SYSTEM_INFO tmp;
        GetSystemInfo(&tmp);
        return tmp.dwNumberOfProcessors;
    }
#endif

int draw(void *arg);
SI uint32_t init_thrd_pool(uint32_t num) {
    uint32_t thrd_count = num ? num : get_core_count();

    thrd_t thr;
    for (int i = 0; i < thrd_count; i ++)
        thrd_create(&thr, draw, &i);
    return thrd_count;
}
