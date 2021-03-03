#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//Example compilation
//mpicc distance_matrix_starter.c -lm -o distance_matrix_starter

//Example execution
//mpirun -np 1 -hostfile myhostfile.txt ./distance_matrix_starter 100000 90 100 MSD_year_prediction_normalize_0_1_100k.txt
//mpirun -np 1 -hostfile ../myhostfile.txt ./dm 100 90 100 ../MSD_year_prediction_normalize_0_1_100k.txt

//function prototypes
int importDataset(char *fname, int N, double **dataset);
double euclidean_dist(double *pt_data_a, double *pt_data_b, unsigned int dim);

int main(int argc, char **argv)
{

  int my_rank, nprocs;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  //Process command-line arguments
  int N;
  int DIM;
  int blocksize;
  char inputFname[500];

  if (argc != 5)
  {
    fprintf(stderr, "Please provide the following on the command line: N (number of lines in the file), dimensionality (number of coordinates per point), block size, dataset filename.\n");
    MPI_Finalize();
    exit(0);
  }

  sscanf(argv[1], "%d", &N);
  sscanf(argv[2], "%d", &DIM);
  sscanf(argv[3], "%d", &blocksize);
  strcpy(inputFname, argv[4]);

  //pointer to dataset
  double **dataset;

  if (N < 1 || DIM < 1)
  {
    printf("\nN is invalid or DIM is invalid\n");
    MPI_Finalize();
    exit(0);
  }
  else
  //All ranks import dataset
  {

    if (my_rank == 0)
    {
      printf("\nNumber of lines (N): %d, Dimensionality: %d, Block size: %d, Filename: %s\n", N, DIM, blocksize, inputFname);
    }

    //allocate memory for dataset
    dataset = (double **)malloc(sizeof(double *) * N);
    for (int i = 0; i < N; i++)
    {
      dataset[i] = (double *)malloc(sizeof(double) * DIM);
    }

    // import data set...
    int ret = importDataset(inputFname, N, dataset);

    // ... failing fast if import fails.
    if (ret == 1)
    {
      MPI_Finalize();
      return 0;
    }
  }

  //Write code here
  // at the start, every rank has a local copy of the data.

  // Each rank is responsible for computing all distances for N/nprocs rows (points)
  // If N isn't a multiple of nprocs, assign extra rows only to the last rank.
  unsigned int n_rows_per_rank = N / nprocs;
  unsigned int n_rows_last_rank = n_rows_per_rank + N % nprocs;
  unsigned int loc_start_idx = 0;
  unsigned int start_indices[nprocs];
  unsigned int dm_idx = 0;

  // printf("R: %d, N: %d, n_rows_per_rank: %d\n", my_rank, N, n_rows_per_rank);

  // Populate array of start indices for Scatter
  if (my_rank == 0)
  {
    // printf("%d rows, %d ranks, %d rows per rank, except %d rows in the last rank\n",
    //        N, nprocs, n_rows_per_rank, n_rows_last_rank );
    for (int i = 0; i < nprocs; i++)
    {
      start_indices[i] = i * n_rows_per_rank;
    }
  }

  // Send start indices to all ranks
  MPI_Scatter(&start_indices, 1, MPI_UNSIGNED, &loc_start_idx, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  // printf("Rank: %d, Start index: %d\n", my_rank, loc_start_idx);

  // allocate memory on all ranks for an n_rows_per_rank x N cols subset of the dm
  if (my_rank == nprocs - 1)
  {
    n_rows_per_rank = n_rows_last_rank;
  }
  double *local_dm_block = (double *)calloc(n_rows_per_rank * N, sizeof(double));

  // printf("R: %d, n_rows_per_rank: %d\n", my_rank, n_rows_per_rank);
  // printf("R: %d, Number of slots in local_dm_block: %d\n", my_rank, n_rows_per_rank * N);

  double time;
  if (my_rank == 0)
  {
    time = MPI_Wtime();
  }

  int loc_stop_idx = loc_start_idx + n_rows_per_rank;
  for (int pt_a = loc_start_idx; pt_a < loc_stop_idx; pt_a++)
  {
    // To drop out lower triangle, change pt_b = 0 to pt_b = pt_a and adjust sums
    for (int pt_b = 0; pt_b < N; pt_b++)
    {
      dm_idx = (pt_a - loc_start_idx) * N + pt_b;
      local_dm_block[dm_idx] = euclidean_dist(dataset[pt_a], dataset[pt_b], DIM);
    }
  }
  // printf("R: %d, value at 0: %lf, value at 1: %lf\n", my_rank, local_dm_block[0], local_dm_block[1]);

  // TODO: Calculate local sums at each rank

  if (my_rank == 0)
  {
    time = MPI_Wtime() - time;
  }
  // TODO: display time

  // TODO: Use the following to sequentially display the DM
  // Display dm values for local block
  for (int i = 0; i < n_rows_per_rank * N; i++){
    printf("%lf", local_dm_block[i]);
    if ((i + 1) % N == 0){
      printf("\n");
    } else {
      printf(", ");
    }
  } // Have rank 0 MPI_Reduce the local sums into a single global sum.
  // Report this in the writeup.

  free(local_dm_block);
  //free dataset
  for (int i = 0; i < N; i++)
  {
    free(dataset[i]);
  }

  free(dataset);

  MPI_Finalize();

  return 0;
}

int importDataset(char *fname, int N, double **dataset)
{

  FILE *fp = fopen(fname, "r");

  if (!fp)
  {
    printf("Unable to open file\n");
    return (1);
  }

  char buf[4096];
  int rowCnt = 0;
  int colCnt = 0;
  while (fgets(buf, 4096, fp) && rowCnt < N)
  {
    colCnt = 0;

    char *field = strtok(buf, ",");
    double tmp;
    sscanf(field, "%lf", &tmp);
    dataset[rowCnt][colCnt] = tmp;

    while (field)
    {
      colCnt++;
      field = strtok(NULL, ",");

      if (field != NULL)
      {
        double tmp;
        sscanf(field, "%lf", &tmp);
        dataset[rowCnt][colCnt] = tmp;
      }
    }
    rowCnt++;
  }

  fclose(fp);

  return 0;
}

double euclidean_dist(double *pt_data_a, double *pt_data_b, unsigned int dim){
  double dist = 0;
  for (int i = 0; i < dim; i++){
    dist = dist + sqrt( (pt_data_a[i] - pt_data_b[i]) * (pt_data_a[i] - pt_data_b[i]) );
  }
  return dist;
}
