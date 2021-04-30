CC = mpicc
# CFLAGS = ""
LFLAGS = -lm
FIXED = topology_fixed
RAND = cannon_random
NP = 4
DIM = 16
DIAGNOSTICS = 0
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
