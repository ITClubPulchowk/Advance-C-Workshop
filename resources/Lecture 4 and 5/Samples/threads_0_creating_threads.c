#include "threading.h"

#include <stdio.h>

// Entry point for another thread
int thread_main(void *arg) {
	printf("The cake is a lie.\n");
	return 0;
}

// Entry point for main thread
int main() {
	thrd_t my_thread;
	thrd_create(&my_thread, thread_main, NULL);

	printf("There are 1000 leaves in the pile.\n");

	return 0;
}
