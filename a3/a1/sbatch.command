#!/bin/bash

for ITER in 1 2 3 ; do
    for NPROCS in 1 2 4 8 12 16 20; do
      sbatch --job-name=sort${NPROCS}_${ITER} --ntasks=${NPROCS} --output=outs/sort${NPROCS}_${ITER}.out jobscript.sh
    done
done
