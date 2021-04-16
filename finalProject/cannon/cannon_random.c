#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>

// #define N 4
// #define N 4096
#define SEED 42

// Declare global config variables
int N;
int DIAGNOSTICS;

// Forward declarations
void print_mtrx_shared_int(int ** matrix, const char *name);
void print_mtrx_shared_long(unsigned long long int ** matrix, const char *name);
void naive_multiply(int **arrA, int **arrB, unsigned long long int **arrC);

int main(int argc, char **argv) {
  int my_rank, nprocs;
  int i, j, k;
  double start_time, end_time;

  // Seed RNG
  srand(SEED);

  // Initialize MPI
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  //Process command-line arguments
  if (argc != 3) {
    fprintf(stderr,"Please provide the following args: N (matrix dimensionality), and a 1/0 diagnostic print flag.");
    MPI_Finalize();
    exit(0);
  }

  sscanf(argv[1],"%d",&N);
  sscanf(argv[2],"%d",&DIAGNOSTICS);
  
  //pointer to entire dataset
  double ** dataset;

// TODO: REMOVE
  if (DIAGNOSTICS){ printf("\nTOTAL RANKS: %d\n", nprocs); }

  // Select the largest max random integer that will not cause int overflow
  int safe_rand = (int) sqrt(ULLONG_MAX / N);
  if (RAND_MAX <= safe_rand){
    safe_rand = RAND_MAX;
  }

  // Allocate Arrays
  int **arrA = (int **)malloc(sizeof(int *) * N);
  int **arrB = (int **)malloc(sizeof(int *) * N);
  unsigned long long int **arrC = (unsigned long long int **)malloc(sizeof(unsigned long long int *) * N);
  for (int i=0; i<N; i++)
  {
    arrA[i]=(int*)malloc(sizeof(int) * N);
    arrB[i]=(int*)malloc(sizeof(int) * N);
    arrC[i]=(unsigned long long int*)malloc(sizeof(unsigned long long int) * N);
  }

  // populate arrays with random ints
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      arrA[i][j] = rand() % safe_rand;
      arrB[i][j] = rand() % safe_rand;
      arrC[i][j] = 0;
    }
  }

// PRINT INPUT ARRAYS
  if (DIAGNOSTICS){
    print_mtrx_shared_int(arrA, "A");
    print_mtrx_shared_int(arrB, "B");
  }

  start_time = MPI_Wtime();
// BEGIN MATRIX MULTIPLY
  naive_multiply(arrA, arrB, arrC);
// END MATRIX MULTIPLY
  end_time = MPI_Wtime();

  // PRINT FINAL ARRAY and TIMING
  if (DIAGNOSTICS){
    print_mtrx_shared_long(arrC, "C");
  }
  printf("TOTAL ELAPSED TIME: %lf\n", end_time - start_time);

  // CLEANUP
  MPI_Finalize();
  return(0);
}

// perform naive matrix multiplication
void naive_multiply(int **arrA, int **arrB, unsigned long long int **arrC){
  for (int i = 0; i < N; i++)
    for (int k = 0; k < N; k++)
      for (int j = 0; j < N; j++){
        unsigned long long int tmp = arrA[i][k] * arrB[k][j];
        if(tmp < 0){ printf("ERROR: product overflow\n");}
        arrC[i][j] += tmp;
      }
}

// print resulting matrix
// TODO: this should be CL flag toggleable
void print_mtrx_shared_int(int ** matrix, const char *name){
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

void print_mtrx_shared_long(unsigned long long int ** matrix, const char *name){
  int i, k;
  printf("Matrix %s\n", name);
  for (i = 0; i < N; i++){
    for (k = 0; k < N; k++){
      printf("%-12llu ", matrix[i][k]);
    }
    printf("\n");
  }
  printf("\n");
}
  // TODO: We can print distributed arrays blockwise for MPI impl. See a2/act1
