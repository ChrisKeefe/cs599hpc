#!/bin/bash

for NPROCS in 1 4 8 12 16 20; do
    echo dm${NPROCS} >> timings.txt
	for ITER in 1 2 3 ; do
	sed -n '3p' a1/dm${NPROCS}_${ITER}.out >> timings.txt
    done
done
