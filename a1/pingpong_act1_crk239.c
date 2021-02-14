#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



//Example compilation
//mpicc pingpong_act1_crk239.c -lm -o pingpong

//Example execution
//mpirun -np 4 -hostfile ../myhostfile.txt ./pingpong


int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  //make sure even number of procs
  if ((nprocs%2)!=0)
  {
    if (my_rank==0)  
    {
    printf("\nYou must enter an even number of process ranks"); 
    }
    MPI_Finalize();
    return 0;
  }

  //Write code here
  



  MPI_Finalize();
  return 0;
}