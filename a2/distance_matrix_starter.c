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
  //All ranks import dataset
  else
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

    int ret=importDataset(inputFname, N, dataset);

    if (ret==1)
    {
      MPI_Finalize();
      return 0;
    }

  }

  //Write code here



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


