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

typedef uint8_t uint8;
typedef struct BumpAllocatorContext{
  void *mem;
  size_t size;
  
  void *current_ptr;
} BumpAllocatorContext;


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

void *bump_alloc( BumpAllocatorContext *context, size_t size ){
  void *ptr = align_ptr( context->current_ptr, DEFAULT_ALIGN );

  if ( (uintptr_t)( (uint8 *)ptr + size ) >= (uintptr_t)( (uint8 *)context->mem + context->size ) ){
    return NULL;
  }
  context->current_ptr = (uint8 *)ptr + size;
  return ptr;

}

void *bump_alloc_aligned( BumpAllocatorContext *context, size_t size, size_t align ){
  void *ptr = align_ptr( context->current_ptr, align );
  if ( (uintptr_t)( (uint8 *)ptr + size ) >= (uintptr_t)( (uint8 *)context->mem + context->size ) ){
    return NULL;
  }
  context->current_ptr = (uint8 *)ptr + size;
  return ptr;
}

void bump_allocator_free( BumpAllocatorContext *context, size_t size ){


  uint8 *x = (uint8 *)context->current_ptr - size;
  ptrdiff_t diff = x - (uint8 *)context->mem;

  context->current_ptr = ( diff < 0 )?context->mem:x ;
  return;
}

void bump_allocator_destroy( BumpAllocatorContext *context ){
  os_free( context->mem , context->size );
  return;
}

BumpAllocatorContext new_bump_allocator(size_t size){
  size = align_size(size,PAGE_SIZE);
  void *ptr = os_alloc( size );
  BumpAllocatorContext context = {
    .mem = ptr,
    .size = size,
    .current_ptr = ptr,
  };
  
  return context;
}




int main( int argc, char *argv[] ){
  
  assert( align_size( 3, 4 ) == 4 );
  assert( align_size( 5, 4 ) == 8 );
  assert( align_size( 1011, 1024 ) == 1024 );
  BumpAllocatorContext bump_allocator = new_bump_allocator( 1024*1024 );
  
  int *arr = bump_alloc( &bump_allocator, sizeof(int)*100 );

  assert( align_size( (size_t)(uintptr_t)arr, 8 ) == (size_t)(uintptr_t)arr );
  for ( int i = 0; i < 100; i++ ){
    arr[i] = i;
  }


  char *str = bump_alloc( &bump_allocator, 23 );
  assert( align_size( (size_t)(uintptr_t)str, 8 ) == (size_t)(uintptr_t)str);
  strcpy( str, "Hello");
  int *arrB = bump_alloc( &bump_allocator, sizeof(int)*100 );
  assert( align_size( (size_t)(uintptr_t)arrB, 8 ) == (size_t)(uintptr_t)arrB );
  for ( int i = 0; i < 100; i++ ){
    arrB[i] = i;
  }

  for ( int i = 0; i < 100; i++ ){
    printf("%d\n", arr[i] + arrB[i] );
  }


  int *mem2 = bump_alloc( &bump_allocator, 1000*1000*1000 );
  assert( mem2 == NULL );

  bump_allocator_free( &bump_allocator, sizeof(int)*100 );
  bump_allocator_free( &bump_allocator, sizeof(int)*100 );
  bump_allocator_free( &bump_allocator, sizeof(int)*100 );
  int *arrC = bump_alloc( &bump_allocator, sizeof(int)*100 );
  assert( align_size( (size_t)(uintptr_t)arrC, 8 ) == (size_t)(uintptr_t)arrC );
  for ( int i = 0; i < 100; i++ ){
    arrC[i] = i+1;
  }
  for ( int i = 0; i < 100; i++ ){
    printf("%d\n",arrC[i] );
  }


  bump_allocator_destroy( &bump_allocator );
  return 1;
}
