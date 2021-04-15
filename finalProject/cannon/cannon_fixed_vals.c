#include <stdio.h>
#include <stdlib.h>

#define N 4
void naive_multiply(int **arrA, int **arrB, int **arrC);
void print_mtrx_shared_int(int ** matrix, char *name);

int main(int argc, char *argv) {
  int i, j, k;
  // Allocate Arrays
  int **arrA = (int **)malloc(sizeof(int *) * N);
  int **arrB = (int **)malloc(sizeof(int *) * N);
  int **arrC = (int **)malloc(sizeof(int *) * N);

  for (int i=0; i<N; i++)
  {
    arrA[i]=(int*)malloc(sizeof(int) * N);
    arrB[i]=(int*)malloc(sizeof(int) * N);
    arrC[i]=(int*)malloc(sizeof(int) * N);
  }

  // populate arrays
  int a[16] = {1, 5, 3, 4, 2, 2, 3, 2, 5, 3, 3, 3, 1, 2, 3, 5};
  int b[16] = {2, 1, 1, 1, 2, 1, 2, 1, 2, 3, 3, 3, 1, 2, 2, 1};
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      arrA[i][j] = a[i*N + j];
      arrB[i][j] = b[i*N + j];
      arrC[i][j] = 0;
    }
  }

  print_mtrx_shared_int(arrA, "A");
  print_mtrx_shared_int(arrB, "B");

  // BEGIN CANNON CODE

  

  naive_multiply(arrA, arrB, arrC);

  print_mtrx_shared_int(arrC, "C");

  exit(0);
}

// perform naive matrix multiplication
void naive_multiply(int **arrA, int **arrB, int **arrC){
  for (int i = 0; i < N; i++)
    for (int k = 0; k < N; k++)
      for (int j = 0; j < N; j++)
        arrC[i][j] += arrA[i][k] * arrB[k][j];
}

void print_mtrx_shared_int(int ** matrix, char *name){
  int i, k;
  printf("Matrix %s\n", name);
  for (i = 0; i < N; i++){
    for (k = 0; k < N; k++){
      printf("%-12d ", matrix[i][k]);
    }
    printf("\n");
  }
  printf("\n");
}
