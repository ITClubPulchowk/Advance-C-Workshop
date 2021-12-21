#include "threads_10_pool.h"

//
// Global Scratch buffer
//

static struct Scratchpad {
	uint8_t *ptr;
	uint8_t *cur;
	uint8_t *cap;
} scratchpad;

#define SCRATCHPAD_SIZE 1024 * 1024 * 1024 // 1GB

static void scratchpad_init() {
	scratchpad.ptr = malloc(SCRATCHPAD_SIZE);
	scratchpad.cur = scratchpad.ptr;
	scratchpad.cap = scratchpad.ptr + SCRATCHPAD_SIZE;
}

static void *scratchpad_alloc(size_t size) {
	uint8_t *ptr = NULL;
	uint8_t *end = scratchpad.cur + size;
	if (end <= scratchpad.cap) {
		ptr            = scratchpad.cur;
		scratchpad.cur = end;
	}
	return ptr;
}

typedef struct TempMem {
	uint8_t *ptr;
} TempMem;

static TempMem scratchpad_begin() {
	TempMem mem;
	mem.ptr = scratchpad.cur;
	return mem;
}

static void scratchpad_end(TempMem *mem) {
	scratchpad.cur = mem->ptr;
}

//
// Random number generation
//

// (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

static pcg32_random_t rng;

void pcg32_random_seed(uint64_t state) {
	rng.state = state;
}

uint32_t pcg32_random_r() {
	uint64_t oldstate = rng.state;
	// Advance internal state
	rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
	// Calculate output function (XSH RR), uses old state for max ILP
	uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
	uint32_t rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((rot & (~rot + 1u)) & 31));
}

//
//
//

void merge(uint32_t *array, int mid_point, int length, uint32_t *result) {
	int left_index = 0, right_index = mid_point;
	for (int index = 0; index < length; ++index) {
		if (left_index < mid_point && (right_index >= length || array[left_index] <= array[right_index])) {
			result[index] = array[left_index];
			left_index = left_index + 1;
		}
		else {
			result[index] = array[right_index];
			right_index = right_index + 1;
		}
	}
}

typedef struct {
	uint32_t *input;
	uint32_t *result;
	int mid_point, length;
} MergeBuffer;

void merge_threaded(void *arg) {
	MergeBuffer *buffer = arg;
	merge(buffer->input, buffer->mid_point, buffer->length, buffer->result);
}

inline void insertion_sort(uint32_t *array, int length) {
	int i, j, key;
	for (j = 1; j < length; j++) {
		key = array[j];
		i = j - 1;
		while (i >= 0 && array[i] > key) {
			array[i + 1] = array[i];
			i = i - 1;
		}
		array[i + 1] = key;
	}
}

void insertion_sort_threaded(void *arg) {
	MergeBuffer *buffer = arg;
	insertion_sort(buffer->result, buffer->length);
}

#define Minimum(x, y) (((x) < (y))?(x):(y))

uint32_t *merge_sort(uint32_t *array, int length, uint32_t *result) {
	const int MIN_INSERT_SORT_THRESHOLD = 128;

	if (length <= MIN_INSERT_SORT_THRESHOLD) {
		insertion_sort(array, length);
		return array;
	}

	TempMem temp = scratchpad_begin();

	MergeBuffer *buffer = scratchpad_alloc(sizeof(MergeBuffer) * (length / 2 + 1));

	int index = 0;
	for (int i = 0; i < length; i += MIN_INSERT_SORT_THRESHOLD, ++index) {
		int right = Minimum(i + MIN_INSERT_SORT_THRESHOLD, length);

		buffer[index].input     = NULL;
		buffer[index].result    = array + i;
		buffer[index].mid_point = 0;
		buffer[index].length    = right - i;
		thrd_pool_add(insertion_sort_threaded, buffer + index);
	}

	thrd_pool_wait();

	for (int width = MIN_INSERT_SORT_THRESHOLD; width < length; width *= 2) {
		index = 0;
		for (int i = 0; i < length; i += 2 * width, ++index) {
			int right = Minimum(i + 2 * width, length);

			buffer[index].input     = array + i;
			buffer[index].result    = result + i;
			buffer[index].mid_point = width;
			buffer[index].length    = right - i;
			thrd_pool_add(merge_threaded, buffer + index);
		}

		thrd_pool_wait();

		uint32_t *temp = result;
		result         = array;
		array          = temp;
	}

	scratchpad_end(&temp);

	return array;
}

//
//
//

#include <time.h>
#include <stdio.h>

int compare_func(const void *a, const void *b) {
	uint32_t va = *(uint32_t *)a;
	uint32_t vb = *(uint32_t *)b;

	if (va > vb) return 1;
	if (va < vb) return -1;
	return 0;
}

#ifndef VERIFY_SORT
#define VERIFY_SORT  0
#endif
#ifndef PRINT_NUMBER
#define PRINT_NUMBER 0
#endif

int main() {
	scratchpad_init();

	//
	// Random number generation
	//

	pcg32_random_seed(time(0));

	int length = 5000000; // <-- total numbers

	uint32_t *buffer = scratchpad_alloc(sizeof(uint32_t) * length * (2 + VERIFY_SORT));
	uint32_t *result = buffer + length;
	uint32_t *check  = result + length;

	for (int index = 0; index < length; ++index) {
		uint32_t random_number = pcg32_random_r(); // pcg32_random_r() % N <-- limit the number
		buffer[index] = random_number;
		result[index] = random_number;
	}

	if (VERIFY_SORT) {
		for (int index = 0; index < length; ++index) {
			check[index] = buffer[index];
		}
	}

	// Threaded sorting

	thrd_pool_create(UINT32_MAX); // <-- Use UINT32_MAX for using number of threads = number of CPU core
	// use other number to change the number of threads, use 0 to measure single thread performance

	uint32_t *sorted = merge_sort(buffer, length, result);

	qsort(check, length, sizeof(uint32_t), compare_func);

	if (VERIFY_SORT) {
		for (uint32_t index = 0; index < length; ++index) {
			if (check[index] != sorted[index]) {
				fprintf(stderr, "Failed at index: %u\n", index);
				exit(0);
			}
		}
		printf("\nVerified.\n");
	}

	if (PRINT_NUMBER) {
		for (uint32_t index = 0; index < length; ++index) {
			printf("%u ", sorted[index]);
		}
	}

	printf("\nFinished.\n");

	// If we have other work, we don't destroy thread pool,
	// we use it for doing other work parallelly, but in this case
	// we exit the threads (OS will cleanup our threads anyway,
	// we are only doing this to show that this procedure exists
	thrd_pool_destroy();

	return 0;
}
