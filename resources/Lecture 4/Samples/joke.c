#include "threading.h"
#include <stdio.h>

int sleepAndPrint(void *n){
  int num = *(int*)n;
  thrd_sleep_millisecs(num * 1000);
  printf("%d, ", num);
  fflush(stdout);
}

int main(){
  int arr[] = { 1, 5, 3, 10, 4, 2};
  const int n = sizeof(arr)/sizeof(int);
  thrd_t threads[n];

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
