# Lecture 3 - Virtual Page allocation, Allocators and Scratchpad

## Syllabus:

* Virtual Page Allocation System Calls
* More on CRT Generic Allocator
* Stack Allocator
* Bump Allocator (Memory Arenas)
* Free List Allocator
* Scratchpad memory

# Advanced C Workshop - Assignment 4

In this assignment, you will be implementing the Bump Allocator and the Free List Allocator as discussed in the 
lecture and apply those allocators to program(s) you have previously completed in the workshop. 
Since some of the code in this assignment will be platform specific, i.e, using `mmap` in Linux or 
`VirtualAlloc` in Windows, be sure to detect the OS using `#ifdef`s and exclude code that is not available in 
other platforms. For example,

```C
#ifdef PLATFORM_LINUX 
//... code that runs in linux goes here
#endif

#ifdef PLATFORM_WINDOWS
//... code that runs in Windows goes here
#endif
``` 
You can find preprocessor defines for various operating systems [here](https://sourceforge.net/p/predef/wiki/OperatingSystems/).
You only need to look at the `Linux` and `Windows` sections.

Before starting the assignment, be sure to revise your pointer arithmetic and check out `unitptr_t` and `ptrdiff_t`!

# Merge Sort Using Bump Allocator
Implement the Bump Allocator as discussed in the lecture, and use that algorithm to re-write the merge sort
program, which you have (hopefully) completed in the previous assignment. The allocator will need to have all 
the functionalities discussed in the lecture, namely:

* An alloc function that necessarily takes a `size` argument and returns a suitably aligned memory to the caller
* An alloc function that necessarily takes `size` as well as an `alignment` argument and returns memory that is aligned to the
  passed alignment value.
* A reset function that that removes all the allocations
* A free function that frees memory from the top of the allocator
* A destroy function that frees all the memory allocated from the OS.

Also, find and print the time difference between your previous implementation of merge sort and the current merge sort implementation.

### Bonus points: Do the same, but by implementing a stack allocator!

# BSTs - Part II
This problem consists of three input files:
* `data.txt` which contains randomly generated strings of length 0 < length < 128. Each line contains a single string. The strings will be used as `key` values for storage in the BST.
* `delete.txt`: Each line in this file contains a single string, which is to be deleted from the BST.
* `search.txt`: Each line in this file contains a single string, which to to be queried in the BST.

Simply put, first read the data from `data.txt` into a BST. Then, read the data from `delete.txt` and delete all the entries in the
BST that match the values in `delete.txt`. Finally, query the values of `search.txt` in the BST and store the result in an 
output file `result.txt`. Each line of `result.txt` must be either `Found` or `Not Found` depending on whether the 
corresponding line in `search.txt` was found in BST or not.

You **must** use a free list allocator to perform insertions and deletions in the BST. The free list allocator must have all
the functionalities as discussed in the lecture, namely:
* An `alloc` function that returns a suitably aligned memory.
* A `free` function that frees memory
* A `destroy` function that frees all the mappings back to the OS.

Note that you might end up creating multiple mapping from the OS (i.e. multiple calls to `VirtualAlloc` or `mmap`). Thus, keep track of 
all the mappings that you have created so far, so that you can later free them. You can use a fixed-size array to store the mapping information. Also, note that you **do not** have to implement an aligned allocating function.

### Bonus points: Find the time difference in executing by using malloc instead of the free list allocator!

