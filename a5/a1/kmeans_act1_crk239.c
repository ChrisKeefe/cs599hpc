//kmeans_starter.c

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



#define KMEANSITERS 10

//compile
//mpicc kmeans.c -lm -o kmeans

//run example with 2 means
//mpirun -np 4 -hostfile ../myhostfile.txt ./kmeans 5159737 2 2 ../iono_57min_5.16Mpts_2D.txt

//function prototypes
int importDataset(char * fname, int DIM, int N, double ** dataset);

int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


  //Process command-line arguments
  int N;
  int DIM;
  int KMEANS;
  char inputFname[500];


  if (argc != 5) {
    fprintf(stderr,"Please provide the following on the command line: N (number of lines in the file), dimensionality (number of coordinates per point/feature vector), K (number of means), dataset filename. Your input: %s\n",argv[0]);
    MPI_Finalize();
    exit(0);
  }

  sscanf(argv[1],"%d",&N);
  sscanf(argv[2],"%d",&DIM);
  sscanf(argv[3],"%d",&KMEANS);
  strcpy(inputFname,argv[4]);
  
  //pointer to entire dataset
  double ** dataset;

  

  if (N<1 || DIM <1 || KMEANS < 1)
  {
    printf("\nOne of the following are invalid: N, DIM, K(MEANS)\n");
    MPI_Finalize();
    exit(0);
  }
  //All ranks import dataset
  else
  {
   
    if (my_rank==0)
    {
    printf("\nNumber of lines (N): %d, Dimensionality: %d, KMEANS: %d, Filename: %s\n", N, DIM, KMEANS, inputFname);
    }



    //allocate memory for dataset
    dataset=(double**)malloc(sizeof(double*)*N);
    for (int i=0; i<N; i++)
    {
      dataset[i]=(double*)malloc(sizeof(double)*DIM);
    }

    int ret=importDataset(inputFname, DIM, N, dataset);

    if (ret==1)
    {
      MPI_Finalize();
      return 0;
    }    
  }

  MPI_Barrier(MPI_COMM_WORLD);

  
  //Write code here
  int niters = 0;
  int first_points[nprocs];
  int my_first_pt, my_last_pt;

  if (my_rank == 0){
    int numPoints = N / nprocs;

    for (int i = 0; i < nprocs; i++){
      first_points[i] = i * numPoints;
    }
  }

  MPI_Bcast(&first_points, nprocs, MPI_INT, 0, MPI_COMM_WORLD);
  my_first_pt = first_points[my_rank];
  my_last_pt = (my_rank + 1 == nprocs) ? N - 1 : first_points[my_rank + 1] - 1;

  // Generate initial centroids
  double **centroids=(double**)malloc(sizeof(double*)*KMEANS);
  for (int i=0; i<KMEANS; i++)
  {
    centroids[i]=(double*)malloc(sizeof(double)*DIM);
    for (int j=0; j<DIM; j++){
      centroids[i][j] = dataset[i][j];
    }
  }

// if (my_rank == nprocs - 1){
//   for (int i=0; i<KMEANS; i++){
//     printf("data pt %d: (%lf, %lf)\n", i, dataset[i][0], dataset[i][1]);
//     printf("cent pt %d: (%lf, %lf)\n", i, centroids[i][0], centroids[i][1]);
//   }
// }

  // TODO:
  // Check for "convergence"
  while (niters < KMEANSITERS){
    // printf("HELLO HEIDI!\n");
    // Assign points to nearest centroid
    // Update cluster means
    // TODO: if centroid has no nearest points, then re-initialize to (0, 0)
    niters++;
  }

  //free dataset
  for (int i=0; i<N; i++)
  {
    free(dataset[i]);
  }
  free(dataset);

  // free centroids
  for (int i=0; i<KMEANS; i++){
    free(centroids[i]);
  }
  free(centroids);

  MPI_Finalize();
  return 0;
}






int importDataset(char * fname, int DIM, int N, double ** dataset)
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

