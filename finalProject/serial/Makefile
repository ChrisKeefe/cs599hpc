CC = gcc
# CFLAGS = ""
LFLAGS = -lm
FIXED = naive_sequential_fixed_vals
RAND = naive_sequential_random

# mpicc kmeans.c -lm -o kmeans
# mpirun -np 4 -hostfile ../myhostfile.txt ./kmeans 5159737 2 2 ../iono_57min_5.16Mpts_2D.txt

.phony = clean run test time fixed rand

fixed:
	${CC} ${CFLAGS} ${LFLAGS} ${FIXED}.c -o naive_fixed

rand:
	${CC} ${CFLAGS} ${LFLAGS} ${RAND}.c -o naive_rand

run: rand
	./naive_rand

test: fixed
	./naive_fixed

time: rand
	time ./naive_rand

clean:
	rm naive_fixed naive_rand
