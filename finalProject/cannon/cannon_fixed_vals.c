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
int copy_col(int **arr, int *send_buff, int col_idx, int n_vals);
int n_ranks_left(int my_rank, int blockDIM, int n);
int n_ranks_up(int my_rank, int nprocs, int blockDIM, int n_vals);
void naive_multiply(int **my_arrA, int **my_arrB, int **my_arrC);
void print_chunk(int **arr, int localDIM);
void print_dist_mat(int **arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world);
void shift_row_left(int * send_buff, int my_rank, int *arr_row, int localDIM, int nPositions);

int main(int argc, char **argv) {
  int my_rank, nprocs, N, localN, localDIM, blockDIM, rowOffset, colOffset;
  int myProcRow, myProcCol, localStartIdx, gridRowOffset, gridColOffset;
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
  // coordinates of my rank in our matrix of processors
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
  // int a[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  // int b[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
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
  MPI_Request req, req2;
  int *send_buff = (int *)malloc(sizeof(int) * localDIM);
  // int *recv_buff = (int *)malloc(sizeof(int) * localDIM);

  // Initial matrix shuffle
  for (i = 0; i < localDIM; i++){
    // Shift row A[x] (in the full matrix) left x rows, so we need to map
    // local row i to its position in the full dataset
    int rowInFullMatrix = myProcRow * localDIM + i;
    shift_row_left(send_buff, my_rank, my_arrA[i], localDIM, rowInFullMatrix);

    // TODO: shift col B[X] up X cols
    // map local col i to its position in the full dataset
    int colInFullMatrix = myProcCol * localDIM + i;
      // copy_col(my_arrB, send_buff, i, localDIM);
      // int sendIsSplit = colInFullMatrix % localDIM;
      // int shiftNRanks = colInFullMatrix / localDIM;
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

int n_ranks_left(int my_rank, int blockDIM, int n){
  int myProcRow, myProcCol;
  int ret_rank = my_rank;
  for (int i = 0; i < n; i++){
    // find the grid row and column of current rank
    myProcRow = ret_rank / blockDIM;
    myProcCol = ret_rank % blockDIM;
    ret_rank = ((myProcCol + blockDIM - 1) % blockDIM) + (myProcRow * blockDIM);
  }
  return ret_rank;
}

int n_ranks_up(int my_rank, int nprocs, int blockDIM, int n_vals){
  int ret_rank = my_rank;
  for (int i = 0; i < n_vals; i++ ){
     ret_rank = (ret_rank + nprocs - blockDIM) % nprocs;
  }
  return ret_rank;
}

int copy_col(int **arr, int *send_buff, int col_idx, int n_vals){
  for (int i = 0; i < n_vals; i++ ){
    send_buff[i] = arr[i][col_idx];
  }
}

void shift_row_left(int * send_buff, int my_rank, int *arr_row, int localDIM, int nPositions){
  MPI_Request req, req2;
  int blockDIM = DIM / localDIM;
  int myProcRow = my_rank / blockDIM;

  // copy local submatrix row into sendbuffer. parts of the sendbuffer will be
  // distributed as needed below
  memcpy(send_buff, (void *)arr_row, sizeof(int) * localDIM);
  int sendIsSplit = nPositions % localDIM;
  int shiftNRanks = nPositions / localDIM;

  if ( ! sendIsSplit ){
    // The sent data will all be sent to one rank, so find that rank and replace all local values
    int dest_rank = n_ranks_left(my_rank, blockDIM, shiftNRanks);
    MPI_Isend(send_buff, localDIM, MPI_INT, dest_rank, 0, MPI_COMM_WORLD, &req);
    MPI_Recv(&(arr_row[0]), localDIM, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Wait(&req, MPI_STATUS_IGNORE);

  } else {
    // The sent data will be split across two ranks, so find both ranks and the number of values for each
    int toFarther = nPositions % localDIM;
    int toNearer = localDIM - toFarther;
    int fartherRank = n_ranks_left(my_rank, blockDIM, shiftNRanks+1);
    int nearerRank = n_ranks_left(my_rank, blockDIM, shiftNRanks);

    MPI_Isend(send_buff, toFarther, MPI_INT, fartherRank, 0, MPI_COMM_WORLD, &req);
    MPI_Isend(send_buff + toFarther, toNearer, MPI_INT, nearerRank, 1, MPI_COMM_WORLD, &req2);

    // TODO: RECEIVE NON-BLOCKING?
    MPI_Recv(&(arr_row[toNearer]), toFarther, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(arr_row[0]), toNearer, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Wait(&req, MPI_STATUS_IGNORE);
    MPI_Wait(&req2, MPI_STATUS_IGNORE);
    }
}