#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// mpicc sorting_act1_crk239.c -lm -O3 -o sort
// mpirun -np 5 -hostfile ../myhostfile.txt ./sort

void generateData(int * data, int SIZE);
int notSorted(int *data, int size);

int compfn (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

//Do not change the seed
#define SEED 72
#define MAXVAL 1000000

//Total input size is N, divided by nprocs
//Doesn't matter if N doesn't evenly divide nprocs
#define N 1000000000

int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

  //seed rng do not modify
  srand(SEED+my_rank);


  //local input size N/nprocs
  const unsigned int localN=N/nprocs;

  //All ranks generate data
  int * data=(int*)malloc(sizeof(int)*localN);

  generateData(data, localN);

  int * sendDataSetBuffer=(int*)malloc(sizeof(int)*localN); //most that can be sent is localN elements
  int * recvDatasetBuffer=(int*)malloc(sizeof(int)*localN); //most that can be received is localN elements
  int * myDataSet=(int*)calloc(N, sizeof(int)); //upper bound size is N elements for the rank


  //Write code here
  double start_time = MPI_Wtime();
  
  // Step 1: Distribute data to correct ranks
  MPI_Request req;
  MPI_Status status;
  unsigned int nValsToSend = 0;
  unsigned int myDataSize = 0;
  double global_dist_time;
  double global_sort_time;
  double global_total_time;
  unsigned long long int local_sum = 0;
  unsigned long long int local_sum_unsorted = 0;
  unsigned long long int global_sum = 0;
  unsigned long long int global_sum_unsorted = 0;

  // Set bucket value indices
  int bucketSize = MAXVAL / nprocs;
  int myBucketStart = my_rank * bucketSize;
  // set bucket end, handling cases where MAXVAL % nprocs != 0
  int myBucketEnd = (my_rank != nprocs -1) ? (myBucketStart + bucketSize - 1) : MAXVAL;
  

  // read and distributes data once per rank
  for (int i = 0; i < nprocs; i++){
    nValsToSend = 0;
    int destBucketIdx = (my_rank + i) % nprocs;
    int destBucketStart = destBucketIdx * bucketSize;
    int destBucketEnd = 
      (destBucketIdx != nprocs -1) ? (destBucketStart + bucketSize - 1) : MAXVAL;

    // sort correct values into send buffer
    for (int val = 0; val < localN; val++){
      if (data[val] >= destBucketStart && data[val] <= destBucketEnd){
        // if values are local, put them directly in myDataSet, else sendBuff
        if(destBucketIdx == my_rank){
          myDataSet[myDataSize] = data[val];
          myDataSize++;
        } else {
          sendDataSetBuffer[nValsToSend] = data[val];
          nValsToSend++;
        }
      }
    }

    // Actually send/receive the data
    if (destBucketIdx != my_rank){
      int max_sendable = localN;
      int actual_sent;
      int source_rank = (my_rank + nprocs - i) % nprocs;
      MPI_Isend(sendDataSetBuffer, nValsToSend, MPI_INT, destBucketIdx, 0, MPI_COMM_WORLD, &req);
      MPI_Recv(recvDatasetBuffer, max_sendable, MPI_INT, source_rank, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_INT, &actual_sent);
      MPI_Wait(&req, &status);
      memcpy((void*)(myDataSet + myDataSize), (void *)recvDatasetBuffer, sizeof(int) * actual_sent);
      myDataSize = myDataSize + actual_sent;
    }
  }

  // Print time to distribute
  double distrib_time = MPI_Wtime() - start_time;


  // Step 2: Sort data at each rank with qsort
  qsort((void*)myDataSet, myDataSize, sizeof(int), &compfn);

  // End timer and display total time
  double sort_time = MPI_Wtime() - distrib_time;
  double total_time = sort_time + distrib_time;
  MPI_Reduce(&distrib_time, &global_dist_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&sort_time, &global_sort_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&total_time, &global_total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (my_rank == 0){
    printf("global distrib time: %f, global sort time: %f, global total time: %f\n", global_dist_time, global_sort_time, global_total_time);
  }


  // Test to make sure data is sorted at each rank
  int notSortd = notSorted(myDataSet, myDataSize);
  if (notSortd){
    printf("R%d is not sorted.\n", my_rank);
  }
  int nRanksUnsorted = 0;
  MPI_Reduce(&notSortd, &nRanksUnsorted, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if(my_rank==0){
    printf("Number of ranks not sorted properly: %d\n", nRanksUnsorted);
  }


  // Test that the global sum of all elements across all ranks before sorting
  // is the same as the global sum of all elements after sorting, using a reduction
  for (int i = 0; i < localN; i++){
    local_sum_unsorted = local_sum_unsorted + data[i];
  }
  for (int i = 0; i < myDataSize; i++){
    local_sum = local_sum + myDataSet[i];
  }
  MPI_Reduce(&local_sum_unsorted, &global_sum_unsorted, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if(my_rank==0){
    if (global_sum == global_sum_unsorted){
      printf("Global sum %llu == unsorted global sum %llu\n", global_sum, global_sum_unsorted);
    } else {
      printf("Global sum %llu != unsorted global sum %llu\n", global_sum, global_sum_unsorted);
    }
  }

  //free
  free(data);
  free(sendDataSetBuffer);
  free(recvDatasetBuffer);
  free(myDataSet);

  MPI_Finalize();
  return 0;
}

// returns 1 if an array is not sorted, 0 if sorted properly
int notSorted(int *data, int size){
  int prev = 0;
  for (int i = 0; i < size; i++){
    if (data[i] < prev){
      return 1;
    }
  }
  return 0;
}

//generates data [0,MAXVAL)
void generateData(int * data, int SIZE)
{
  for (int i=0; i<SIZE; i++)
  {
  
  data[i]=rand()%MAXVAL;
  }
}
