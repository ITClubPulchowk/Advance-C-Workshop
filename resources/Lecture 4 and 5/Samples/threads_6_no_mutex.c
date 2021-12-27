#include "threading.h"

#include <stdio.h>
#include <stdint.h>

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

static volatile uint32_t g_mt_result;
static uint32_t g_result;

int thread_zero(void *ptr) {
	for (uint32_t index = 0; index < 50000; ++index) {
		interlocked_add(&g_mt_result, index);
	}
	return 0;
}

int thread_one(void *ptr) {
	for (uint32_t index = 50000; index <= 100000; ++index) {
		interlocked_add(&g_mt_result, index);
	}
	return 0;
}

void single_thread_add() {
	for (uint32_t index = 0; index <= 100000; ++index) {
		g_result += index;
	}
}

int main() {
	thrd_t my_thread[2];

	thrd_create(&my_thread[0], thread_one, NULL);
	thrd_create(&my_thread[1], thread_zero, NULL);

	for (uint32_t index = 0; index < ArrayCount(my_thread); ++index) {
		thrd_join(my_thread[index], NULL);
	}

	single_thread_add();

	printf("Multi-threaded result: %u\n", g_mt_result);
	printf("Single-threaded result: %u\n", g_result);

	return 0;
}
