#ifndef VECTORLOGIC
#define VECTORLOGIC 1

// #include "mergelogic.c"
#include <stdlib.h>

struct Vector_list
{
    int * start;
    size_t sizeofeachelement;
    size_t array_length;
    size_t max_len;
};

struct Vector
{
    struct Vector_list * v;
};

void initialize_vector(struct Vector vector);
int vector_push(struct Vector vector, int  element);
void free_vector(struct Vector vector);
int get_element(struct Vector vector, size_t index);

int get_element(struct Vector vector, size_t index)
{
    if(vector.v->array_length >index)
        return vector.v->start[index];
    else
        return 0;
}

void initialize_vector(struct Vector vector)
{
    struct Vector_list * v = vector.v;
    v->array_length = 0;
    v->sizeofeachelement = sizeof(int);
    v->start = malloc(v->sizeofeachelement);
    v->max_len = 1;
}

int vector_push(struct Vector vector, int element)
{
    struct Vector_list * v = vector.v;

    if(v->array_length >= v-> max_len)
    {
        v->max_len*=2;        
        v->start = realloc((void *)(v->start),v-> max_len * v->sizeofeachelement);
        if(!v->start)
            exit(0);
    }

    v->start[v->array_length++] = element;
    return element;
}

void free_vector(struct Vector vector)
{
    free(vector.v->start);
}

struct Vector make_vector()
{
    struct Vector vector = {.v = malloc(sizeof(struct Vector_list))};
    initialize_vector(vector);
    return vector;
}
void destroy_vector(struct Vector vector)
{
    free_vector(vector);
    free(vector.v);
    vector.v = NULL;
}

void vector_merge_sort(struct Vector vector)
{
    mergesort(vector.v->start,vector.v->array_length);
}

int vector_test()
{
    char displaystring[10000];

    // Initializing a vector data structure
    struct Vector v = make_vector();


    vector_push(v, 10);
    vector_push(v,20);
    vector_push(v,50);
    vector_push(v,30);

    displaylist(displaystring,v.v->start, v.v->array_length);
    printf("\n%s\n",displaystring);

    vector_merge_sort(v);

    displaylist(displaystring,v.v->start, v.v->array_length);
    printf("\n%s\n",displaystring);



    free_vector(v);
}

#endif