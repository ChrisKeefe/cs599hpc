//reduction_starter.c


#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



//Example compilation
//mpicc reduction_starter.c -lm -o reduction_starter

//Example execution
//mpirun -np 1 -hostfile myhostfile.txt ./reduction_starter 10000



//Do not change the seed
#define SEED 72
#define MAXVAL 100000000



int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


  //Process command-line arguments
  unsigned int N;


  if (argc != 2) {

    if (my_rank==0)
    fprintf(stderr,"Please provide the following on the command line: N (total size of array)\n");  
    
    MPI_Finalize();
    exit(0);
  }

  sscanf(argv[1],"%d",&N);

  if (N%nprocs!=0)
  {
    if (my_rank==0)
    fprintf(stderr,"Please enter a new value of N. N mod p !=0\n");      
    
    MPI_Finalize();
    exit(0);
  }

  
  
  //Global array (rank 0 allocates this)
  unsigned int * data;

  //Local array for the rank
  unsigned int * localData=(unsigned int *)malloc(sizeof(unsigned int)*(N/nprocs));
  
  if (my_rank==0)
  {
    srand(SEED);
    data=(unsigned int *)malloc(sizeof(unsigned int)*N);
    //generate data on rank 0
    for (int i=0; i<N; i++)
    {
      data[i]=rand()%MAXVAL;
    }
  }


  //Write code here
  // Pt. 1, implement with send/receive

  // Scatter
  if (my_rank == 0){
    for (int i = 0; i<nprocs; i++ ){
      MPI_Send(&data[(i*N/nprocs)], N/nprocs, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
    }
    // rank = only needs to worry about the data at 'data'
    localData = data;
  } else {
    MPI_Recv(localData, N/nprocs, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  
  // Find the minimum value for each rank
  unsigned int localMin = localData[0];
  for (int i = 1; i < N/nprocs; i++){
    if (localData[i] < localMin){
      localMin=localData[i];
    }
  }

  printf("\nRank: %d: Local Minimum: %u", my_rank, localMin);
  unsigned int globalMin = localMin;

  if (my_rank == 0){
    unsigned int tmpBuffer;
    for (int i = 1; i < nprocs; i++){
      MPI_Recv(&tmpBuffer, 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if (tmpBuffer < globalMin){
        globalMin = tmpBuffer;
      }
    }
    // rank = only needs to worry about the data at 'data'
  } else {
    MPI_Send(&localMin, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
  }

  if (my_rank==0)
  {
    printf("\nGlobal minimum: %u", globalMin);
    free(data);
  }
  
  free(localData);


  MPI_Finalize();

  return 0;
}
