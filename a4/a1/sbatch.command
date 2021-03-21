#!/bin/bash

export PREFIX='/scratch/crk239/cs599hpc'
export ASSIGN='3'
export ACTIVITY='3'
export INFILE=rename_me_range_act${ACTIVITY}_crk239.c
export OUTFILE='range'

module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

for ITER in 1 2 3 ; do
    DATASIZE=$(expr $NPROCS \* 31)
    MYMEM=$(expr $DATASIZE + 4)
    for NPROCS in 1 4 8 12 16 20; do
      sbatch --job-name=${OUTFILE}${NPROCS}_${ITER} --ntasks=${NPROCS} --mem=${MYMEM} --output=out/${OUTFILE}${NPROCS}_${ITER}.out jobscript.sh
    done
done
