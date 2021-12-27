#ifdef _WIN32 
  #define PLATFORM_WINDOWS
#else
  #ifdef __linux__ 
    #define PLATFORM_LINUX
  #else
    #ifdef _WIN64
      #define PLATFORM_WINDOWS
    #endif
  #endif
#endif

#ifdef PLATFORM_LINUX
#include <sys/mman.h>
#endif

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <Memoryapi.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <assert.h>
#include <string.h>

#define DEFAULT_ALLOCATION_COUNT 5 
#define MAX(a,b) ( ( (a) > (b) )?(a):(b) )

typedef struct MemoryMapInfo {
  void *ptr;
  size_t size;
} MemoryMapInfo;

typedef struct FreeListHdr {
  struct FreeListHdr *next;
} FreeListHdr;

typedef struct FreeListContext{
  FreeListHdr *free_list;
  size_t chunk_size;
  size_t allocation_count;

  MemoryMapInfo *map_info;
  size_t map_len;
  size_t map_size;
} FreeListContext;

typedef uint8_t uint8;

static inline size_t align_size( size_t size, size_t align ){
  return ( size + ( align - 1 ) ) &  ~(align-1);
}

static inline void *align_ptr( void *ptr, size_t align ){
  uintptr_t uptr = (uintptr_t)ptr;
  uintptr_t aptr = ( uptr + ( align - 1 ) ) &  ~(align-1);
  return (void *)aptr;
}

#define PAGE_SIZE 4096  // 4kb = 1 page
#define DEFAULT_ALIGN 8 // 8 byte alignment by default
void *os_alloc( size_t size ){
  // TODO: align size to a multiple of page size.
#ifdef PLATFORM_LINUX
  /* the length argument does NOT need to be aligned to the page */
  void *mem = mmap( NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0 );
  
  if ( mem == MAP_FAILED ){
    return NULL;
  }

  return mem;
#endif

#ifdef PLATFORM_WINDOWS
  void *mem = VirtualAlloc(NULL,size, MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE );
  if ( mem == NULL ){ return NULL; }
  return mem;
#endif


}

void os_free( void *ptr, size_t size ){
#ifdef PLATFORM_LINUX
  munmap( ptr, size );
#endif

#ifdef PLATFORM_WINDOWS
  VirtualFree( ptr, 0, MEM_RELEASE);
#endif
  return;
}

/*
 * Returns a memory chunk of chunk_size bytes
 */
void *free_list_alloc( FreeListContext *context ){

  void *ptr = NULL;

  if ( context->free_list ){
    ptr = context->free_list;
    context->free_list = context->free_list->next;
  } else {
    
    // Check if we can store more memory mappings info
    if ( context->map_len == context->map_size-1 ){
      // return null otherwise. We could always expand the mapping array
      // to store more, but we will (most probably) run out of memory 
      // before filling this array. (with the default parameters at least).
      return NULL;
    }

    size_t alloc_size = context->allocation_count * context->chunk_size;
    alloc_size = align_size( alloc_size, PAGE_SIZE);


    void *mem = os_alloc(alloc_size);
    for ( int i = 0; i < DEFAULT_ALLOCATION_COUNT; i++ ){
      FreeListHdr *hdr = (void *)( (uint8*)mem + i *  context->chunk_size );
      hdr->next = (void *)( (uint8*)mem + ( i + 1 ) * context->chunk_size);
    }

    FreeListHdr *last = (void *)( (uint8 *)mem + ( DEFAULT_ALLOCATION_COUNT-1 ) * context->chunk_size );
    last->next = NULL;

    ptr = mem;
    context->free_list = ((FreeListHdr *)mem)->next;
    context->map_info[ context->map_len ] = (MemoryMapInfo){
      .ptr = mem,
      .size = alloc_size
    };

    context->map_len++;

  }
  return ptr;

}

void free_list_free( FreeListContext *context, void *mem ){
  FreeListHdr *hdr = mem;
  hdr->next = context->free_list;
  context->free_list = hdr;
}

void free_list_destroy( FreeListContext *context ){
  for ( size_t i = 0; i < context->map_len; i++ ){
#ifdef PLATFORM_LINUX 
    munmap( context->map_info[i].ptr, context->map_info[i].size );
#endif

#ifdef PLATFORM_WINDOWS 
    VirtualFree( context->map_info[i].ptr, 0, MEM_RELEASE );
#endif
  }
}



FreeListContext new_free_list( size_t chunk_size ){
  // Setup memory for storing the memory mappings
  
  MemoryMapInfo *info = os_alloc( PAGE_SIZE );
  

  chunk_size = align_size( chunk_size, DEFAULT_ALIGN );

  size_t alloc_size = chunk_size * DEFAULT_ALLOCATION_COUNT;
  alloc_size = align_size( alloc_size, PAGE_SIZE );




  FreeListContext context = {
    .free_list = os_alloc( alloc_size ),
    .chunk_size = chunk_size,
    .allocation_count = DEFAULT_ALLOCATION_COUNT * 2,
    .map_info = info,
    .map_size = PAGE_SIZE / sizeof(MemoryMapInfo),
    .map_len = 0
  };

  
  context.map_info[0] = (MemoryMapInfo){.ptr = context.free_list,
                                        .size = alloc_size };
  context.map_len++;

  void *mem = context.free_list;
  for ( int i = 0; i < DEFAULT_ALLOCATION_COUNT; i++ ){
    FreeListHdr *hdr = (void *)( (uint8*)mem + i * chunk_size );
    hdr->next = (void *)( (uint8*)mem + ( i + 1 ) * chunk_size);
  }

  FreeListHdr *last = (void *)( (uint8 *)mem + ( DEFAULT_ALLOCATION_COUNT-1 ) * chunk_size );
  last->next = NULL;

  return context;

}

void free_list_test(void){
  FreeListContext context = new_free_list( sizeof( int ) * 100 );

  FreeListHdr *hdr = context.free_list;
  while ( hdr != NULL ){
    hdr = hdr->next;
  }

  int *ptr_array[10];

  for ( int i = 0; i < 10; i++ ){
    ptr_array[i] = free_list_alloc( &context );
  }



  hdr = context.free_list;

  while ( hdr != NULL ){
    hdr = hdr->next;
  }

  for ( int i = 0; i < 10; i++ ){
    free_list_free( &context, ptr_array[i] );
  }

  int *arr = free_list_alloc( &context );
  for ( int i = 0; i < 34; i++ ){
    arr[i] = i + 1;
  }

  int *arr2 = free_list_alloc( &context );
  for ( int i = 0; i < 34; i++ ){
    arr2[i] = i + 1;
  }


  for ( int i = 0; i < 34; i++ ){
    printf("%d %d\n", arr[i], arr2[i] );
  }

  free_list_free( &context, arr );

  arr = free_list_alloc( &context );


  free_list_destroy( &context );
}


int main( int argc, char *argv[] ){
  
  free_list_test();
  return 1;
}

