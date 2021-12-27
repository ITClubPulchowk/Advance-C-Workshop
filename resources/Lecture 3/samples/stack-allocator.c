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

typedef struct StackSizeInfo {
  struct StackSizeInfo *next;
} StackSizeInfo;


typedef struct StackAllocatorContext{
  void *mem;
  size_t size;
  
  void *current_ptr;

  StackSizeInfo *top;
} StackAllocatorContext;


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


#define PTR_COMPARE(x,OP,y) ( (uintptr_t)( x ) OP (uintptr_t)(y ) )

void *stack_alloc( StackAllocatorContext *context, size_t size ){
  void *ptr = align_ptr( context->current_ptr, DEFAULT_ALIGN );

  uint8 *ptr_end = (uint8*)ptr + size + sizeof(StackSizeInfo);

  if ( PTR_COMPARE(ptr_end, >= , (uint8*)context->mem + context->size ) ){
    return NULL;
  }

  StackSizeInfo *info = ptr;
  info->next = context->top;
  context->top = info;

  context->current_ptr = ptr_end;
  return (uint8 *)ptr + sizeof(StackSizeInfo);

}

void *stack_alloc_aligned( StackAllocatorContext *context, size_t size, size_t align ){
  /* align to 8 bytes to store the struct StackSizeInfo. */
  void* ptr = align_ptr( context->current_ptr, DEFAULT_ALIGN );
  
  /* This is the pointer that will be returned */
  uint8* ret_ptr = align_ptr( (uint8*)ptr + sizeof(StackSizeInfo), align );

  uint8 *ptr_end = ret_ptr + size;

  if ( PTR_COMPARE(ptr_end, >= , (uint8*)context->mem + context->size ) ){
    return NULL;
  }

  StackSizeInfo *info = ptr;
  info->next = context->top;
  context->top = info;

  context->current_ptr = ptr_end;
  return ret_ptr;
}

void stack_allocator_free( StackAllocatorContext *context, size_t size ){
  uint8 *x = (uint8 *)context->current_ptr - size;
  ptrdiff_t diff = x - (uint8 *)context->mem;

  context->current_ptr = ( diff < 0 )?context->mem:x ;
  return;
}

void stack_allocator_pop( StackAllocatorContext *context ){
  if ( context->top == NULL ){ return; }
  
  StackSizeInfo *next_ptr = context->top->next;
  if ( next_ptr == NULL ){
    context->current_ptr = context->mem;
  } else {
    context->current_ptr = context->top;
  }
  context->top = next_ptr;
  return;
}



void stack_allocator_destroy( StackAllocatorContext *context ){
  os_free( context->mem , context->size );
  return;
}

StackAllocatorContext new_stack_allocator(size_t size){
  // TODO: align the size to a multiple of page size.
  size = align_size( size, PAGE_SIZE ); 
  void *ptr = os_alloc( size );
  StackAllocatorContext context = {
    .mem = ptr,
    .size = size,
    .current_ptr = ptr,
    .top = NULL
  };
  
  return context;
}



static inline int check_ptr_alignment( void *ptr, size_t align ){
  return ( (uintptr_t)ptr & ( align - 1 ) ) == 0 ;
}


int stack_allocator_test_alignment(void){
  StackAllocatorContext allocator = new_stack_allocator( 120  * sizeof(int) );

  char *str = stack_alloc( &allocator, 23 );
  if( !check_ptr_alignment(str, 8) ){ return 0; }

  int *arr = stack_alloc( &allocator, sizeof(int)*80 );
  if( !check_ptr_alignment(arr, 8) ){ return 0; }

  int *arr3 = stack_alloc_aligned( &allocator, sizeof(int)*5, 32 );
  if( !check_ptr_alignment( arr3, 32) ){ return 0; }

  stack_allocator_destroy(&allocator);

  return 1;
}


int stack_allocator_test_alloc( void ){
  StackAllocatorContext stack_allocator = new_stack_allocator( 120  * sizeof(int) );
  
  int *arr = stack_alloc( &stack_allocator, sizeof(int)*80 );
  if( !check_ptr_alignment(arr, 8) ){ return 0; }

  for ( int i = 0; i < 80; i++ ){
    arr[i] = i;
  }

  char *str = stack_alloc( &stack_allocator, 23 );
  if( !check_ptr_alignment(str, 8) ){ return 0; }
  strcpy( str, "Hello");
  int *arr2 = stack_alloc_aligned( &stack_allocator, sizeof(int)*30, 16 );
  if( arr2 != NULL ){ return 0; }

  int *arr3 = stack_alloc_aligned( &stack_allocator, sizeof(int)*5, 32 );
  if( arr3 == NULL ){ return 0; }
  if( !check_ptr_alignment( arr3, 32) ){ return 0; }

  for ( int i = 0; i < 5; i++ ){
    arr3[i] = i;
  }

  for ( int i = 0; i < 5; i++ ){
    printf("%d\n", arr[i] + arr3[i] );
  }

  stack_allocator_pop( &stack_allocator);
  
  int *arr4 = stack_alloc_aligned( &stack_allocator, sizeof(int)*9, 32 );
  for ( int i = 0; i < 9; i++ ){
    arr4[i] = ( i+1 )*(i+1);
  }

  for ( int i = 0; i < 9; i++ ){
    printf("%d\n", arr4[i] );
  }

  while ( stack_allocator.top != NULL ){
    stack_allocator_pop( &stack_allocator );
  }

  printf("%s\n", str );

  if( stack_allocator.mem != stack_allocator.current_ptr ){ return 0; }

  stack_allocator_destroy( &stack_allocator );
  return 1;
}

void stack_allocator_test(void){
  assert( stack_allocator_test_alignment() && stack_allocator_test_alloc() );

}


int main( int argc, char *argv[] ){
  
  assert( align_size( 3, 4 ) == 4 );
  assert( align_size( 5, 4 ) == 8 );
  assert( align_size( 1011, 1024 ) == 1024 );

  stack_allocator_test();
  return 1;
}

