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

typedef struct {
    void *base, *cur;
    size_t max_size;
} bump_context;

bool init_bump_context(size_t max_size);
void reset_bump_context();
void end_bump_context();

void *bump(size_t size);
void *bump_aligned(size_t size, size_t alignment);
void debump(size_t size);

void *vm_alloc(size_t size);
bool vm_free(void *addr, size_t size);
