#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=ring3
#SBATCH --time=01:00				# 1 min
#SBATCH --mem=2000
#SBATCH --nodes=1
#SBATCH --ntasks=6
#SBATCH --cpus-per-task=1

PREFIX='/scratch/crk239/cs599hpc'
ASSIGN='1'
ACTIVITY='3'
INFILE=ring_act${ACTIVITY}_crk239.c
OUTFILE=ring${ACTIVITY}

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}
