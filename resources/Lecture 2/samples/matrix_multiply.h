#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H

void matrix_multiply_ijk_order(int **mat_A, int **mat_B, int **mat_res, int N) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        mat_res[i][j] += mat_A[i][k] * mat_B[k][j];
      }
    }
  }
}

void matrix_multiply_ikj_order(int **mat_A, int **mat_B, int **mat_res, int N) {
  for (int i = 0; i < N; ++i) {
    for (int k = 0; k < N; ++k) {
      for (int j = 0; j < N; ++j) {
        mat_res[i][j] += mat_A[i][k] * mat_B[k][j];
      }
    }
  }
}

void matrix_multiply_jki_order(int **mat_A, int **mat_B, int **mat_res, int N) {
  for (int j = 0; j < N; ++j) {
    for (int k = 0; k < N; ++k) {
      for (int i = 0; i < N; ++i) {
        mat_res[i][j] += mat_A[i][k] * mat_B[k][j];
      }
    }
  }
}

void matrix_multiply_jik_order(int **mat_A, int **mat_B, int **mat_res, int N) {
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      for (int k = 0; k < N; ++k) {
        mat_res[i][j] += mat_A[i][k] * mat_B[k][j];
      }
    }
  }
}

void matrix_multiply_kij_order(int **mat_A, int **mat_B, int **mat_res, int N) {
  for (int k = 0; k < N; ++k) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        mat_res[i][j] += mat_A[i][k] * mat_B[k][j];
      }
    }
  }
}


void matrix_multiply_kji_order(int **mat_A, int **mat_B, int **mat_res, int N) {
  for (int k = 0; k < N; ++k) {
    for (int j = 0; j < N; ++j) {
      for (int i = 0; i < N; ++i) {
        mat_res[i][j] += mat_A[i][k] * mat_B[k][j];
      }
    }
  }
}


#endif