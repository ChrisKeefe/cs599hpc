#!/bin/bash

export PREFIX='/scratch/crk239/cs599hpc'
export ASSIGN='4'
export ACTIVITY='2'
export INFILE=range_act${ACTIVITY}_crk239.cpp
export OUTFILE='range'

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

for ITER in 1 2 3 ; do
    for NPROCS in 1 4 8 12 16 20; do
      sbatch --job-name=${OUTFILE}${NPROCS}_${ITER} --ntasks=${NPROCS} --output=out/${OUTFILE}${NPROCS}_${ITER}.out jobscript.sh
    done
done
