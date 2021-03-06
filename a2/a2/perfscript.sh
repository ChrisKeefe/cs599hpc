#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=dist_mat3        # In practice, this is superseded from the CLI
#SBATCH --time=15:00				# 5 min
#SBATCH --mem=8000                  # 80GB dm + (1 GB raw data + odds and ends) * ntasks. Also superseded
#SBATCH --nodes=1
#SBATCH --ntasks=20                 # Also superseded from the CLI
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores

PREFIX='/scratch/crk239/cs599hpc'
ASSIGN='2'
ACTIVITY='3'
INFILE='dist_mat_act2_crk239.c'  # Supplied by shell script export
OUTFILE='dm'

# run args
N=100000
DIMS=90
BLOCKSIZE=5             # Supplied by shell script export
FNAME='../MSD_year_prediction_normalize_0_1_100k.txt'

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun /usr/bin/perf stat -B -e cache-references,cache-misses ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE} ${N} ${DIMS} ${BLOCKSIZE} ${FNAME}
