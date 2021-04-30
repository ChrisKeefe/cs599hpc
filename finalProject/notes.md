## Project notes for writeup

## 4/14
- modularized sequential code
- Updated int sizes and manually capped RAND_MAX to guarantee no integer overflows. Code now uses unsigned long long ints for C matrix and ints for A/B
- MPI and jobscripts for running
- Diagnostic flags
- Basic (single-values) cannon
- external validation to reference impl?

- Requires hadamard product, not naive MM
- corrected wikipedia entry
- Hadamard works at small scales, but does not scale up

Actually, we need to shift blocks at the same scale at which we multiply them.
In other words, scalar multiplication goes alongside row/col-wise shifts, as initially implemented,
but naive block multiplication must be paired with block-wise shifts. 

TODO: 
- consolidate sending of blocks instead of rows
- Compare to non-hadamard implementations

If we were going to continue working with matrices a and b after multiplication, we would need to return them to the original position (1 additional shift?)
