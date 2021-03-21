#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//Example compilation
//mpicc -O3 range_act1_crk239.c -lm -o range

//Example execution
//mpirun -np 1 -hostfile ../myhostfile.txt ./range 100 100

struct dataStruct
{
  double x;
  double y;
};

struct queryStruct
{
  double x_min;
  double y_min;
  double x_max;
  double y_max;
};

void generateData(struct dataStruct * data, unsigned int localN);
void generateQueries(struct queryStruct * data, unsigned int localQ, int my_rank);

//Do not change constants
#define SEED 72
#define MAXVAL 100.0
#define QUERYRNG 10.0

int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  //Process command-line arguments
  if (argc != 3) {
    fprintf(stderr,"Please provide the following on the command line: <Num data points> <Num query points> \n");
    MPI_Finalize();
    exit(0);
  }

  int N;
  int Q;
  sscanf(argv[1],"%d",&N);
  sscanf(argv[2],"%d",&Q);
  const unsigned int localN=N;
  const unsigned int localQ=Q/nprocs;

  //local storage for the number of results of each query -- init to 0
  unsigned int * numResults=(unsigned int *)calloc(localQ, sizeof(unsigned int));

  //All ranks generate the same input data
  struct dataStruct * data=(struct dataStruct *)malloc(sizeof(struct dataStruct)*localN);
  generateData(data, localN);

  //All ranks generate different queries
  struct queryStruct * queries=(struct queryStruct *)malloc(sizeof(struct queryStruct)*localQ);
  generateQueries(queries, localQ, my_rank);

  MPI_Barrier(MPI_COMM_WORLD);


  // Write code here





  // cleanup
  free(numResults);
  free(data);
  free(queries);

  // End my code
  MPI_Finalize();
  return 0;
}


//generates data [0,MAXVAL)
void generateData(struct dataStruct * data, unsigned int localN)
{
  //seed rng do not modify
  //Same input dataset for all ranks
  srand(SEED);
  for (int i=0; i<localN; i++)
  {
        data[i].x=((double)rand()/(double)(RAND_MAX))*MAXVAL;
        data[i].y=((double)rand()/(double)(RAND_MAX))*MAXVAL;
  }
}

//generates queries
//x_min y_min are in [0,MAXVAL]
//x_max y_max are x_min+d1 y_min+d2
//distance (d1)= [0, QUERYRNG)
//distance (d2)= [0, QUERYRNG)

void generateQueries(struct queryStruct * data, unsigned int localQ, int my_rank)
{
  //seed rng do not modify
  //Different queries for each rank
  srand(SEED+my_rank);
  for (int i=0; i<localQ; i++)
  {
        data[i].x_min=((double)rand()/(double)(RAND_MAX))*MAXVAL;
        data[i].y_min=((double)rand()/(double)(RAND_MAX))*MAXVAL;

        double d1=((double)rand()/(double)(RAND_MAX))*QUERYRNG;
        double d2=((double)rand()/(double)(RAND_MAX))*QUERYRNG;
        data[i].x_max=data[i].x_min+d1;
        data[i].y_max=data[i].y_min+d2;
  }
}
