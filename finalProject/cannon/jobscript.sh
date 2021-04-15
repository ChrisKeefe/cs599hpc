#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=cannon
#SBATCH --time=5:00 	            # 2 min
#SBATCH --mem=16G
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores


module load openmpi

N=4096
DIAGNOSTIC=0

srun ${PREFIX}/${ASSIGN}/${ACTIVITY}/${OUTFILE} ${N} ${DIAGNOSTIC}
