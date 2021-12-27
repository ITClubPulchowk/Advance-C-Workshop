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

typedef struct FreeListInfo {
  size_t size; // Stores the size of the header + the size of free space
  struct FreeListInfo *next;
  uint8 paddding;
} FreeListInfo;

typedef struct AllocHeader {
  size_t size;
  size_t offset;
} AllocHeader;


typedef struct FreeListContext {
  void *mem;
  size_t size;

  void *current_ptr;
  FreeListInfo *free_list;
} FreeListContext;

typedef struct MemoryMapInfo {
  void *addr;
  size_t size;
} MemoryMapInfo;

#define PAGE_SIZE 4096  // 4kb = 1 page
#define DEFAULT_ALIGN sizeof(AllocHeader) // 16 byte alignment by default
#define MAX(a,b) ( ( (a) > (b) )?(a):(b) )




static inline size_t align_size( size_t size, size_t align ){
  return ( size + ( align - 1 ) ) &  ~(align-1);
}

static inline int check_ptr_alignment( void *ptr, size_t align ){
  return ( (uintptr_t)ptr & ( align - 1 ) ) == 0 ;
}

static inline void *align_ptr( void *ptr, size_t align ){
  uintptr_t uptr = (uintptr_t)ptr;
  uintptr_t aptr = ( uptr + ( align - 1 ) ) &  ~(align-1);
  return (void *)aptr;
}
void *os_alloc( size_t size ){
  /* the length argument does NOT need to be aligned to the page */
#ifdef PLATFORM_LINUX
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
  VirtualFree( ptr, size, MEM_DECOMMIT );
#endif
  return;
}


#define PTR_COMPARE(x,OP,y) ( (uintptr_t)( x ) OP (uintptr_t)(y ) )

#define free_list_alloc(x,y) free_list_alloc_aligned(x,y,DEFAULT_ALIGN)

void *make_alloc_block( void *block, size_t block_size, size_t align ){
  void *aligned_block = align_ptr( block, align );
  ptrdiff_t offset = (uint8*)aligned_block - (uint8*)block;
  
  if ( offset < sizeof(AllocHeader) ){
    aligned_block = (uint8*)aligned_block + align;
  }

  offset = (uint8*)aligned_block - (uint8*)block - sizeof(AllocHeader);
  AllocHeader *hdr = (void *)((uint8*)aligned_block - sizeof(AllocHeader));
#if 1
  hdr->size = block_size;
  hdr->offset = offset;
#endif
  
  return aligned_block;
}

void *free_list_alloc_aligned( FreeListContext *context, size_t size, size_t align ){
  
  size = align_size( size, DEFAULT_ALIGN );

  align = MAX(align, DEFAULT_ALIGN);

  // The size that we actually want to alloc
  // see: https://stackoverflow.com/questions/61853580/free-list-allocator-header-metadata
  // We add align as well, to account for extra required padding bytes (if any).
  size_t alloc_size = size + align + sizeof(AllocHeader);

  alloc_size = align_size( alloc_size,  DEFAULT_ALIGN );

  FreeListInfo *parent = context->free_list;
  FreeListInfo *iter = context->free_list;
  while ( iter ){

    if ( alloc_size <= iter->size ){

      // try to split the block if possible
      if ( iter->size - alloc_size > sizeof(FreeListInfo) ){
        // split the block
        FreeListInfo *split_block = (FreeListInfo *)( (uint8 *)iter + alloc_size );
        // This is always true since alloc_size is always aligned to DEFAULT_ALIGN
        assert( check_ptr_alignment( split_block, DEFAULT_ALIGN ) );
        
        split_block->next = iter->next;
        split_block->size = iter->size - alloc_size;
        
        if ( iter == parent ){ 
          /* iter is the HEAD node */
          context->free_list = split_block;
        } else {
          parent->next = split_block;
        }
         
      } else {
        // No split happens 
        // Unlink from the free list
        
        // if the node is the HEAD of the list
        if ( iter == parent ){
          context->free_list = iter->next;
        } else {
          parent->next = iter->next;
        }
      }


      return make_alloc_block( iter, alloc_size, align );

    }

    parent = iter;
    iter = iter->next;
    
  }
  
  size_t new_size = MAX(1024*1024*1024, alloc_size);
  void *mem = os_alloc( new_size );
  if ( !mem ){ return NULL; }

  // try to split the block if possible
  if ( new_size - alloc_size > sizeof(FreeListInfo) ){
    // split the block
    FreeListInfo *split_block = (FreeListInfo *)( (uint8 *)mem + alloc_size );
    // This is always true since alloc_size is always aligned to DEFAULT_ALIGN
    assert( check_ptr_alignment( split_block, DEFAULT_ALIGN ) );
    
    split_block->next = NULL;
    split_block->size = new_size - alloc_size;
    
    parent->next = split_block;
  }
  
  return make_alloc_block( mem, alloc_size, align );

}


void free_list_free( FreeListContext *context, void *ptr ){
  // Get the block info
  AllocHeader *hdr = (void *)((uint8 *)ptr - sizeof(AllocHeader));
  size_t block_size = hdr->size;
  
  // Pointer to the starting of the block
  void *block = (uint8 *)hdr - hdr->offset;

  // Turn the block into a linked list node
  FreeListInfo *info = block;
  info->size = block_size;
  
  // The pointer MUST and IS always be aligned
  assert( check_ptr_alignment( info, DEFAULT_ALIGN ) );
  
  
  // Find the position in the free list to add the block to
  // The free list is always in sorted order by the starting memory address
  // of the block
  
  FreeListInfo *iter = context->free_list;
  FreeListInfo *parent = iter;
  while( iter && PTR_COMPARE(info,>,iter ) ){
    parent = iter;
    iter = iter->next;
  }
  
  // Insertion at the head of the list
  if ( parent == iter ){
    context->free_list = info;
    info->next = iter;
  } else {
    parent->next = info;
    info->next = iter;
  }

  // Try to merge list nodes
  
  // Merge with parent
  if ( PTR_COMPARE( (uint8*)parent + parent->size, == , info ) ){
    parent->size += info->size;
    parent->next = info->next;

    info = parent;
  }
  
  // Merge with the NEXT node
  while ( PTR_COMPARE( (uint8*)info + info->size, == , info->next ) ){
    info->size += info->next->size;
    info->next = info->next->next;
  }

  
  // TODO: Maybe Check if any pages can be unmapped

}


FreeListContext new_free_list( size_t size ){
  //size = align_size(size,PAGE_SIZE);
  void *mem = os_alloc( size );
  FreeListContext context = {
    .mem = mem,
    .size = size,
    .current_ptr = mem
  };

  context.free_list = mem;

  context.free_list->size = size;
  context.free_list->next = NULL;

  return context;
}

void free_list_destroy( FreeListContext *context ){
//  os_free( context, context->size );
  
  
  void *page_addr_start = context->free_list;
  size_t page_addr_len = align_size( context->free_list->size, PAGE_SIZE );

  FreeListInfo *info = context->free_list;
#ifdef PLATFORM_LINUX
  if ( munmap( info, info->size ) == -1 ){
    fprintf(stderr, "munmap error!\n" );
    return;
  }
#endif

#ifdef PLATFORM_WINDOWS
#endif

  info = info->next;
  
  while ( info ){

    if ( ! ( PTR_COMPARE(info, >=, page_addr_start ) &&
             PTR_COMPARE(info, <=, (uint8*)page_addr_start + page_addr_len ) ) )
    {
      // the address range is outisde the previously unmapped region
      void *aligned_ptr = (void *)( (uintptr_t)info & ~(PAGE_SIZE-1) );
#ifdef PLATFORM_LINUX
      if ( munmap( aligned_ptr, info->size ) == -1 ){
        fprintf(stderr, "munmap failed on address %p of size %zu\n", aligned_ptr, info->size );
      }
#endif
      page_addr_start = info;
      page_addr_len = align_size( info->size, PAGE_SIZE );
    }
          
    info = info->next;

  }

  
}


int free_list_alloc_test2( void ){
  FreeListContext context= new_free_list( 120  * sizeof(int) );
  
  int *arr = free_list_alloc_aligned( &context, sizeof(int)*80,8 );
  if( !check_ptr_alignment(arr, 8) ){ return 0; }

  for ( int i = 0; i < 80; i++ ){
    arr[i] = i;
  }

  char *str = free_list_alloc_aligned( &context, 23,8 );
  if( !check_ptr_alignment(str, 8) ){ return 0; }
  strcpy( str, "Hello");
  int *arr2 = free_list_alloc_aligned( &context, sizeof(int)*30, 16 );
  if( arr2 == NULL ){ return 0; }

  int *arr3 = free_list_alloc_aligned( &context, sizeof(int)*5, 32 );
  if( arr3 == NULL ){ return 0; }
  if( !check_ptr_alignment( arr3, 32) ){ return 0; }

  for ( int i = 0; i < 5; i++ ){
    arr3[i] = i;
  }

  for ( int i = 0; i < 5; i++ ){
    printf("%d\n", arr[i] + arr3[i] );
  }

  int *arr4 = free_list_alloc_aligned( &context, sizeof(int)*9, 32 );
  for ( int i = 0; i < 9; i++ ){
    arr4[i] = ( i+1 )*(i+1);
  }

  for ( int i = 0; i < 9; i++ ){
    printf("%d\n", arr4[i] );
  }

  printf("%s\n", str );
  
  free_list_free( &context, arr );
  free_list_free( &context, arr2 );
  free_list_free( &context, arr3 );
  free_list_free( &context, arr4 );
  free_list_free( &context, str );
  return 1;
}
int free_list_alloc_test( void ){
  free_list_alloc_test2();
  FreeListContext context = new_free_list( PAGE_SIZE );

  int *arr1 = free_list_alloc_aligned( &context, sizeof(int)*100, 8 );

  int *arr2 = free_list_alloc_aligned( &context, sizeof(int)*1000, 32 );
  if ( !check_ptr_alignment( arr2, 32 ) ){ return 0; }  

  int *arr3 = free_list_alloc_aligned( &context, sizeof(int)*10, 8 );

  int *arr4 = free_list_alloc_aligned( &context, sizeof(int)*10, 16 );
  if ( !check_ptr_alignment( arr4, 16 ) ){ return 0; }  
  
  free_list_free( &context, arr3 );

  int *arr5 = free_list_alloc_aligned( &context, sizeof(int)*10, 32);
  if ( !check_ptr_alignment( arr5, 32 ) ){ return 0; }  

  free_list_free( &context, arr1 );
  free_list_free( &context, arr2 );

  int *arr6 = free_list_alloc_aligned( &context, sizeof(int)*10, 32 );
  if ( !check_ptr_alignment( arr6, 32 ) ){ return 0; }  
  
  free_list_free( &context, arr4 );
  free_list_free( &context, arr5 );
  free_list_free( &context, arr6 );

  return 1;
}


int main( int argc, char *argv[] ){
  
  assert( align_size( 3, 4 ) == 4 );
  assert( align_size( 5, 4 ) == 8 );
  assert( align_size( 1011, 1024 ) == 1024 );
  
  free_list_alloc_test();
  return 1;
}
