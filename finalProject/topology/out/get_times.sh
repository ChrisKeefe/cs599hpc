#!/bin/bash
for NNODES in 1; do
  for NPROCS in 1 4 16 64; do
        echo topology_fixed_${NNODES}_${NPROCS} >> timings.txt
  	  for ITER in 0 1 2 3 4 5 6 7 8 9; do
  	    tail -n 2 ./topology_fixed${NNODES}_${NPROCS}_${ITER}.out | head -n 1 >> timings.txt
      done
  done
done

for NNODES in 2; do
  for NPROCS in 4 16 64; do
    echo topology_fixed_${NNODES}_${NPROCS} >> timings.txt
      for ITER in 0 1 2 3 4 5 6 7 8 9; do
  	tail -n 2 ./topology_fixed${NNODES}_${NPROCS}_${ITER}.out | head -n 1 >> timings.txt
    done
  done
done

for NNODES in 4; do
  for NPROCS in 4 16 64 256; do
    echo topology_fixed_${NNODES}_${NPROCS} >> timings.txt
      for ITER in 0 1 2 3 4 5 6 7 8 9; do
  	tail -n 2 ./topology_fixed${NNODES}_${NPROCS}_${ITER}.out | head -n 1 >> timings.txt
    done
  done
done

for NNODES in 8; do
  for NPROCS in 16 64 256; do
    echo topology_fixed_${NNODES}_${NPROCS} >> timings.txt
      for ITER in 0 1 2 3 4 5 6 7 8 9; do
  	tail -n 2 ./topology_fixed${NNODES}_${NPROCS}_${ITER}.out | head -n 1 >> timings.txt
    done
  done
done
