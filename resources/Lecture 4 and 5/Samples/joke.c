#include "threading.h"
#include <stdio.h>

#define ArrayCount(arr) (sizeof(arr)/sizeof((arr)[0]))

int sleepAndPrint(void *n){
  int num = *(int*)n;
  thrd_sleep_millisecs(num * 1000);
  printf("%d, ", num);
  fflush(stdout);
  return 0;
}

int main(){
  int arr[] = { 1, 5, 3, 10, 4, 2};
  thrd_t threads[ArrayCount(arr)];

  int n = ArrayCount(arr);

  for (int i=0; i<n; i++){
    thrd_create(&threads[i], sleepAndPrint, &arr[i]);
  }

  for (int i=0; i<n; i++){
    thrd_join(threads[i], NULL);
  }
  printf("\n");
  return 0;
}

// 1,2,3,4,5,10
