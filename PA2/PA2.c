#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv)
{

    	int rank;
    	int numranks;
    	//initialize MPI
    	MPI_Init(&argc,&argv);
    	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    	MPI_Comm_size(MPI_COMM_WORLD,&numranks);
	
	MPI_Status stat;

	int number;
    	//Ring -- Your version here
	if (rank == 0)
	{
		printf("The process 0 send %d to process 1.\n",number);
		MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&number, 1, MPI_INT, numranks -1, 0, MPI_COMM_WORLD, &stat);
		printf("The process 0 recived number %d from process %d.\n", number, numranks - 1);
	}

	else
	{
		MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &stat); 
		printf("The process %d recived number %d from process %d.\n", rank, number, rank - 1);
		number++;	
		MPI_Send(&number, 1, MPI_INT, (rank + 1)% numranks, 0, MPI_COMM_WORLD);
		//printf("The process %d send number %d to  process %d.\n", rank, number , rank - 1);
	}


    	//make sure every rank is here before continuing 
    	MPI_Barrier(MPI_COMM_WORLD);

	number = 0; // reset number.
	printf("\n");
    	//Ping-Pong -- Your version here
	for (int i = 1; i < numranks; i++ )
	{	
		if (rank == 0)
		{
			printf("The process 0 send number %d to process %d.\n", number, i);
			MPI_Send(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Recv(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
		printf("The process 0 received number %d from process %d.\n", number, i);
		}
		else if (rank == i)
		{	
			MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
			number++;
			printf("The process %d recived number %d from process 0.\n", rank, number);
			printf("The process %d send number %d to process 0.\n",rank, number);
			MPI_Send (&number, 1,MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
	
    	//finalize MPI
    	MPI_Finalize();

	 return 0;
}
