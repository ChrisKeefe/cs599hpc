#!/bin/bash

for NPROCS in 1; do
      echo cannon${NPROCS} >> timings.txt
	  for ITER in 0 1 2 3 4 5 6 7 8 9; do
	    tail -n 1 ./cannon_random${NPROCS}_${ITER}.out >> timings.txt
    done
done
