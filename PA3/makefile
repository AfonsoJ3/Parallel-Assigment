FCC = mpicc 
LDA = mpicc

PROG = main.x
OBJ = main.o

RM = rm -rf

PBS_FILE = pbs_MPI

#all rules:
all: $(PROG)

$(PROG): $(OBJ)
	$(LDA) $(OBJ) -o $(PROG)

%.o: %.c
	$(FCC) -c $< 

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


#clean
clean: 
	$(RM) *.o *.x first_run.*		
