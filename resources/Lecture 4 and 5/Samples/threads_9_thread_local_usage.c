#include "threading.h"
#include <stdio.h>
#define SIZE 5

static char SCRATCH_CHAR_BUFFER[2048];

char *number_to_string(unsigned int number) {
  int tmp_number = number;
  int strlen=1;

  while ((int)tmp_number/10 != 0)
  {
    tmp_number = (int)tmp_number/10;
    strlen++;
  }
  tmp_number = number;

  do
  {
    *(SCRATCH_CHAR_BUFFER+strlen-1) = (tmp_number%10)+'0';
    tmp_number = (int) tmp_number / 10;
  } while(strlen--);

  return SCRATCH_CHAR_BUFFER;
}

int thread_func(void *number)
{
  char *array = number_to_string(*(int*)number);
  printf("Number: %u, string: %s\n", *(int*)number, array);
  return 0;
}

int main(void)
{
    thrd_t thr[SIZE];
    unsigned int arr[SIZE] = {112, 221, 21313, 213312, 123};

    /* create 5 threads. */
    for(int i = 0; i < SIZE; i++)
      thrd_create(&thr[i], thread_func, &arr[i]);

    for(int i = 0; i< SIZE; i++)
      thrd_join(thr[i], NULL);

}
