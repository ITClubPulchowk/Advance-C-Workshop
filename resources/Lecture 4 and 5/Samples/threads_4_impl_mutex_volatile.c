#include "threading.h"

#include <stdio.h>
#include <stdint.h>

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
	volatile uint32_t value;
} Mutex;

void mutex_init(Mutex *mutex) {
	mutex->value = 0;
}

void mutex_lock(Mutex *mutex) {
	// this is also known as spin lock
	while (mutex->value == 1) // if locked (i.e. being used by another thread) then wait
		;
	// @Thread-Safety, two threads don't sync this variable
	mutex->value = 1;
}

void mutex_unlock(Mutex *mutex) {
	mutex->value = 0;
}

static uint32_t g_mt_result;
static Mutex g_mutex;

static uint32_t g_result;

int thread_zero(void *ptr) {
	for (uint32_t index = 0; index < 50000; ++index) {
		mutex_lock(&g_mutex);
		g_mt_result += index;
		mutex_unlock(&g_mutex);
	}
	return 0;
}

int thread_one(void *ptr) {
	for (uint32_t index = 50000; index <= 100000; ++index) {
		mutex_lock(&g_mutex);
		g_mt_result += index;
		mutex_unlock(&g_mutex);
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

	mutex_init(&g_mutex);

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
