#include "threading.h"

#include <stdio.h>
#include <stdint.h>

int thread_main(void *ptr) {
    int *number = (int *)ptr;
    while (*number) {
        *number += 5;
        printf("%d ", *number);
        number++;
    }
    printf("\n");
	return 0;
}

int main() {
	thrd_t my_thread;

    {
        // Remove the quotes to make the program bug free

        // Here, *numbers* is valid only when the scope is valid
        // But since it takes time to create thread, before the 
        // main thread is begin created the *numbers* become invalid
        // and the child thread will use invalid memory (memory that is 
        // already freed) and cause memory corruption
        int numbers[] = {1, 2, 3, 4, 5, 0};
	    thrd_create(&my_thread, thread_main, (void *)numbers);
    }

    int numbers[] = {5 , 4, 3, 2, 1, 0};

	thrd_join(my_thread, NULL);

    for (int i = 0; i < 5; ++i)
        printf("%d ", numbers[i]);
    printf("\n");

	return 0;
}
