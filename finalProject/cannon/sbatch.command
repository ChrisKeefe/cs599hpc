#!/bin/bash

export PREFIX='/scratch/crk239/cs599hpc'
export ASSIGN='finalProject'
export ACTIVITY='cannon'
export INFILE=${ACTIVITY}_random.c
export OUTFILE='cannon_random'

module load openmpi

mpic++ ${PREFIX}/${ASSIGN}/${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/${ASSIGN}/${ACTIVITY}/${OUTFILE}

NNODES=1

for ITER in 1 2 3 ; do
    for NPROCS in 1; do
            NTPN=$(( NPROCS/NNODES ))
            sbatch --job-name=${OUTFILE}${NPROCS}_${ITER} --ntasks=${NPROCS} --nodes=${NNODES} --ntasks-per-node=${NTPN} --output=out/${OUTFILE}${NPROCS}_${ITER}.out jobscript.sh
    done
done
