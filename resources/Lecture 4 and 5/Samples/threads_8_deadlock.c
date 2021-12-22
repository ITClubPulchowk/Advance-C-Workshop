#include <stdio.h>
#include "threading.h"

mtx_t M1, M2; 
int ran_by_threadA(void* nothing)
{
	mtx_lock(&M1);
	thrd_sleep_millisecs(500);
	fprintf(stderr, "Thread A passed first lock\n");
	mtx_lock(&M2);
	fprintf(stderr, "Thread A never reached here\n");
	mtx_unlock(&M2);
	mtx_unlock(&M1);
	return 0;

}
int ran_by_threadB(void* nothing)
{
	mtx_lock(&M2);
	thrd_sleep_millisecs(500);
	fprintf(stderr, "Thread B passed first lock\n");
	mtx_lock(&M1);
	fprintf(stderr, "Thread B never reached here\n");
	mtx_unlock(&M2);
	mtx_unlock(&M1);
	return 0;
}


int main() {
	mtx_init(&M1,mtx_plain);
	mtx_init(&M2,mtx_plain);

	thrd_t my_thread[2];
	thrd_create(&my_thread[0], ran_by_threadA, NULL);
	thrd_create(&my_thread[1], ran_by_threadB, NULL);

	thrd_join(my_thread[1],NULL);
	thrd_join(my_thread[0],NULL);
	return 0;
}