#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



//Example compilation
//mpicc ring_comm_starter.c -lm -o ring_comm_starter

//Example execution
//mpirun -np 2 -hostfile myhostfile.txt ./ring_comm_starter



int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  if (nprocs==1)
  {
    printf("\n\nEnter at least 2 process ranks\n\n");
    MPI_Finalize();
    return 0;
  }

  //Write code here
  int i;
  int recv_buff;
  int recv_counter = 0;

  for (i = 0; i < 10; i++){
    if (my_rank == 0) {
      MPI_Send(&my_rank, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
      // rank 0 must receive from the last rank
      MPI_Recv(&recv_buff, 1, MPI_INT, nprocs - 1, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      recv_counter = recv_counter + recv_buff;
    } else if (my_rank == nprocs - 1){
      MPI_Recv(&recv_buff, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      recv_counter = recv_counter + recv_buff;
      // last rank must send to 0
      MPI_Send(&my_rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
      MPI_Recv(&recv_buff, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      recv_counter = recv_counter + recv_buff;
      MPI_Send(&my_rank, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
    }
  }
  // Print counter value after loop terminates
  printf("Rank %d received the value %d\n", my_rank, recv_counter);


  MPI_Finalize();
  return 0;
}

