#ifndef MATRIX_HELPERS_H
#define MATRIX_HELPERS_H

#include <stdlib.h>
#include <stdio.h>

#define RANDOM_LIM 0x32 // must be a power of two

// General matrix allocations and free methods

// Initialize 2D array with random values
int** random_initialize_2D_matrix(int** matrix, int rows, int cols){
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < cols; ++j){
            matrix[i][j] = rand() & (RANDOM_LIM - 1 )+ 1; // random number between 1 and 31, remember the modulo and bitwise AND hack?
        }
    }
}

// Allocate 2D matrix
int** allocate_2D_matrix(int rows, int cols){
    int** matrix= (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; ++i){
        matrix[i] = (int *)malloc(cols * sizeof(int *));
    }
    return matrix;
}

// Free allocated 2D matrix
void free_2D_matrix(int** matrix, int rows){
    for (int i = 0; i < rows; ++i){
        free(matrix[i]);
    }
    free(matrix);
}

// Print 2D Matrix
void print_2D_matrix(int** matrix, int rows, int cols){
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < cols; ++j){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


#endif