#include "os_virt_mem.h"

struct {
    void *base, *cur;
    size_t max_size;
} bump_context;

INLINE void *align_ptr(void *ptr, size_t alignment){
    uintptr_t hld = ((uintptr_t)ptr + alignment - 1) & ~(alignment - 1);
    return (void *)hld;
}

void *bump(size_t size) {
    // msvc doesnt like pointer arithmetic on void * apparently
    uintptr_t tmp_cur = (uintptr_t)bump_context.cur;
    uintptr_t tmp_base = (uintptr_t)bump_context.base;
    if (tmp_cur + size <= tmp_base + bump_context.max_size) {
        void *tmp = bump_context.cur;
        tmp_cur += size;
        bump_context.cur = (void *) tmp_cur;
        return tmp;
    }
    return NULL;
}

void *bump_aligned(size_t size, size_t alignment){
    uintptr_t aligned_cur = (uintptr_t)align_ptr(bump_context.cur, alignment);
    uintptr_t tmp_cur = (uintptr_t)bump_context.cur;
    uintptr_t tmp_base = (uintptr_t)bump_context.base;
    size_t total_alloc_size = (aligned_cur - tmp_cur) + size;
    if (aligned_cur + total_alloc_size <= tmp_base + bump_context.max_size) {
        tmp_cur += total_alloc_size;
        bump_context.cur = (void *)tmp_cur;
        return (void *)aligned_cur;
    }
    return NULL;
}

void debump(size_t size){
    intptr_t db = (intptr_t)bump_context.cur - size;
    bump_context.cur = (db < (intptr_t)bump_context.base) ? bump_context.base : (void *)db;
}

bool init_bump_context(size_t max_size) {
    bump_context.max_size = max_size;
    bump_context.base = vm_alloc(max_size);
    bump_context.cur  = bump_context.base;
    return bump_context.base != NULL;
}

void reset_bump_context() {
    bump_context.cur  = bump_context.base;
}

void end_bump_context() {
    vm_free(bump_context.base, bump_context.max_size);
    bump_context.max_size = 0;
    bump_context.base = NULL;
    bump_context.cur  = NULL;
}
