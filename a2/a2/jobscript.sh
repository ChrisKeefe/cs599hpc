#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=sample
#SBATCH --time=02:00				# 2 min
#SBATCH --mem=2000
#SBATCH --nodes=1
#SBATCH --ntasks=10
#SBATCH --cpus-per-task=1

PREFIX='/scratch/crk239/cs599hpc'
ASSIGN='2'
ACTIVITY='1'
INFILE='pingpong_act1_crk239.c'
OUTFILE='pingpong'

# run args
N=100000
DIMS=90
BLOCKSIZE=''
FNAME='../MSD_year_prediction_normalize_0_1_100k.txt'

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE} ${N} ${DIMS} ${BLOCKSIZE} ${FNAME}
