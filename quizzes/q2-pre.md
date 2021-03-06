Chris Keefe
Quiz 2 - pre

Q1: What is the opposite (or inverse) of MPI_Gather() ? 
A: 2 - MPI_Scatter()

Q2: Consider an array stored at the root (rank0) that contains integer values. There are p process ranks. The array is large and you want to find the smallest value in the array across the p ranks. What is the best (most efficient) way to find that value using MPI?
A: 2. An MPI_Scatter() followed by an MPI_Reduce()

Q3: Your friend runs an MPI program and generates the plot shown in fig 1. The program was executed on 2 nodes, each with 20 cores (40 physical cores in total). Do you expect the program to achieve good scalability on 3 nodes, each with 20 cores?
A: Yes. Though some speedup is lost, the program has great parallel efficiency.

Q4: I'm not sure why I was typing all of these questions out. XD
A: Yes.

Q5: 
A: No. Here, we have to go back and forth between two different arrays in memory to get one value.
