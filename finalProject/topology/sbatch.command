#!/bin/bash

export PREFIX='/scratch/crk239/cs599hpc'
export ASSIGN='finalProject'
export ACTIVITY='topology'
export INFILE=${ACTIVITY}_fixed.c
export OUTFILE=${ACTIVITY}_fixed

module load openmpi

mpic++ ${PREFIX}/${ASSIGN}/${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/${ASSIGN}/${ACTIVITY}/${OUTFILE}

NNODES=1

for ITER in 0 1 2 3 4 5 6 7 8 9; do
    for NPROCS in 1 4 16 64; do
            NTPN=$(( NPROCS/NNODES ))
            sbatch --job-name=${OUTFILE}${NNODES}_${NPROCS}_${ITER} --ntasks=${NPROCS} --nodes=${NNODES} --ntasks-per-node=${NTPN} --output=out/${OUTFILE}${NNODES}_${NPROCS}_${ITER}.out jobscript.sh
    done
done