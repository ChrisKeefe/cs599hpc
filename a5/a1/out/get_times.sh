#!/bin/bash

for NPROCS in 4 8 12 16 20; do
    for K in 2 25 50 100; do
      echo kmeans${NPROCS}_${K} >> timings.txt
	  for ITER in 1 2 3 ; do
	    tail -n 1 ./kmeans${NPROCS}_${K}_${ITER}.out >> timings.txt
      done
    done
done