#!/bin/bash

for NPROCS in 4 8 12 16 20; do
    echo range${NPROCS} >> timings.txt
	for ITER in 1 2 3 ; do
	sed -n '2p' ./range${NPROCS}_${ITER}.out >> timings.txt
    # use awk NR and NF and exec to average the timings
    done
done

for NPROCS in 4 8 12 16 20; do
    echo range${NPROCS} >> global_sums.txt
	for ITER in 1 2 3 ; do
	sed -n '1p' ./range${NPROCS}_${ITER}.out >> global_sums.txt
    done
done
