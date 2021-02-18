#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

//Example compilation
//mpicc random_comm_starter.c -lm -o random_comm_starter

//Example execution
//mpirun -np 1 -hostfile myhostfile.txt ./random_comm_starter

//Do not change the seed, or your answer will not be correct
#define SEED 72

//Change this if you want, but make sure it is set to 10 when submitting the assignment
#define TOTALITER 10

int generateRandomRank(int max_rank, int my_rank);

int main(int argc, char **argv) {
  int i, my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  if (nprocs<3)
  {
    if (my_rank==0)
    printf("\nRun with at least 3 ranks.\n\n");
    MPI_Finalize();
    return 0;
  }

  //seed rng do not modify
  srand(SEED+my_rank);


  //WRITE CODE HERE
  int root_rank = 0;
  int source_rank = 0;
  int dest_rank = 0;
  int old_counter = 0;
  int new_counter = 0;

  if (my_rank == root_rank){
    dest_rank = generateRandomRank(nprocs - 1, my_rank);
    printf("Master: first rank: %d\n", dest_rank);
  }

  for (i = 0; i < TOTALITER; i++){
    MPI_Bcast(&source_rank, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
    MPI_Bcast(&dest_rank, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
    // printf("my_rank = %d, dest_rank = %d\n", my_rank, dest_rank);

    if(my_rank == dest_rank){
      MPI_Recv(&old_counter, 1, MPI_INT, source_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("My rank: %d, old counter: %d\n", my_rank, old_counter);

      new_counter = old_counter + my_rank;
      printf("My rank: %d, new counter: %d\n", my_rank, new_counter);

      // - calculate next to receive rank
      dest_rank = generateRandomRank(nprocs - 1, my_rank);
      printf("My rank: %d, next to recv: %d\n", my_rank, dest_rank);

      // send new dest rank to root so it can prepare for broadcast
      MPI_Send(&dest_rank, 1, MPI_INT, root_rank, 0, MPI_COMM_WORLD);


    } else if (my_rank == 0){
      // Handle the initial case where root is also the source rank
      if (my_rank == source_rank){
        MPI_Send(&new_counter, 1, MPI_INT, dest_rank, 0, MPI_COMM_WORLD);
      }
      // currently-active rank becomes source rank for next round
      source_rank = dest_rank;
      // receive new dest_rank from currently active rank
      MPI_Recv(&dest_rank, 1, MPI_INT, dest_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // source and dest are updated and ready to broadcast


    } else if (my_rank == source_rank) {
      // NOTE: this doesn't get called on the first send when source is root
      MPI_Send(&new_counter, 1, MPI_INT, dest_rank, 0, MPI_COMM_WORLD);


    } else {
      // ranks that aren't source or dest idle 
    }
  }

  

  MPI_Finalize();
  return 0;
}


//Do not modify the rank generator or you will get the wrong answer
//returns a rank between 1 and max_rank, but does not return itself
//does not generate rank 0
int generateRandomRank(int max_rank, int my_rank)
{
  
  int tmp=round(max_rank*((double)(rand()) / RAND_MAX));
  while(tmp==my_rank || tmp==0)
  {  
  tmp=round(max_rank*((double)(rand()) / RAND_MAX)); 
  }

  return tmp;
}
