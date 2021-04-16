CC = mpicc
# CFLAGS = ""
LFLAGS = -lm
FIXED = cannon_fixed_vals
RAND = cannon_random
NP = 4
HOST = ../myhostfile.txt

# mpicc kmeans.c -lm -o kmeans
# mpirun -np 4 -hostfile ../myhostfile.txt ./kmeans 5159737 2 2 ../iono_57min_5.16Mpts_2D.txt

.phony = clean run test time fixed rand

fixed:
	${CC} ${CFLAGS} ${LFLAGS} ${FIXED}.c -o ${FIXED}

rand:
	${CC} ${CFLAGS} ${LFLAGS} ${RAND}.c -o ${RAND}

run: rand
	mpirun -np ${NP} -hostfile ${HOST} ./${RAND} 4 1

test: fixed
	mpirun -np ${NP} -hostfile ${HOST} ./${FIXED} 4 1 1

time: naive_sequential_fixed_vals
	time ./${FIXED}

clean:
	rm ${FIXED} ${RAND}
