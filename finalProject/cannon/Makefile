CC = mpicc
# CFLAGS = ""
LFLAGS = -lm
FIXED = cannon_fixed_vals
RAND = cannon_random
NP = 9
DIM = 3
NP = 4
DIM = 4
BLOCKSIZE = 1
DIAGNOSTICS = 1
HOST = ../myhostfile.txt

# mpicc kmeans.c -lm -o kmeans
# mpirun -np 4 -hostfile ../myhostfile.txt ./kmeans 5159737 2 2 ../iono_57min_5.16Mpts_2D.txt

.phony = clean run test time fixed rand

fixed:
	${CC} ${CFLAGS} ${LFLAGS} ${FIXED}.c -o ${FIXED}

rand:
	${CC} ${CFLAGS} ${LFLAGS} ${RAND}.c -o ${RAND}

run: rand
	mpirun -np ${NP} -hostfile ${HOST} ./${RAND} ${DIM} ${BLOCKSIZE}

test: fixed
	mpirun -np ${NP} -hostfile ${HOST} ./${FIXED} ${DIM} ${BLOCKSIZE} ${DIAGNOSTICS}

time: naive_sequential_fixed_vals
	time ./${FIXED}

clean:
	rm ${FIXED} ${RAND}
