#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=ping_pong
#SBATCH --time=02:00				# 2 min
#SBATCH --mem=2000
#SBATCH --nodes=1
#SBATCH --ntasks=10
#SBATCH --cpus-per-task=1

PREFIX='/scratch/crk239/cs599hpc'
ASSIGN='1'
ACTIVITY='1'
INFILE='pingpong_act1_crk239.c'
OUTFILE='pingpong'


module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}
