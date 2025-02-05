#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define SIZE (1024L * 1024 * 1024 / sizeof(int))
int main(int argc, char** argv)
{

    	int rank;
    	int numranks;

	int* number = (int*) malloc(SIZE*sizeof(int));	
	
    	//initialize MPI
    	MPI_Init(&argc,&argv);
    	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    	MPI_Comm_size(MPI_COMM_WORLD,&numranks);
	
	MPI_Status stat;
	

    	//Ring -- Your version here
	if (rank == 0)
	{	
		double start = MPI_Wtime();
		printf("The process 0 send array to process 1.\n");
		MPI_Send(number, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(number, SIZE, MPI_INT, numranks - 1, 0, MPI_COMM_WORLD, &stat);
		double end = MPI_Wtime();
		printf("The process 0 recived array from process %d.\n", numranks - 1);
		printf("Time: %.8f.\n", end - start);
	}

	else
	{
		MPI_Recv(number, SIZE, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &stat); 
		printf("The process %d received array from process %d.\n", rank, rank - 1);
		
		MPI_Send(number, SIZE, MPI_INT, (rank + 1)% numranks, 0, MPI_COMM_WORLD);
		
		if (rank == numranks - 1)
		{
			printf("The process %d send array to process 0.\n", rank);
		}
		else
		{
			
			printf("The process %d send array to  process %d.\n", rank, rank + 1);
		}
	}


    	//make sure every rank is here before continuing 
    	MPI_Barrier(MPI_COMM_WORLD);
	
	double start1 = MPI_Wtime();
    	//Ping-Pong -- Your version here
		
		if (rank == 0)
		{	
			for (int i = 1; i < numranks; i++ )
			{
				printf("The process 0 send array to process %d.\n", i);
				MPI_Send(number, SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Recv(number, SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
				printf("The process 0 received array from process %d.\n", i);
			}
		}
		else
		{	
			MPI_Recv(number, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
			printf("The process %d recived array from process 0.\n",rank);
			printf("The process %d send array to process 0.\n",rank);
			MPI_Send (number, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	
	double end1 = MPI_Wtime();
	if (rank == 0)
	{
		printf("Time: %.8f.", end1 - start1);
	}
    	//finalize MPI
    	MPI_Finalize();

	 return 0;
}
