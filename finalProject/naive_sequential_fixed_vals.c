#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 3

// Allocate Arrays
int arrA[N][N];
int arrB[N][N];
int arrC[N][N];

int main(int argc, char *argv) {
  int i, j, k;

  // populate arrays
  int a[9] = {1, 5, 3, 4, 2, 2, 3, 2, 5};
  int b[9] = {2, 1, 1, 1, 2, 1, 2, 1, 2};

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      arrA[i][j] = a[i*N + j];
      arrB[i][j] = b[i*N + j];
      arrC[i][j] = 0;
    }
  }

  
  // perform naive matrix multiplication
  for (i = 0; i < N; i++)
    for (k = 0; k < N; k++)
      for (j = 0; j < N; j++) 
        arrC[i][j] += arrA[i][k] * arrB[k][j];

  // print resulting matrix
  // We can do this blockwise for MPI impl. See a2/act1
  // TODO: this should be CL flag toggleable
  for (i = 0; i < N; i++){
    for (k = 0; k < N; k++){
      printf("%-12d ", arrC[i][k]);
    }
    printf("\n");
  }

  exit(0);
}