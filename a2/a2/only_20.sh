#!/bin/bash

for ITER in 1 2 3 ; do
    for BLOCKSZ in 5 100 500 1000 2000 3000 4000 5000; do
      NPROCS=20
      MYGIGS=$(expr $NPROCS + 80)
      MYMEM=$(expr $MYGIGS \* 1000)
      sbatch --job-name=dm${BLOCKSZ}_${ITER} --mem=${MYMEM} --ntasks=20 --export=BLOCKSIZE=${BLOCKSZ} --output=dm${BLOCKSZ}_${ITER}.out jobscript.sh
    done
done
