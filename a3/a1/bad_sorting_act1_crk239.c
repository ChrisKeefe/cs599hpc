#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



void generateData(int * data, int SIZE);


int compfn (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}


//Do not change the seed
#define SEED 72
#define MAXVAL 1000000

//Total input size is N, divided by nprocs
//Doesn't matter if N doesn't evenly divide nprocs
// #define N 1000000000
// TODO: Replace the OG val
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

// TODO: remove
  // printf("R: %d, data[0]: %d, data[1]: %d\n", my_rank, data[0], data[1]);

  int * sendDataSetBuffer=(int*)malloc(sizeof(int)*localN); //most that can be sent is localN elements
  int * recvDatasetBuffer=(int*)malloc(sizeof(int)*localN); //most that can be received is localN elements
  int * myDataSet=(int*)malloc(sizeof(int)*N); //upper bound size is N elements for the rank


  //Write code here
  // TODO: Begin timer

  // Step 1: Each rank stores the elements in a bucket in a buffer for a given
  // rank and sends the data to that rank

  // this array of counters tracks the number of values assigned to each bucket
  int *bucketCounters = (int*)calloc(nprocs, sizeof(int));
  // We'll put all outgoing data into sendDataSetBuffer, but local bucket data in myDataSet
  // NOTE: because we assume a roughly even distribution of values, we can probably get
  // away with splitting the sendDataSetBuffer up into p-2 even blocks while nprocs << localN
  // This solution is fragile, however, and would be better replaced with either an array of
  // per-bucket arrays or a single dynamic data structure into which we could efficiently insert 
  // values by index (Rust's Vec<Int> would probably do)
  
  // Set bucket value indices
  int bucketSize = MAXVAL / nprocs;
  int myBucketNum = my_rank;
  int myBucketStart = my_rank * bucketSize;
  // set bucket end, handling cases where MAXVAL % nprocs != 0
  int myBucketEnd = (my_rank != nprocs -1) ? (myBucketStart + bucketSize - 1) : MAXVAL;

  // Perform the bucketing:
  int sendBuffCtr = 0;
  for (int i = 0; i < localN; i++){
    // get bucket num, explicitly setting too-high values to larger last bucket
    int destBucketNum = data[i] / bucketSize;
    destBucketNum = (destBucketNum < nprocs) ? destBucketNum:(nprocs-1);

    // separate data into "my bucket" and "other buckets"
    if (destBucketNum == myBucketNum){
      myDataSet[bucketCounters[myBucketNum]] = data[i];
      bucketCounters[myBucketNum]++; 
      printf("R: %d, Putting %d into buffer for rank %d\n", my_rank, data[i], my_rank);
    } else {
      bucketCounters[destBucketNum]++; 
      sendBuffCtr++;
      // my_bucket is not included in the sendDataSetBuffer, so...
      if (destBucketNum > myBucketNum){
        destBucketNum--;
      }
      // printf("destbucketnum: %d, localN: %d, nprocs: %d\n", destBucketNum, localN, nprocs);
      // printf("start idx: %d, plus: %d\n", destBucketNum * localN / (nprocs - 1), bucketCounters[myBucketNum]);
      printf("R: %d, Putting %d into buffer for rank %d\n\n", my_rank, data[i], destBucketNum);
      sendDataSetBuffer[destBucketNum * localN / (nprocs - 1) + bucketCounters[myBucketNum]] = data[i];
    }
  }

  // TODO: Remove this block
  // printf("R: %d, check\n", my_rank);
  MPI_Barrier(MPI_COMM_WORLD);

  // for (int rank = 0; rank < nprocs - 2; rank++){
  //   int startidx = rank * localN / (nprocs - 1);
  //   for (int i = startidx; i < startidx + bucketCounters[rank]; i++ ){
  //     printf("R: %d, data rank: %d, data: %d\n", my_rank, rank, sendDataSetBuffer[i]);
  //   }
  // }

  if (my_rank == 0){
    for (int i = 0; i< nprocs; i++){
      printf("bucketcounters[%d]: %d\n", i, bucketCounters[i]);
    }
    for (int i = 0; i < localN; i++){
      if( i < bucketCounters[0]){
        printf("R: %d, data rank: 0, data: %d\n", my_rank, myDataSet[i]);
      }
    }
    for (int i = 0; i < i/ (nprocs -1); i++){
      for (int j = 0; j < localN / nprocs; j++){
        if( j < bucketCounters[i / (nprocs - 1)]){
          printf("R: %d, data rank: %d, data: %d\n", my_rank, i / (nprocs - 1) + 1, sendDataSetBuffer[i]);
        }
      }
    }  
  }

  // Print time to distribute

  // Step 2: After all ranks have their data in the specified ranges, each rank
  // sorts the data in their buckets.  (Each rank will need to send to p-1 other ranks)

  // Print time to sort
  
  // Q: How do we send the data to the appropriate ranks as a fun of the data ranges
  // assigned to each rank

  //  Once the data is obtained by all ranks, sort using qsort

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

