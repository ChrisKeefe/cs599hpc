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

TODO: 
- consolidate sending of blocks instead of rows
- Compare to non-hadamard implementations