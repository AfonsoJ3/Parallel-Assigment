#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//Change it to a bigger mumber. 
#define SIZE 3

int main(int argc, char** argv)
{

	int rank,numranks ;

	int* m; //matrix pointer
	int* v; //vector pointer
	int* result;
	MPI_Init(&argc,&argv);	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numranks);

	if(rank == 0)
	{
		m = (int*)malloc(SIZE*SIZE*sizeof(int));
		v = (int*)malloc(1*SIZE*sizeof(int));

		for (int i = 0; i < SIZE*SIZE; i++)
		{
			m [i] = i + 1; 
		}

		for (int i = 0; i < SIZE; i++)
		{
			v [i] = i + 1; 
		}
		
		result = (int*)malloc(1*SIZE*sizeof(int));
	}

	MPI_Bcast( v, SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	//int partition = SIZE/numranks;
	int* mym = (int*) malloc(1*SIZE*sizeof(int)); //holds the value of the rows 
	MPI_Scatter(m, SIZE, MPI_INT, mym, SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	
	int product = 0;

	for (int i = 0; i < SIZE; i++)
	{
		product += mym[i] * v[i];
	}

	MPI_Gather( &product , 1 , MPI_INT ,  result, 1, MPI_INT , 0 , MPI_COMM_WORLD);

	if (rank == 0)
	{
		for (int i = 0; i < SIZE; i++)
		{
			printf("The resul is: %d", result[i]);
		}

		free(result);
		free(mym);
	}

	free(m);
	free(v);

	MPI_Finalize();

	return 0;
}
