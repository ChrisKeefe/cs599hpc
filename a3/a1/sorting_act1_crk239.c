#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// mpicc sorting_act1_crk239.c -lm -O3 -o sort
// mpirun -np 5 -hostfile ../myhostfile.txt ./sort

void generateData(int * data, int SIZE);

int compfn (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

//Do not change the seed
#define SEED 72
// TODO: revert
// #define MAXVAL 1000000
#define MAXVAL 1000

//Total input size is N, divided by nprocs
//Doesn't matter if N doesn't evenly divide nprocs
// #define N 10000000000
// TODO: return to the original value
#define N 100

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
  int * myDataSet=(int*)malloc(sizeof(int)*N); //upper bound size is N elements for the rank


  //Write code here
  
  // TODO: Begin timer
  // Step 1: Distribute data to correct ranks

  MPI_Request req = MPI_SUCCESS;
  MPI_Status status;
  unsigned int nValsToSend = 0;
  unsigned int myDataSize = 0;
  long int global_sum = 0;

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

// Send buffers look great!
if(my_rank == 0){
  printf("rank: %d, send buffer sz: %d\n", my_rank, nValsToSend);
  for (int i = 0; i < nValsToSend; i++ ){
    printf("%d ", sendDataSetBuffer[i]);
  }
 printf("\n\n");
}

      int max_sent = localN - myDataSize;
      int actual_sent;
      // TODO: See if we can remove this source_rank calc in favor of MPI_ANY_SOURCE
      int source_rank = (my_rank + nprocs - i) % nprocs;
      MPI_Isend(sendDataSetBuffer, nValsToSend, MPI_INT, destBucketIdx, 0, MPI_COMM_WORLD, &req);
      MPI_Recv(recvDatasetBuffer, max_sent, MPI_INT, source_rank, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_INT, &actual_sent);
      MPI_Wait(&req, &status);
      memcpy((void*)(myDataSet + sizeof(int) * myDataSize), (void *)recvDatasetBuffer, sizeof(int) * actual_sent);
      myDataSize += actual_sent;
if(my_rank == 1){
  printf("rank: %d, received sz: %d\n", my_rank, actual_sent);
  for (int i = 0; i < actual_sent; i++ ){
    printf("%d ", recvDatasetBuffer[i]);
  }
 printf("\n\n");
}
    }
  }

// Check all values at rank0
// if (my_rank == 0){
//   for (int i = 0; i < myDataSize; i++){
//     printf("%d ", myDataSet[i]);
//   }
//   printf("\n");
// }

  // receive values from MPI_ANY_SOURCE?, add to myDataSet, and increment myDataSize
  // end loop

  // at this point, we should have looped nprocs times, sent nprocs - 1 times,
  // and received nprocs - 1 times. 
  // r0 shouldn't receive when n-1 is sending to self. r1 shouldn't receive when
  // r0 is sending to self

  // first round, r0 sends to r0, r1 to r1, r n-1 to n-1: (my_rank + 0) % nprocs
  // second round, r0 sends to r1, r1 sends to r2, rn-1 to 9: (my_rank + 1) % nprocs
  // if three ranks: 0 + 1 % 3 = 1, 1 + 1 % 3 = 2, 2 + 1 % 3 = 0

  // Print time to distribute

  // Step 2: Sort data at each rank with qsort
  
  // End timer and display total time

  // Check that the global sum of all elements across all ranks before sorting
  // is the same as the global sum of all elements after sorting, using a reduction

  // TODO: Remove
  // printf("When p=20, global sum should be 499937769104586\n");

  // printf("Global sum is: %d", global_sum);

  // test to make sure data is sorted at each rank

  //free
  free(data); 
  free(sendDataSetBuffer); 
  free(recvDatasetBuffer); 
  free(myDataSet);

  MPI_Finalize();
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
