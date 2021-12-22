#include "threading.h"

#include <stdio.h>
#include <stdint.h>

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

static uint32_t g_mt_result;
static mtx_t g_mutex;

static uint32_t g_result;

int thread_zero(void *ptr) {
	// This time we lock and unlock the mutex before accessing and updating it
	for (uint32_t index = 0; index < 50000; ++index) {
		mtx_lock(&g_mutex);
		g_mt_result += index;
		mtx_unlock(&g_mutex);
	}

	return 0;
}

int thread_one(void *ptr) {
	uint32_t result = 0;
	for (uint32_t index = 50000; index <= 100000; ++index) {
		mtx_lock(&g_mutex);
		g_mt_result += index;
		mtx_unlock(&g_mutex);
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

	mtx_init(&g_mutex, mtx_plain);

	thrd_create(&my_thread[0], thread_one, NULL);
	thrd_create(&my_thread[1], thread_zero, NULL);

	for (uint32_t index = 0; index < ArrayCount(my_thread); ++index) {
		thrd_join(my_thread[index], NULL);
	}

	mtx_destroy(&g_mutex);

	single_thread_add();

	printf("Multi-threaded result: %u\n", g_mt_result);
	printf("Single-threaded result: %u\n", g_result);

	return 0;
}
