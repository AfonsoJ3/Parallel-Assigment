FCC = mpicc
LDA = mpicc

PROG = PA2.x 
OBJ = PA2.o

RM = rm -rf

#all rules:
all: $(PROG)

$(PROG): $(OBJ)
	$(LDA) $(OBJ) -o $(PROG)

%.o: %.c
	$(FCC) -c $<

#clean
clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
