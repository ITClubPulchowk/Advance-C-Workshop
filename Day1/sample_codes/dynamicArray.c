#include <stdio.h>
#include <stdlib.h>


typedef struct {
    int* data;
    int size;
    int totalElements;
} Dynamic_Array;

void initArray(Dynamic_Array* dynamicArray, int size) {
    dynamicArray->size = size;
    dynamicArray->totalElements = 0;
    dynamicArray->data = (int*) malloc(sizeof(int) * dynamicArray->size);
}

void addElement(int data, Dynamic_Array* dynamicArray) {
    if (dynamicArray->size == dynamicArray->totalElements) {
        dynamicArray->size *= 2;
        dynamicArray->data = (int*) realloc(dynamicArray->data, sizeof(int) * (dynamicArray->size));
    }
    dynamicArray->data[(dynamicArray->totalElements)++] = data;
}

void resizeArray(Dynamic_Array* dynamicArray) {
        dynamicArray->size /= 4;
        dynamicArray->data = (int*) realloc(dynamicArray->data, sizeof(int) * (dynamicArray->size));
}

void removeLastElement(Dynamic_Array* dynamicArray) {
    dynamicArray->data[(dynamicArray->totalElements)--] = 0;
    if (dynamicArray->totalElements == dynamicArray->size / 4) {
        resizeArray(dynamicArray);
    }
}

void freeArray(Dynamic_Array* dynamicArray) {
    free(dynamicArray->data);
    dynamicArray->data = NULL;
    dynamicArray->size = dynamicArray->totalElements = 0;
}


int main() {
    Dynamic_Array dynamicArray;
    initArray(&dynamicArray, 5);

    for (int i = 0; i < 12; i++) {
        addElement(i, &dynamicArray);
    }
    for (int i = 0; i < dynamicArray.totalElements; i++) {
        printf("%d ", dynamicArray.data[i]);
    }
    printf("\n");
    for (int i = 0; i < 10; i++) {
        removeLastElement(&dynamicArray);
    }
    printf("%d %d\n", dynamicArray.size, dynamicArray.totalElements);
    for (int i = 0; i < dynamicArray.totalElements; i++) {
        printf("%d ", dynamicArray.data[i]);
    }
    free(dynamicArray.data);
    return 0;
}
