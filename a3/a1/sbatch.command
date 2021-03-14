#!/bin/bash

for ITER in 1 2 3 ; do
    for NPROCS in 1 4 8 12 16 20; do
      # MYGIGS=$(expr $NPROCS + 80)
      # MYMEM=$(expr $MYGIGS \* 1000)
      # sbatch --job-name=dm${NPROCS}_${ITER} --mem=${MYMEM} --ntasks=${NPROCS} --output=dm${NPROCS}_${ITER}.out jobscript.sh
      sbatch --job-name=dm${NPROCS}_${ITER} --ntasks=${NPROCS} --output=sort${NPROCS}_${ITER}.out jobscript.sh
    done
done