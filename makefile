EXECS=mpi_phi_prob
MPICXX?=mpicxx
MPIRUN?=mpirun

all: ${EXECS}

mpi_phi_prob: baseline.cpp
	${MPICXX} -o run baseline.cpp
	${MPIRUN} -n 6 run

clean:
	rm ${EXECS}
