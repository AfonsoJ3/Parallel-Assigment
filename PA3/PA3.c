#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//Change it to a bigger mumber. 
#define SIZE 10

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
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			printf("%d ", matrix[i * size + j]); 
		}
	}
	printf("]\n\n");
}

void printVector(int* vector, int size)
{
	printf("%d * 1 Vector:\n[ ",size);
	for (int i = 0; i < size; i++)
	{
		printf("%d \n", vector[i]); 
		
	}
	printf("]\n\n");
}

// Debug 
void printResult(int* vector, int size)
{
	printf("[");
	for (int i = 0; i < size; i++)
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
	}

	MPI_Bcast(v, SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	//debug
//	printf("The vector was Bcast. Rank: %d\n", rank);
	//printResult(v, SIZE);
	

	int* mym = (int*) malloc(row*SIZE*sizeof(int)); //holds the value of the rows 
	MPI_Scatter(m, row * SIZE, MPI_INT, mym, row * SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	//debug
	printf("The vector was Scatter. Rank: %d\n", rank);
	printResult(mym, (SIZE*SIZE/numranks));
	
	// int product = 0;

	// for (int i = 0; i < partitionLine; i++)
	// {
	// 	for (int j = 0; j < SIZE; j++)
	// 	{
	// 		product += mym[i * SIZE + j] * v[j];
	// 	}
	// }

	// MPI_Gather(&product, 1, MPI_INT,  result, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		// for (int i = 0; i < SIZE; i++)
		// {
		// 	printf("The resul is: %d.\n", result[i]);
		// }
		printMatrix(m, SIZE);
		printVector(v, SIZE);
		free(result);
		free(m);
	}

	//free(m);
	free(v);

	MPI_Finalize();

	return 0;
}
