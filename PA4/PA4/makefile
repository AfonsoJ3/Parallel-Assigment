CC = mpicc
CPP = mpic++
LDC = mpicc
LD_FLAGS = -lm -fopenmp -lstdc++ -lopencv_core -lopencv_highgui -lopencv_imgproc
FLAGS = -fopenmp
CPPFLAGS= -I/usr/include/opencv $(FLAGS)
PROG = imageTest.cx
RM= /bin/rm
PBS_FILE = pbs_MPI

#all rule

OBJS=$(PROG:%.cx=%.o)
TOOLS=imageTools.o

all: $(PROG)

$(PROG): $(OBJS) $(TOOLS)
	$(LDC)  $^ $(LD_FLAGS) -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@
%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $^ -o $@

#pbs script
pbs:
	echo "#!/bin/bash" > $(PBS_FILE)
	echo "#PBS -l nodes=2:ppn=12" >> $(PBS_FILE)
	echo "#PBS -l walltime=00:03:00" >> $(PBS_FILE)
	echo "#PBS -q batch" >> $(PBS_FILE)
	echo "#PBS -N first_run" >> $(PBS_FILE)
	echo "#PBS -j oe" >> $(PBS_FILE)
	echo "cd \$$HOME" >> $(PBS_FILE)
	echo "mpiexec -np 2 --map-by ppr:1:node --hostfile \$$PBS_NODEFILE ./*.x" >> $(PBS_FILE)

#clean rule
clean:
	$(RM) -rf *.o *.x *.cx *.mod
