#include "threading.h"

#include <stdio.h>
#include <stdint.h>

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

// Lets use this struct for passing number of strings to another thread
typedef struct {
	const char **strings;
	uint32_t string_count;
} Thread_Arg;

int thread_main(void *ptr) {
	// This came from the last argument to thrd_create function
	Thread_Arg *arg = ptr;

	// Loop and print all the strings
	for (uint32_t index = 0; index < arg->string_count; ++index) {
		printf("%s.\n", arg->strings[index]);
	}

	return 0;
}

int main() {
	const char *args[] = {
		"The cake is a lie",
		"People die when they are killed"
	};

	Thread_Arg thread_arg;
	thread_arg.strings = args;
	thread_arg.string_count = ArrayCount(args);

	thrd_t my_thread;
	// The last argument is passed to *thread_main* as the argument
	// The pointer (last argument) must be valid while the newly
	// created thread is using that argument
	thrd_create(&my_thread, thread_main, (void *)&thread_arg);

	printf("There are 1000 leaves in the pile.\n");

	// Waiting for another thread
	thrd_join(my_thread, NULL);

	return 0;
}
