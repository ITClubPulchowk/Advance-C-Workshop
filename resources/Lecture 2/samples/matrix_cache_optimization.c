#include <stdio.h>
#include <string.h>
#include "time_measure.h"
#include "matrix_multiply.h"
#include "matrix_helpers.h"

#define SEED 1 // using the same seed so that tests are repeatable
#define N 1024

// Run multiplication code and measure time

double multiply_and_measure_time(int** matrix_A, int** matrix_B, int** matrix_C, void (*fp)(int**, int**, int**, int)){
    n_time start, end;

    start = get_current_time();
    (*fp)(matrix_A, matrix_B, matrix_C, N);
    end = get_current_time();

    return t_diff(start, end);
}

int main(){
    srand(SEED);
    int **matrix_A, **matrix_B, **matrix_C;

    matrix_A = allocate_2D_matrix(N, N);
    matrix_B = allocate_2D_matrix(N, N);
    matrix_C = allocate_2D_matrix(N, N);

    random_initialize_2D_matrix(matrix_A, N, N);
    random_initialize_2D_matrix(matrix_B, N, N);

    double time_ijk = multiply_and_measure_time(matrix_A, matrix_B, matrix_C, matrix_multiply_ijk_order);
    printf("Matrix multiply by ijk-order : %lf s\n", time_ijk);

    double time_ikj = multiply_and_measure_time(matrix_A, matrix_B, matrix_C, matrix_multiply_ikj_order);
    printf("Matrix multiply by ikj-order : %lf s\n", time_ikj);

    double time_jki = multiply_and_measure_time(matrix_A, matrix_B, matrix_C, matrix_multiply_jki_order);
    printf("Matrix multiply by jki-order : %lf s\n", time_jki);

    double time_jik = multiply_and_measure_time(matrix_A, matrix_B, matrix_C, matrix_multiply_jik_order);
    printf("Matrix multiply by jik-order : %lf s\n", time_jik);

    double time_kij = multiply_and_measure_time(matrix_A, matrix_B, matrix_C, matrix_multiply_kij_order);
    printf("Matrix multiply by kij-order : %lf s\n", time_kij);

    double time_kji = multiply_and_measure_time(matrix_A, matrix_B, matrix_C, matrix_multiply_kji_order);
    printf("Matrix multiply by kji-order : %lf s\n", time_kji);

    free_2D_matrix(matrix_A, N);
    free_2D_matrix(matrix_B, N);
    free_2D_matrix(matrix_C, N);
}