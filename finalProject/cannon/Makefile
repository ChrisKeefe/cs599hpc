CC = mpicc
# CFLAGS = ""
LFLAGS = -lm
FIXED = cannon_fixed_vals
RAND = cannon_random
NP = 4
DIM = 6
BLOCKSIZE = 1
DIAGNOSTICS = 2
# DIAGNOSTICS = 1
HOST = ../myhostfile.txt

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
