CC = gcc
# CFLAGS = ""
# LFLAGS = -lm
FIXED = naive_sequential_fixed_vals

# mpicc kmeans.c -lm -o kmeans
# mpirun -np 4 -hostfile ../myhostfile.txt ./kmeans 5159737 2 2 ../iono_57min_5.16Mpts_2D.txt

.phony = clean run test time

naive_sequential_fixed_vals:
	${CC} ${CFLAGS} ${LFLAGS} ${FIXED}.c -o naive_fixed

run: naive_sequential_fixed_vals
	./naive_fixed

test: naive_sequential_fixed_vals
	./naive_fixed

time: naive_sequential_fixed_vals
	time ./naive_fixed

clean:
	rm naive_fixed
