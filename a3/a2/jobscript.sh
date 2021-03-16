#!/bin/bash
#SBATCH --account=cs599-spr21
#SBATCH --job-name=sort2            # In practice, this is superseded from the CLI
#SBATCH --time=10:00 				# 10 min
#SBATCH --mem=16000                 # 160GB
#SBATCH --nodes=1
#SBATCH --ntasks=10                 # Also superseded from the CLI
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C sl                       # there are a bunch of broadwell (bw) and skylake (sl) cores


module load openmpi

mpicc ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${INFILE} -lm -O3 -o ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}

srun ${PREFIX}/a${ASSIGN}/a${ACTIVITY}/${OUTFILE}
