#!/bin/bash

for ITER in 1 2 3 ; do
    for NPROCS in 1 4 8 12 16 20; do
      MYMEM=$(expr $NPROCS + 80)
      sbatch --job-name=dm${NPROCS}_${ITER} --mem=${MYMEM} --ntasks=${NPROCS} --output=dm${NPROCS}_${ITER}.out jobscript.sh
    done
done