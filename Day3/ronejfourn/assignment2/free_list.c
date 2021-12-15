#include "os_virt_mem.h"

#define MIN_VIRT_SIZE PAGE_SIZE
#define MIN_CHUNK_SIZE 256
// arbitrary maximum no. of mappings
#define MAXM_MAP 4096

typedef struct chunk {
    struct chunk *next;
} chunk;

struct {
    chunk top;
    chunk *list;
    size_t list_size, chunk_size;
    size_t map_count;
    void *map_track[MAXM_MAP];
} list_context;

void list_add_page(){
    list_context.list->next = vm_alloc(list_context.list_size);
    void *top = list_context.list->next;
    list_context.map_track[list_context.map_count] = top;
    list_context.map_count ++;

    size_t a = 0;
    while (a < list_context.list_size) {
        list_context.list->next = (void *)((char *)top + a);
        list_context.list = list_context.list->next;
        list_context.list->next = NULL;
        a += list_context.chunk_size;
    }
}

void init_list_context(size_t max_size, size_t chunk_size) {
    list_context.list_size  = max_size < MIN_VIRT_SIZE ? MIN_VIRT_SIZE : max_size;
    list_context.chunk_size = chunk_size < MIN_CHUNK_SIZE ? MIN_CHUNK_SIZE : chunk_size;
    list_context.map_count = 0;
    list_context.list = &list_context.top;
    list_add_page();
}

void *lalloc() {
    if (!list_context.top.next) {
        if (list_context.map_count < MAXM_MAP)
            list_add_page();
        else
            return NULL;
    }
    void *ret = list_context.top.next;
    list_context.top.next = list_context.top.next->next;
    return ret;
}

void lfree(void *ptr) {
    void *tmp = list_context.top.next;
    list_context.top.next = ptr;
    list_context.top.next->next = tmp;
}

void end_list_context() {
    for (int i = 0; i < list_context.map_count; i ++)
        vm_free(list_context.map_track[i], list_context.list_size);
}
