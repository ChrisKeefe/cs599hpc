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
  int recv_buff;
  int recv_counter = 0;
  int i;
  for (i = 0; i < 5; i++){
    if (my_rank % 2 == 0){
      MPI_Send(&my_rank, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&recv_buff, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      recv_counter = recv_counter + recv_buff;
    } else {
      MPI_Recv(&recv_buff, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      recv_counter = recv_counter + recv_buff;
      MPI_Send(&my_rank, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
    }
  }
  printf("Rank %d received the value %d\n", my_rank, recv_counter);

  MPI_Finalize();
  return 0;
}