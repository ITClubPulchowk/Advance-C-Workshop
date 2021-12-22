#include "threading.h"

#include <stdio.h>
#include <stdint.h>

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
	volatile uint32_t value;
	uint32_t max_value;
} Semaphore;

void sem_init(Semaphore *s, uint32_t max_value) {
	assert(max_value != 0);
	s->value = 0;
	s->max_value = max_value;
}

void sem_wait(Semaphore *s) {
	while (1) {
		// Make sure we read s->value only once before calling *interlocked_cmpxchg*.
		// This is important because next time we retrive the value like s->value,
		// the value have been already changed by another thread!!!!

		// When we compare we expect the old value to be the value we actuall read right now
		uint32_t expected_old_value = s->value;

		// The semaphore value has reached the max value, wait until the value gets decreased by some other thread
		if (expected_old_value == s->max_value) {
			continue;
		}
		
		// We want to put this value in the sempahore now
		// but s->value could have already changed at this point
		// because another thread could have ran **interlocked_cmpxchg** before us
		uint32_t new_value = expected_old_value + 1;

		uint32_t old_value = interlocked_cmpxchg(&s->value, new_value, expected_old_value);

		// If we get lucky the *old_value* will be the same as *expected_old_value*
		// which ensures that during retrival of s>value for *expected_old_value* before 
		// calling *interlocked_cmpxchg* and during the assignment for *new_value*
		// no other thread called *interlocked_cmpxchg* and we got to increase the value 
		// of the sem, yes at that point, we won!
		if (old_value == expected_old_value) {
			// We just have to try again to win if *old_value* != *expected_old_value*
			// because *old_value* not being equal to *expected_old_value* means that
			// some other thread won in the competition of calling *interlocked_cmpxchg*
			// before we could do it, sad :(
			break;
		}
	}
}

void sem_signal(Semaphore *s) {
	// Unlilke mutex where we simply assign zero to the mutex value like
	// mutex->value = 0;
	// we can't do this for semaphore, because multiple threads could
	// be calling the *sem_signal* function, and semaphore are not binary!

	// The reasoning for this function is the same as *sem_wait*
	while (1) {
		uint32_t expected_old_value = s->value;

		if (expected_old_value == 0) {
			continue;
		}

		uint32_t new_value = expected_old_value - 1;
		uint32_t old_value = interlocked_cmpxchg(&s->value, new_value, expected_old_value);

		if (old_value == expected_old_value) {
			break;
		}
	}
}

static uint32_t g_mt_result;
static Semaphore g_sem;

static uint32_t g_result;

int thread_zero(void *ptr) {
	for (uint32_t index = 0; index < 50000; ++index) {
		sem_wait(&g_sem);
		g_mt_result += index;
		sem_signal(&g_sem);
	}
	return 0;
}

int thread_one(void *ptr) {
	for (uint32_t index = 50000; index <= 100000; ++index) {
		sem_wait(&g_sem);
		g_mt_result += index;
		sem_signal(&g_sem);
	}
	return 0;
}

void single_thread_add() {
	for (uint32_t index = 0; index <= 100000; ++index) {
		g_result += index;
	}
}

// do -DSEM_COUNT=2 to check what happens if you increase the sem count to 2
#ifndef SEM_COUNT
#define SEM_COUNT 1
#endif

int main() {
	thrd_t my_thread[2];

	sem_init(&g_sem, SEM_COUNT);

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
