#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=dist_mat2        # In practice, this is superseded from the CLI
#SBATCH --time=05:00				# 5 min
#SBATCH --mem=8000                  # 80GB dm + (1 GB raw data + odds and ends) * ntasks. Also superseded
#SBATCH --nodes=1
#SBATCH --ntasks=10                 # Also superseded from the CLI
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores

PREFIX='/scratch/crk239/cs599hpc'
ASSIGN='2'
ACTIVITY='1'
INFILE='dist_mat_act1_crk239.c'
OUTFILE='dm'

# run args
N=100000
DIMS=90
BLOCKSIZE=100
FNAME='../MSD_year_prediction_normalize_0_1_100k.txt'

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE} ${N} ${DIMS} ${BLOCKSIZE} ${FNAME}
