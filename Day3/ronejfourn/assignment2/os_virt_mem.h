#ifndef THIS_IS_VERY_STUPID
#define THIS_IS_VERY_STUPID
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PF_LINUX 0
#define PF_WINDOWS 0

#if defined(__linux__)
#undef PF_LINUX
#define PF_LINUX 1
#elif defined(_WIN32) || defined(_WIN64)
#undef PF_WINDOWS
#define PF_WINDOWS 1
#endif

#define PAGE_SIZE 4096
#define kilobytes(a) (a * 1024)
#define megabytes(a) (a * 1048576)
#define INLINE static inline

#if PF_LINUX
#include <sys/mman.h>
INLINE void *vm_alloc(size_t size) {
    void *ret = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ret == MAP_FAILED) return NULL;
    return ret;
}

INLINE bool vm_free(void *addr, size_t size) {
    return munmap(addr, size) == 0;
}
#elif PF_WINDOWS
#define VC_EXTRALEAN
#include <Windows.h>
#include <memoryapi.h>
INLINE void *vm_alloc(size_t size) {
    void *ret = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return ret;
}

INLINE bool vm_free(void *addr, size_t size) {
    return VirtualFree(addr, size, MEM_RELEASE);
}
#endif
#endif
