#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

// This implementation of Cannon's algorithm uses a 2D torus topology
#define N_PROC_DIMS 2
// TODO: Does this need to go?
// #define N 16

// declare global config variables
int DIM;
int DIAGNOSTICS;
int BLOCKSIZE;

enum dim {COL, ROW};
enum hdir {LEFT, RIGHT};
enum vdir {UP, DOWN};

// Forward Declarations
void populate_sequential_matrices(int *my_arrA, int *my_arrB, unsigned long long int *my_arrC, int DIM, int nprocs, int my_rank);
void print_chunk(int *arr, int localDIM);
void print_chunk_ul(unsigned long long int *arr, int localDIM);
void print_dist_mat(int *arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world);
void print_dist_mat_ul(unsigned long long int *arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world);
// void vshift(int my_rank, int nprocs, int ** arr, int locColNum, int localDIM, int nPositions, enum vdir direction);
// void hshift(int my_rank, int *arr_row, int localDIM, int nPositions, enum hdir direction);

int main(int argc, char **argv) {
  int i, j, k, tmp;
  int my_rank, nprocs, my_cart_rank, src, dest, my_coords[N_PROC_DIMS];
  int N, localN, localDIM, blockDIM, myProcRow, myProcCol;
  double start_time, end_time;

  // Initialize MPI
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
  
  /* ########### Read in CL Args, checking algorithm assumptions ############ */
  float nprocs_per_dim = sqrt(nprocs);
  if (nprocs_per_dim != (int) nprocs_per_dim)
  {
    if(my_rank == 0){
      printf("\nERROR: Cannon's algorithm requires a square number of processors.\n");
    }
    MPI_Finalize();
    exit(0);
  }

  if (argc != 3) {
    if(my_rank == 0){
      fprintf(stderr,"\nERROR: Please provide the following args: DIM (matrix dimensionality), and a diagnostic print flag.");
    }
    MPI_Finalize();
    exit(0);
  }

  // DIM is the length of one dimension of the full-sized square matrices A, B, and C
  sscanf(argv[1],"%d",&DIM);
  sscanf(argv[2],"%d", &DIAGNOSTICS);

  if (DIM < 1)
  {
    if(my_rank == 0){
      printf("\nERROR: DIM is invalid\n");
    }
    MPI_Finalize();
    exit(0);
  }

  /* ####################   Create rank topology ############################ */
  MPI_Comm comm_cart;
  int proc_dims[N_PROC_DIMS] = {nprocs_per_dim, nprocs_per_dim};
  // We want both x and y dimensions to be periodic (i.e. to wrap), so [True, True]
  int is_periodic[N_PROC_DIMS] = {1, 1};
  // There's nothing special about the order of our ranks in WORLD, so re-ordering
  // won't hurt anything if it happens, and could help MPI better account for the
  // hardware's own topology:
  int ok_to_reorder = 1;
  MPI_Cart_create(MPI_COMM_WORLD, N_PROC_DIMS, proc_dims, is_periodic, ok_to_reorder, &comm_cart);

  // Capture my_rank for the topology-aware communicator
  MPI_Comm_rank(comm_cart, &my_cart_rank);
  // Get the 2D coordinates of this rank
  MPI_Cart_coords(comm_cart, my_cart_rank, N_PROC_DIMS, my_coords);
  // printf("r: %d, row: %d, col: %d\n", my_cart_rank, my_coords[0], my_coords[1]);

  /* ######### Determine the dimensions of the data for each rank: ###########*/
  // N: The total number of elements of the matrix
  N = DIM * DIM;
  // The number of elements of each rank's chunk of the matrix
  localN = N / nprocs;
  // The side length of each rank's submatrix
  localDIM = sqrt(localN);

  // TODO: Remove
  // the number of submatrices on one axis of the matrix
  // blockDIM = DIM / localDIM;
  // coordinates of my rank in our matrix of processors
  // myProcRow = my_rank / blockDIM;
  // myProcCol = my_rank % blockDIM;

  /* ######################   Allocate Arrays   ############################# */
  /*      Move to 1d arrays, to better align with per-subarray sending        */
  int *my_arrA = (int *)malloc(sizeof(int) * localN); 
  int *my_arrB = (int *)malloc(sizeof(int) * localN);
  unsigned long long int *my_arrC = (unsigned long long int *)malloc(sizeof(unsigned long long int *) * localN);

  /* #######################  Populate Arrays  ############################## */
  populate_sequential_matrices(my_arrA, my_arrB, my_arrC, DIM, nprocs, my_cart_rank);

  if(DIAGNOSTICS){
    print_dist_mat(my_arrA, "A", localDIM, nprocs, my_rank, comm_cart);
    MPI_Barrier(comm_cart);
    sleep(1);
    print_dist_mat(my_arrB, "B", localDIM, nprocs, my_rank, comm_cart);
    MPI_Barrier(comm_cart);
    sleep(1);
    print_dist_mat_ul(my_arrC, "C", localDIM, nprocs, my_rank, comm_cart);
    MPI_Barrier(comm_cart);
    sleep(1);
  }
  
  /* ######################  Pre-Shift Arrays  ############################## */
  // Each rank must get both source and destination ranks for MPI_Send_recv
  // MPI_Cart_shift can deduce these from our cartesian communicator and some params
  // The negative sign on my_coords indicates left/up shift rather than right/down
  MPI_Status status;

  MPI_Cart_shift(comm_cart, ROW, -my_coords[COL], &src, &dest);
  // printf("r: %d shift: %d src: %d dest: %d\n", my_cart_rank, -my_coords[COL], src, dest);
  MPI_Sendrecv_replace(my_arrA, localN, MPI_INT, dest, 0, src, 0, comm_cart, &status);

  MPI_Cart_shift(comm_cart, COL, -my_coords[ROW], &src, &dest);
  // printf("r: %d coords: %d src: %d dest: %d\n", my_cart_rank, -my_coords[ROW], src, dest);
  MPI_Sendrecv_replace(my_arrB, localN, MPI_INT, dest, 1, src, 1, comm_cart, &status);

  if(DIAGNOSTICS){
    print_dist_mat(my_arrA, "A", localDIM, nprocs, my_rank, comm_cart);
    MPI_Barrier(comm_cart);
    sleep(1);
    print_dist_mat(my_arrB, "B", localDIM, nprocs, my_rank, comm_cart);
    MPI_Barrier(comm_cart);
    sleep(1);
    print_dist_mat_ul(my_arrC, "C", localDIM, nprocs, my_rank, comm_cart);
    MPI_Barrier(comm_cart);
    sleep(1);
  }

  /* ###########################  Cleanup  ################################## */
  free(my_arrA);
  free(my_arrB);
  free(my_arrC);

  // Frees our locally-created topology-aware communicator
  MPI_Comm_free(&comm_cart);

  MPI_Finalize();
  return 0;
}

void populate_sequential_matrices(int *my_arrA, int *my_arrB, unsigned long long int *my_arrC, int DIM, int nprocs, int my_rank){
  // Zeroes C, and populates arrs A & B with sequence of integers from (0.. localDIM * localDIM - 1), as below.
  // eg. array [[0, 1, 2, 3],
  //            [4, 5, 6, 7],
  //            [8, ...
  //            [...,    15]]
  int N = DIM * DIM;
  int localN = N / nprocs;
  int localDIM = sqrt(localN);
  int blockDIM = DIM / localDIM;

  int myProcRow = my_rank / blockDIM;
  int myProcCol = my_rank % blockDIM;
  int gridRowOffset = DIM * localDIM;
  int gridColOffset = localDIM;

  int localStartIdx = myProcRow * gridRowOffset + myProcCol * gridColOffset;

  for (int row = 0; row < localDIM; row++) {
    for (int col = 0; col < localDIM; col++) {
      int tmp = localStartIdx + row * DIM + col;
      my_arrA[row * localDIM + col] = tmp;
      my_arrB[row * localDIM + col] = tmp;
      my_arrC[row * localDIM + col] = 0;
    }
  }
}

void print_chunk(int *arr, int localDIM){
  for (int row = 0; row < localDIM; row++){
    for (int col = 0; col < localDIM; col++){
      printf("%-12d ", arr[row * localDIM + col]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_chunk_ul(unsigned long long int *arr, int localDIM){
  for (int row = 0; row < localDIM; row++){
    for (int col = 0; col < localDIM; col++){
      printf("%-12llu ", arr[row * localDIM + col]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_dist_mat(int *arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world){
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

void print_dist_mat_ul(unsigned long long int *arr, const char *name, int localDIM, int nprocs, int my_rank, MPI_Comm world){
  if(my_rank == 0) printf("\n### Matrix %s ###\n", name);
  int print_rank = 0;
  for (int i = 0; i < nprocs; i++){
    if (my_rank == print_rank){
      printf("Rank: %llu\n", my_rank);
      print_chunk_ul(arr, localDIM);
    }
    print_rank++;
    MPI_Barrier(world);
  }
}
