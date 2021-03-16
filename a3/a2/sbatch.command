#!/bin/bash

export PREFIX='/scratch/crk239/cs599hpc'
export ASSIGN='3'
export ACTIVITY='2'
export INFILE='sorting_act${ACTIVITY}_crk239.c'
export OUTFILE='sort'

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

for ITER in 1 2 3 ; do
    for NPROCS in 1 2 4 8 12 16 20; do
      sbatch --job-name=sort${NPROCS}_${ITER} --ntasks=${NPROCS} --output=outs/sort${NPROCS}_${ITER}.out jobscript.sh
    done
done
