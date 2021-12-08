#include "bump.h"
#include <stdint.h>

bump_context bumper;

INLINE void *align_ptr(void *ptr, size_t alignment){
    uintptr_t hld = ((uintptr_t)ptr + alignment - 1) & ~(alignment - 1);
    return (void *)hld;
}

void *bump(size_t size) {
    // msvc doesnt like pointer arithmetic on void * apparently
    uintptr_t tmp_cur = (uintptr_t)bumper.cur;
    uintptr_t tmp_base = (uintptr_t)bumper.base;
    if (tmp_cur + size <= tmp_base + bumper.max_size) {
        void *tmp = bumper.cur;
        tmp_cur += size;
        bumper.cur = (void *) tmp_cur;
        return tmp;
    }
    return NULL;
}

void *bump_aligned(size_t size, size_t alignment){
    uintptr_t aligned_cur = (uintptr_t)align_ptr(bumper.cur, alignment);
    uintptr_t tmp_cur = (uintptr_t)bumper.cur;
    uintptr_t tmp_base = (uintptr_t)bumper.base;
    size_t total_alloc_size = (aligned_cur - tmp_cur) + size;
    if (aligned_cur + total_alloc_size <= tmp_base + bumper.max_size) {
        tmp_cur += total_alloc_size;
        bumper.cur = (void *)tmp_cur;
        return (void *)aligned_cur;
    }
    return NULL;
}

void debump(size_t size){
    intptr_t db = (intptr_t)bumper.cur - size;
    bumper.cur = (db < (intptr_t)bumper.base) ? bumper.base : (void *)db;
}

bool init_bump_context(size_t max_size) {
    bumper.max_size = max_size;
    bumper.base = vm_alloc(max_size);
    bumper.cur  = bumper.base;
    return bumper.base != NULL;
}

void reset_bump_context() {
    bumper.cur  = bumper.base;
}

void end_bump_context() {
    vm_free(bumper.base, bumper.max_size);
    bumper.max_size = 0;
    bumper.base = NULL;
    bumper.cur  = NULL;
}

#if PF_LINUX
#include <sys/mman.h>
void *vm_alloc(size_t size) {
    void *ret = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ret == MAP_FAILED) return NULL;
    return ret;
}

bool vm_free(void *addr, size_t size) {
    return munmap(addr, size) == 0;
}
#elif PF_WINDOWS
#define VC_EXTRALEAN
#include <Windows.h>
#include <memoryapi.h>
void *vm_alloc(size_t size) {
    void *ret = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return ret;
}

bool vm_free(void *addr, size_t size) {
    return VirtualFree(addr, size, MEM_RELEASE);
}
#endif
