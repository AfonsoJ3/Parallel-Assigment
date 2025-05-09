#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

//Change it to a bigger mumber. 
#define SIZE 30000

void gen_matrix(int* matrix, int size)
{
	srand(time(NULL)); 
	for (int i = 0; i < size*size; i++)
	{
		matrix [i] = rand() % SIZE; 
	}
}

void gen_vector(int* vector, int size)
{
	srand(time(NULL) + 1);
	for (int i = 0; i < SIZE; i++)
	{
		vector [i] = rand() % SIZE; 
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
	
		printf("Printing the first 10 elements!!!The size of matrix and vector are powers of 2.\n");		
	
		//Debug
		printMatrix(m, SIZE);
		printVector(v, SIZE);
	}

	double bStart = MPI_Wtime();
	MPI_Bcast(v, SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	double bEnd = MPI_Wtime();

	int* mym = (int*) malloc(row*SIZE*sizeof(int)); 

	double scStart = MPI_Wtime();
	MPI_Scatter(m, row * SIZE, MPI_INT, mym, row * SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	double scEnd = MPI_Wtime();
	
	int* myv = (int*) malloc(row * sizeof(int));
	double calStart = MPI_Wtime();
	calVector(myv, mym, v, row);
	double calEnd = MPI_Wtime();

	long long flops = row * (2 * SIZE - 1);
	double flops_per_sec = flops / (calEnd - calStart);

	double gStart = MPI_Wtime();
	MPI_Gather(myv, row, MPI_INT,  result, row, MPI_INT, 0, MPI_COMM_WORLD);
	double gEnd = MPI_Wtime();

	if (rank == 0)
	{
		printf("\nThe final result\n");
		printVector(result, SIZE);

		printf(" %.10f -> Time that took to broadcastt the vector to all %d ranks.\n", bEnd - bStart, numranks);
		printf(" %.10f -> Time that took to Scatter the matrix to all %d ranks.\n", scEnd - scStart, numranks);
		printf(" %.10f -> Time that took to gather the vector to all %d ranks.\n", gEnd - gStart, rank);
		printf(" %.10f -> Time that took to calculate the vector.\n", calEnd - calStart);
		printf(" %.2f FLOPS (Floating Point Operations Per Second)\n", flops_per_sec);
		
		//free memory
		free(result);
		free(m);
	}

	//free memory ;
	free(v);
	free(mym);
	free(myv);

	MPI_Finalize();

	return 0;
}
