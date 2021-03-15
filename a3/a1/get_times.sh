#!/bin/bash

for NPROCS in 1 2 4 8 12 16 20; do
    echo sort${NPROCS} >> timings.txt
	for ITER in 1 2 3 ; do
	sed -n '1p' ./sort${NPROCS}_${ITER}.out >> timings.txt
    done
done

for NPROCS in 1 2 4 8 12 16 20; do
    echo sort${NPROCS} >> global_sums.txt
	for ITER in 1 2 3 ; do
	sed -n '3p' ./sort${NPROCS}_${ITER}.out >> global_sums.txt
    done
done
