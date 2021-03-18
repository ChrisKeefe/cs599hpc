#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 2048

int arrA[N][N];
int arrB[N][N];
int arrC[N][N];

// TODO: Take N as param
int main(int argc, char *argv) {

int i, j, k;

  // populate arrays with random ints
  srand(time(NULL));
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      arrA[i][j] = rand();
      arrB[i][j] = rand();
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