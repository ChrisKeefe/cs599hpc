#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

//Example compilation
//mpicc distance_matrix_starter.c -lm -o distance_matrix_starter

//Example execution
//mpirun -np 1 -hostfile myhostfile.txt ./distance_matrix_starter 100000 90 100 MSD_year_prediction_normalize_0_1_100k.txt
//mpirun -np 1 -hostfile ../myhostfile.txt ./dm 100 90 100 ../MSD_year_prediction_normalize_0_1_100k.txt

//function prototypes
int importDataset(char *fname, int N, double **dataset);
double euclidean_dist(double *pt_data_a, double *pt_data_b, unsigned int dim);
void print_chunk(unsigned int local_dm_arr_len, unsigned int N, double *local_dm_chunk);
void calculate_tile_dists(int start_row, int start_col, int end_row, int end_col,
                          double **dataset, double *local_dm_chunk, int N, int DIM);

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
  if (my_rank == nprocs - 1)
  {
    n_rows_per_rank = n_rows_last_rank;
  }
  unsigned int loc_start_idx = 0;
  unsigned int start_indices[nprocs];
  unsigned int dm_idx = 0;
  unsigned int local_dm_arr_len = n_rows_per_rank * N;
  double local_sum = 0;
  double global_sum = 0;

  // Populate array of start indices for Scatter
  if (my_rank == 0)
  {
    for (int i = 0; i < nprocs; i++)
    {
      start_indices[i] = i * n_rows_per_rank;
    }
  }

  // Send start indices to all ranks
  MPI_Scatter(&start_indices, 1, MPI_UNSIGNED, &loc_start_idx, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

  // allocate memory on all ranks for an n_rows_per_rank x N cols subset of the dm
  double *local_dm_chunk = (double *)calloc(n_rows_per_rank * N, sizeof(double));

  // begin timer on root only
  double time;
  if (my_rank == 0)
  {
    time = MPI_Wtime();
  }

  // set start/end indices for each rank's chunk of the data
  const int rank_start_row = loc_start_idx;
  const int rank_end_row = loc_start_idx + n_rows_per_rank;
  const int rank_start_col = 0;
  const int rank_end_col = N;
  // Set start/end indices for the first tile we'll calculate
  int tile_start_row = rank_start_row;
  int tile_start_col = rank_start_col;
  int tile_end_row = tile_start_row + blocksize - 1;
  int tile_end_col = blocksize - 1;

  // How many tiles will we calculate?
  int tiles_per_row = (N % blocksize == 0) ? (int)(N / blocksize):(int)(N/blocksize) + 1;
  int tiles_per_col = (n_rows_per_rank % blocksize == 0) ? (int)(n_rows_per_rank / blocksize):(int)(n_rows_per_rank/blocksize) + 1;
  int tiles_remaining = tiles_per_row * tiles_per_col;
  bool to_next_row = true;

  // printf("Tiles to calculate: %d\n", tiles_remaining);

  while (tiles_remaining){
    // Ceiling end indices to prevent overflow, and flag if at end of row
    tile_end_row = (tile_end_row < n_rows_per_rank) ? tile_end_row : n_rows_per_rank;
    if (tile_end_col >= N){
      tile_end_col = N;
      tile_start_row += blocksize;
      to_next_row = true;
    }
    calculate_tile_dists(rank_start_row, rank_start_col, rank_end_row, rank_end_col, dataset, local_dm_chunk, N, DIM);
    // adjust loop flag and indices 
    tiles_remaining--;
    tile_start_col += blocksize;
    tile_end_col += blocksize;
    if (to_next_row){
      tile_start_row += blocksize;
      tile_end_row += blocksize;
      to_next_row = false;
    }
  }

  // Display run time of core calculation
  if (my_rank == 0)
  {
    time = MPI_Wtime() - time;
    printf("Run time for r0: %f\n", time);
  }

  // Calculate local and global sums
  for (int i = 0; i < local_dm_arr_len; i ++){
    local_sum += local_dm_chunk[i];
  }
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (my_rank == 0){
    // Adjust global sum to match square matrix calculation (instead of upper triangle)
    global_sum = 2 * global_sum;
    printf("Global sum of distances: %lf\n", global_sum);
  }

  // display dm by sequentially printing each chunk
  int print_rank = 0;
  for (int i = 0; i < nprocs; i++){
    if (my_rank == print_rank){
      print_chunk(local_dm_arr_len, N, local_dm_chunk);
      print_rank++;
    }
    MPI_Bcast(&print_rank, 1, MPI_INT, i, MPI_COMM_WORLD);
  }

  free(local_dm_chunk);
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
    dist = dist + (pt_data_a[i] - pt_data_b[i]) * (pt_data_a[i] - pt_data_b[i]);
  }
  dist = sqrt(dist);
  return dist;
}

void calculate_tile_dists(int start_row, int start_col, int end_row, int end_col,
                          double **dataset, double *local_dm_chunk, int N, int DIM){
  unsigned int dm_idx = 0;
  for (int pt_a = start_row; pt_a < end_row; pt_a++)
  {
    for (int pt_b = start_col; pt_b < end_col; pt_b++)
    {
      dm_idx = (pt_a - start_row) * N + pt_b;
      local_dm_chunk[dm_idx] = euclidean_dist(dataset[pt_a], dataset[pt_b], DIM);
    }
  }
}

void print_chunk(unsigned int local_dm_arr_len, unsigned int N, double *local_dm_chunk){
  for (int i = 0; i < local_dm_arr_len; i++){
    printf("%lf", local_dm_chunk[i]);
    if ((i + 1) % N == 0){
      printf("\n");
    } else {
      printf(", ");
    }
  }
}