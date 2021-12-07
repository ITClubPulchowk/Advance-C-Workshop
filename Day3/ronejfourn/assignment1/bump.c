#include "bump.h"
#include <stdint.h>

bump_context bumper;

INLINE uintptr_t align_ptr(void *ptr, size_t alignment){
    return ((uintptr_t)ptr + alignment - 1) & ~(alignment - 1);
}

void *bump(size_t size) {
    if (bumper.cur + size <= bumper.base + bumper.max_size) {
        void *tmp = bumper.cur;
        bumper.cur += size;
        return tmp;
    }
    return NULL;
}

void *bump_aligned(size_t size, size_t alignment){
    uintptr_t aligned_cur = align_ptr(bumper.cur, alignment);
    size_t total_alloc_size = (aligned_cur - (uintptr_t)bumper.cur) + size;
    if (aligned_cur + total_alloc_size <= (uintptr_t)(bumper.base + bumper.max_size)) {
        bumper.cur += total_alloc_size;
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
#endif
