#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

// declare global config variables
int DIM;
int DIAGNOSTICS;
int BLOCKSIZE;

// forward declarations
int one_rank_left(int startCol, int startRow, int blockDim);
int one_rank_up(int startRank, int nprocs, int blockDim);
void naive_multiply(int **my_arrA, int **my_arrB, int **my_arrC);
void print_chunk(int **arr, int localDIM);
void print_dist_mat(int **arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world);

int main(int argc, char **argv) {
  int my_rank, nprocs, N, localN, localDIM, blockDIM, rowOffset, colOffset;
  int myProcRow, myProcCol, localStartIdx, gridRowOffset, gridColOffset, rankLeft, rankUp;
  int i, j, k, tmp, print_rank;
  double start_time, end_time;

  // Initialize MPI
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  float nprocs_check = sqrt(nprocs);
  if (nprocs_check != (int) nprocs_check)
  {
    printf("\nCannon's algorithm requires a square number of processors.\n");
    MPI_Finalize();
    exit(0);
  }

  //Process command-line arguments
  if (argc != 4) {
    fprintf(stderr,"Please provide the following args: DIM (matrix dimensionality), the BLOCKSIZE at which component submatrices will be multiplied naively, and a 1/0 diagnostic print flag.");
    MPI_Finalize();
    exit(0);
  }
  // DIM is the length of one dimension of the square matrices being multiplied
  sscanf(argv[1],"%d",&DIM);
  // BLOCKSIZE is the side-length of the smallest unit of multiplication
  // i.e. a cache-sized matrix which will be naively multiplied
  sscanf(argv[2], "%d", &BLOCKSIZE);
  sscanf(argv[3],"%d",&DIAGNOSTICS);

  // TODO: test behavior with nprocs = 3, 4, 5

  // Determine the dimensions of the data for each rank:
  // The number of elements of the matrix
  N = DIM * DIM;
  // The number of elements of each rank's chunk of the matrix
  localN = N / nprocs;
  // The dimensionality of each rank's chunk of the matrix
  localDIM = sqrt(localN);

  rowOffset = DIM;
  colOffset = 1;
  // TODO: remove?
  gridRowOffset = rowOffset * localDIM;
  gridColOffset = colOffset * localDIM;

  // the number of blocks on one axis of the matrix
  blockDIM = DIM / localDIM;
  // coordinates of the rank in our matrix of processors
  myProcRow = my_rank / blockDIM;
  myProcCol = my_rank % blockDIM;

  if (DIM < 1 || BLOCKSIZE < 1 || BLOCKSIZE > localDIM)
  {
    printf("\nDIM is invalid or BLOCKSIZE is invalid\n");
    MPI_Finalize();
    exit(0);
  }

  // Allocate Arrays
  // pointer to local subsets of the data
  int **my_arrA = (int **)malloc(sizeof(int *) * localDIM);
  int **my_arrB = (int **)malloc(sizeof(int *) * localDIM);
  int **my_arrC = (int **)malloc(sizeof(int *) * localDIM);

  for (int i=0; i<localDIM; i++)
  {
    my_arrA[i]=(int*)malloc(sizeof(int) * localDIM);
    my_arrB[i]=(int*)malloc(sizeof(int) * localDIM);
    my_arrC[i]=(int*)malloc(sizeof(int) * localDIM);
  }

  // populate arrays in a distributed manner
  // int a[16] = {1, 5, 3, 4, 2, 2, 3, 2, 5, 3, 3, 3, 1, 2, 3, 5};
  // int b[16] = {2, 1, 1, 1, 2, 1, 2, 1, 2, 3, 3, 3, 1, 2, 2, 1};
  int a[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  int b[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  localStartIdx = myProcRow * gridRowOffset + myProcCol * gridColOffset;
// printf("r %d startIdx %d\n", my_rank, localStartIdx);
  for (i = 0; i < localDIM; i++) {
    for (j = 0; j < localDIM; j++) {
      tmp = localStartIdx + i * rowOffset + j * colOffset;
      my_arrA[i][j] = a[tmp];
      my_arrB[i][j] = b[tmp];
      my_arrC[i][j] = 0;
    }
  }

  // display matrix A by sequentially printing each block
  if(DIAGNOSTICS){
    print_dist_mat(my_arrA, "A", localDIM, nprocs, my_rank, MPI_COMM_WORLD);
    // print_dist_mat(my_arrB, "B", localDIM, nprocs, my_rank, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  // BEGIN CANNON CODE
  // Allocate send and receive buffers for communications TODO: deal with int overflow
  MPI_Request req;
  int *send_buff = (int *)malloc(sizeof(int) * localDIM);
  // int *recv_buff = (int *)malloc(sizeof(int) * localDIM);

  // Initial matrix shuffle
  for (i = 0; i < localDIM; i++){
    // get ranks for left shift and up shift
    // oneRankLeft = ((myProcCol + blockDIM - 1) % blockDIM) + (myProcRow * blockDIM);
    // oneRankUp = (my_rank + nprocs - blockDIM) % nprocs;
    rankLeft = one_rank_left(myProcCol, myProcRow, blockDIM);
    rankUp = one_rank_up(my_rank, nprocs, blockDIM);

    // map i to its position in the full dataset
    int rowInFullMatrix = myProcRow * localDIM + i;
    int colInFullMatrix = myProcCol * localDIM + i;

    // get the number of values we'll send, max is number of values local to this row/col
    int nValsToSend = (rowInFullMatrix < localDIM) ? rowInFullMatrix : localDIM;
    printf("R: %d nValsToSend %d\n", my_rank, nValsToSend);

    if (nValsToSend != 0){

// TODO: We'll only ever send between 1 and localDIM values left, BUT we might send them
// left by more than one block, and may have to split the shift to more than one bloc

      // shift row A[X] left X cols:
      // Copy i values into send buffer
      memcpy(send_buff, (void *)my_arrA[i], sizeof(int) * nValsToSend);

      // Isend buffer left
      MPI_Isend(send_buff, rowInFullMatrix, MPI_INT, rankLeft, 0, MPI_COMM_WORLD, &req);

      // shift remaining local values left i places
      for (int j = 0; j < localDIM - rowInFullMatrix; j++){
        my_arrA[i][j] = my_arrA[i][j+i];
      }
      // Blocking receive directly into right side of matrix
      MPI_Recv(&(my_arrA[i][localDIM - rowInFullMatrix]), i, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    // TODO: shift col B[X] up X cols
  }

  MPI_Barrier(MPI_COMM_WORLD);
  
  // print shuffled matrices to verify correctness
  if(DIAGNOSTICS){
    if (my_rank == 0){
      printf("AFTER SHUFFLE\n");
    }
    print_dist_mat(my_arrA, "A", localDIM, nprocs, my_rank, MPI_COMM_WORLD);
    // print_dist_mat(my_arrB, "B", localDIM, nprocs, my_rank, MPI_COMM_WORLD);
  }


  // TODO NEXT

  // MPI_Barrier(MPI_COMM_WORLD);
  // print_dist_mat(my_arrC, "C", localDIM, nprocs, my_rank, MPI_COMM_WORLD);

// CLEANUP
  free(send_buff);
  // free(recv_buff);

  for (int i = 0; i < localDIM; i++)
  {
    free(my_arrA[i]);
    free(my_arrB[i]);
    free(my_arrC[i]);
  }
  free(my_arrA);
  free(my_arrB);
  free(my_arrC);

  MPI_Finalize();
  return 0;
}
// END MAIN

// perform naive matrix multiplication
void naive_multiply(int **my_arrA, int **my_arrB, int **my_arrC){
  for (int i = 0; i < DIM; i++)
    for (int k = 0; k < DIM; k++)
      for (int j = 0; j < DIM; j++)
        my_arrC[i][j] += my_arrA[i][k] * my_arrB[k][j];
}

void print_dist_mat(int **arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world){
  if(my_rank == 0) printf("\n### Matrix %s ###\n", name);
  int print_rank = 0;
  for (int i = 0; i < nprocs; i++){
    if (my_rank == print_rank){
      printf("Rank: %d\n", my_rank);
      print_chunk(arr, localDIM);
    }
    print_rank++;
    MPI_Barrier(world);
  }
}

void print_chunk(int **arr, int localDIM){
  for (int i = 0; i < localDIM; i++){
    for (int k = 0; k < localDIM; k++){
      printf("%-12d ", arr[i][k]);
    }
    printf("\n");
  }
  printf("\n");
}

int one_rank_left(int startCol, int startRow, int blockDIM){
    return ((startCol + blockDIM - 1) % blockDIM) + (startRow * blockDIM);
}

int one_rank_up(int startRank, int nprocs, int blockDIM){
    return (startRank + nprocs - blockDIM) % nprocs;
}