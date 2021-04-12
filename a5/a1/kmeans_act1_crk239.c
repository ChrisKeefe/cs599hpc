//kmeans_starter.c

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>


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

  // Calculate per-rank data ranges
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
  int num_pts = my_last_pt - my_first_pt + 1;
  // cluster-center labels for each point "owned" by this rank:
  int clusterings[num_pts];

  // Generate initial centroids in a 1d array. Access individual points by
  // iterating once per DIM over a stride of DIM
  double *centroids=(double*)malloc(sizeof(double*) * KMEANS * DIM);
  for (int i=0; i<KMEANS; i++)
  {
    for (int j=0; j<DIM; j++){
      centroids[i*DIM + j] = dataset[i][j];
    }
  }

  // Check for "convergence" (using fixed number of iterations per spec)
  while (niters < KMEANSITERS){
    // report cluster centers - remove my_rank condition to confirm centroids identical
    if (my_rank == 0){
      printf("\nCentroids at iteration %d r%d\n", niters, my_rank);
      for (int ctr_dim = 0; ctr_dim < KMEANS * DIM; ctr_dim++){
        printf("%f\t", centroids[ctr_dim]);
        if (ctr_dim % DIM - 1 == 0){
          printf("\n");
        }
      }
    }

    // Assign points to nearest centroid
    int pt;
    for (pt = my_first_pt; pt <= my_last_pt; pt++){
      int nearest_ctr_idx = 0;
      double nearest_ctr_distance = DBL_MAX;

      // NOTE: This implementation does not require us to quantify total loss, so we
      // can we can calculate distance to each center using squared euclidean
      // distance for runtime efficiency, and we can keep only the resulting clustering
      for (int centroid = 0; centroid < KMEANS; centroid++ ){
        double diff = 0;
        double dist = 0;
        // calculate a distance for each point-centroid pair
        for (int dim = 0; dim < DIM; dim++) {
          double data_coord = dataset[pt][dim];
          double centroid_coord = centroids[centroid * DIM + dim];
          diff = data_coord - centroid_coord;
          dist += diff * diff;
        }

        if (dist < nearest_ctr_distance) {
          nearest_ctr_idx = centroid; 
          nearest_ctr_distance = dist;
        }
      }
      clusterings[pt - my_first_pt] = nearest_ctr_idx;
    }

    // Update cluster means (skipping this after final clustering assignment)
    if (niters != KMEANSITERS - 1){
      long int loc_per_ctr_cardinalities[KMEANS];
      long int gl_per_ctr_cardinalities[KMEANS];
      for (int i = 0; i < KMEANS; i++){
        loc_per_ctr_cardinalities[i] = 0;
        gl_per_ctr_cardinalities[i] = 0;
      }
      // Our local per-dim sums are zero-initialized, so any centroid dim with no
      // associated points will have a global mean of 0. Empty clusters
      // "re-initialize" at (0, ..., 0) by default
      double *loc_per_dim_sums = (double *)calloc(KMEANS * DIM, sizeof(double));
      double *weighted_means = (double *)calloc(KMEANS * DIM, sizeof(double));

      // Get local cardinalities for each clustering, and sums per dimension
      for (int pt = my_first_pt; pt <= my_last_pt; pt++){
        int loc_idx = pt - my_first_pt;
        // Increment the cardinality for this point's center
        int associated_ctr = clusterings[pt - my_first_pt];
        loc_per_ctr_cardinalities[associated_ctr]++;
        for (int dim = 0; dim < DIM; dim++){
          int ctr_start_idx = associated_ctr * DIM;
          loc_per_dim_sums[ctr_start_idx + dim] += dataset[pt][dim];
        }
      }

      // Get global cardinalities
      MPI_Allreduce(&loc_per_ctr_cardinalities, &gl_per_ctr_cardinalities, KMEANS, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);

      // calculate the local weighted mean for each dimension of each centroid
      for (int ctr = 0; ctr < KMEANS; ctr++){
        for (int dim = 0; dim < DIM; dim++){
          int ctr_start_idx = ctr * DIM;
          weighted_means[ctr_start_idx + dim] = loc_per_dim_sums[ctr_start_idx + dim] / gl_per_ctr_cardinalities[ctr];
        }
      }

      // calculate new centroids by summing weighted means
      MPI_Allreduce(weighted_means, centroids, KMEANS * DIM, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

      free(loc_per_dim_sums);
      free(weighted_means);
    }
    niters++;
  }

  free(centroids);

  //free dataset
  for (int i=0; i<N; i++)
  {
    free(dataset[i]);
  }
  free(dataset);


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

