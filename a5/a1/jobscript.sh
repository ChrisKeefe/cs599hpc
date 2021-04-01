#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=range3           # In practice, this is superseded from the CLI
#SBATCH --time=5:00 	            # 5 min
#SBATCH --mem=0                     # We are exclusive: 0 -> use all available mem
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores


module load openmpi

# TODO: bump N up
# N=5159737
N=1000
DIM=2

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE} ${N} ${DIM} ${K} iono_57min_5.16Mpts_2D.txt
