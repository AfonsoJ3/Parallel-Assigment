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

	MPI_Init(&argc,&argv);	
    	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    	MPI_Comm_size(MPI_COMM_WORLD,&numranks);

		if(rank == 0)
		{
			m = (int*)malloc(SIZE*SIZE*sizeof(int));
			v = (int*)malloc(1*SIZE*sizeof(int));

			for (int i = 0; i < SIZE*SIZE; i++)
			{
				if (i < SIZE - 1)
				{
					v[i] = i + 1;
				}

				m [i] = i + 1; 
			}
		}

		int partition = SIZE/numranks;
		int* mym = (int*) malloc(1*SIZE*sizeof(int));
		MPI_Scatter(m, SIZE, MPI_INT, mym, SIZE, MPI_INT, 0, MPI_COMM_WORLD);

		for(int i = 0; i < SIZE; i++)
		{
			printf("Rank %d: %d\n", rank,mym[i]);
		}
		

    	MPI_Finalize();

    return 0;
}
