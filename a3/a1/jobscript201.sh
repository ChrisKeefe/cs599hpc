#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=sort20_1            # In practice, this is superseded from the CLI
#SBATCH --output=sort20_1.out
#SBATCH --time=8:00 				# 15 min
#SBATCH --mem=16000                 # 160GB
#SBATCH --nodes=1
#SBATCH --ntasks=20                 # Also superseded from the CLI
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores

PREFIX='/scratch/crk239/cs599hpc'
ASSIGN='3'
ACTIVITY='1'
INFILE='sorting_act1_crk239.c'
OUTFILE='sort'

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}
