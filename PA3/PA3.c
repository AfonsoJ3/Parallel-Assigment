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
				if (i < SIZE)
				{
					v[i] = i + 1;
				}

				m [i] = i + 1; 
			}

			for (int i = 0; i < SIZE*SIZE; i++)
			{
				printf("%d\n", m[i]);
			}
			printf("\n\n");
			for (int i = 0; i < SIZE; i++)
			{
				printf("%d\n", v[i]);
			}
		}

		

    	MPI_Finalize();

    return 0;
}
