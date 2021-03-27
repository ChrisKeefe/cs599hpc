#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=range3e          # In practice, this is superseded from the CLI
#SBATCH --time=1:00 	            # 1 min
#SBATCH --mem=0                     # We are exclusive: 0 -> use all available mem
#SBATCH --cpus-per-task=1
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores


module load openmpi

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE} 2000000 100000
