#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=kmeans           # In practice, this is superseded from the CLI
#SBATCH --time=2:00 	            # 2 min
#SBATCH --mem=0                     # We are exclusive: 0 -> use all available mem
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores


module load openmpi

N=5159737
DIM=2

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE} ${N} ${DIM} ${K} ${PREFIX}/$a{ASSIGN}/iono_57min_5.16Mpts_2D.txt
