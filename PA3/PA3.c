#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//Change it to a bigger mumber. 
#define SIZE 40000

void gen_matrix(int* matrix, int size)
{
	for (int i = 0; i < size*size; i++)
	{
		matrix [i] = i + 1; 
	}
}

void gen_vector(int* vector, int size)
{
	for (int i = 0; i < SIZE; i++)
	{
		vector [i] = i + 1; 
	}
}

void printMatrix(int* matrix, int size)
{
	printf("%d * %d Matrix:\n[ ",size,size);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			printf("%d ", matrix[i * size + j]); 
		}
		printf("\n");
	}
	printf("]\n\n");
}

void printVector(int* vector, int size)
{
	printf("%d * 1 Vector:\n[ ",size);
	for (int i = 0; i < 10; i++)
	{
		printf("%d ", vector[i]); 
		
	}
	printf("]\n\n");
}

void calVector(int* resultV, int* mymatrix, int* vector, int row)
{
	for(int i = 0; i < row; i++)
	{
		resultV[i] = 0;
		for (int j = 0; j < SIZE; j++)
		{
			resultV[i] += mymatrix[i * SIZE + j] * vector[j];
		}
		
	}
}

// Debug 
void printResult(int* vector, int size)
{
	printf("[");
	for (int i = 0; i < 100; i++)
	{
		printf(" %d", vector[i]); 
		
	}
	printf("]\n");
}

int main(int argc, char** argv)
{
	int rank, numranks, row;

	int mysize = SIZE;
	int* m; //matrix pointer
	int* v; //vector pointer
	int* result;

	MPI_Init(&argc,&argv);	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numranks);

	row = SIZE / numranks;

	if(SIZE % numranks != 0)
	{
		row += 1;
		mysize = row * numranks; 
	}

	v = (int*)malloc(1*SIZE*sizeof(int));

	if(rank == 0)
	{
		m = (int*)malloc(mysize*SIZE*sizeof(int));
		result = (int*)malloc(1*SIZE*sizeof(int));
		gen_matrix(m, SIZE);
		gen_vector(v, SIZE);

		printMatrix(m, SIZE);
		printVector(v, SIZE);
	}
	double bStart = MPI_Wtime();
	MPI_Bcast(v, SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	double bEnd = MPI_Wtime();

	//debug
	//printf("The vector was Bcast. Rank: %d\n", rank);
	//printResult(v, SIZE);
	

	int* mym = (int*) malloc(row*SIZE*sizeof(int)); //holds the value of the rows 

	double scStart = MPI_Wtime();
	MPI_Scatter(m, row * SIZE, MPI_INT, mym, row * SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	double scEnd = MPI_Wtime();
	//debug
	//printf("The vector was Scatter. Rank: %d\n", rank);
	//printResult(mym, (SIZE*SIZE/numranks));
	
	int* myv = (int*) malloc(row * sizeof(int));
	calVector(myv, mym, v, row); // Removed incorrect type declarations

	double gStart = MPI_Wtime();
	MPI_Gather(myv, row, MPI_INT,  result, row, MPI_INT, 0, MPI_COMM_WORLD);
	double gEnd = MPI_Wtime();

	if (rank == 0)
	{
		printf("\nThe final result\n");
		printVector(result, SIZE);

		printf(" %.6f -> Time that took to broadcastt the vertice to all %d ranks.\n", bEnd - bStart, numranks);
		printf(" %.6f -> Time that took to Scatter the matrix to all %d ranks.\n", scEnd - scStart, numranks);
		printf(" %.6f -> Time that took to gather the vertice to all %d ranks.\n", gEnd - gStart, rank);
		free(result);
		free(m);
	}

	//free(m);
	free(v);
	free(mym);
	free(myv);

	MPI_Finalize();

	return 0;
}
