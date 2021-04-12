#!/bin/bash

export PREFIX='/scratch/crk239/cs599hpc'
export ASSIGN='5'
export ACTIVITY='1'
export INFILE=kmeans_act${ACTIVITY}_crk239.c
export OUTFILE='kmeans'

module load openmpi

mpic++ ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

NNODES=1

for ITER in 1 2 3 ; do
    for NPROCS in 1 4 8 12 16 20; do
	for K in 2 25 50 100; do
	    # If this blows up, start troubleshooting by checking that K exports successfully to the jobscript
	    export K
            NTPN=$(( NPROCS/NNODES ))
            sbatch --job-name=${OUTFILE}${NPROCS}_${K}_${ITER} --ntasks=${NPROCS} --nodes=${NNODES} --ntasks-per-node=${NTPN} --output=out/${OUTFILE}${NPROCS}_${K}_${ITER}.out jobscript.sh
	done
    done
done
