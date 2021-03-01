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
int importDataset(char * fname, int N, double ** dataset);


int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


  //Process command-line arguments
  int N;
  int DIM;
  int blocksize;
  char inputFname[500];


  if (argc != 5) {
    fprintf(stderr,"Please provide the following on the command line: N (number of lines in the file), dimensionality (number of coordinates per point), block size, dataset filename.\n");
    MPI_Finalize();
    exit(0);
  }

  sscanf(argv[1],"%d",&N);
  sscanf(argv[2],"%d",&DIM);
  sscanf(argv[3],"%d",&blocksize);
  strcpy(inputFname,argv[4]);
  
  //pointer to dataset
  double ** dataset;

  
  if (N<1 || DIM <1)
  {
    printf("\nN is invalid or DIM is invalid\n");
    MPI_Finalize();
    exit(0);
  }
  else
  //All ranks import dataset
  {
   
    if (my_rank==0)
    {
    printf("\nNumber of lines (N): %d, Dimensionality: %d, Block size: %d, Filename: %s\n", N, DIM, blocksize, inputFname);
    }

    //allocate memory for dataset
    dataset=(double**)malloc(sizeof(double*)*N);
    for (int i=0; i<N; i++)
    {
      dataset[i]=(double*)malloc(sizeof(double)*DIM);
    }

    // import data set...
    int ret=importDataset(inputFname, N, dataset);

    // ... failing fast if import fails.
    if (ret==1)
    {
      MPI_Finalize();
      return 0;
    }

  }

  //Write code here
  // at the start, every rank has a local copy of the data.

  if (my_rank == 0){
    // Each rank is responsible for computing all distances for N/nprocs rows (points)
    int n_rows_per_rank = N / nprocs;
    // If N isn't a multiple of nprocs, assign extra rows only to the last rank.
    int n_rows_last_rank = n_rows_per_rank + N % nprocs;

    printf("%d rows, %d ranks, %d rows per rank, except %d rows in the last rank\n",
           N, nprocs, n_rows_per_rank, n_rows_last_rank );
    // Use MPI_Scatter() to "assign" the work to each rank using MPI scatter
    // Send row information, not raw data
  }

  // allocate memory on all ranks for an n_rows_per_rank x N cols subset of the dm
  // This will stay distributed.
  if (my_rank != nprocs - 1){
    double *local_dm_block = (double *) malloc(sizeof(double) * n_rows_per_rank * N);
  } else {
    double *local_dm_block = (double *) malloc(sizeof(double) * n_rows_last_rank * N);
  }

  // Use MPI_Wtime() to begin timing on rank 0 only
  double time = MPI_Wtime();
  // Calculate distances

  // Calculate local sums at each rank

  // Use MPI_Wtime() to end timing on rank 0 only
  time = MPI_Wtime() - time;
  
  // output the distance matrix (sequentially)

  // Have rank 0 MPI_Reduce the local sums into a single global sum. 
  // Report this in the writeup.




  //free dataset
  for (int i=0; i<N; i++)
  {
    free(dataset[i]);
  }

  free(dataset);

  MPI_Finalize();

  return 0;
}




int importDataset(char * fname, int N, double ** dataset)
{

    FILE *fp = fopen(fname, "r");

    if (!fp) {
        printf("Unable to open file\n");
        return(1);
    }

    char buf[4096];
    int rowCnt = 0;
    int colCnt = 0;
    while (fgets(buf, 4096, fp) && rowCnt<N) {
        colCnt = 0;

        char *field = strtok(buf, ",");
        double tmp;
        sscanf(field,"%lf",&tmp);
        dataset[rowCnt][colCnt]=tmp;

        
        while (field) {
          colCnt++;
          field = strtok(NULL, ",");
          
          if (field!=NULL)
          {
          double tmp;
          sscanf(field,"%lf",&tmp);
          dataset[rowCnt][colCnt]=tmp;
          }   

        }
        rowCnt++;
    }

    fclose(fp);

    return 0;


}


