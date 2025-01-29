#include <stdio.h>
#include <stdlib.h>
#include "mm-student.h"
#include <time.h>

void printMatrix(int n, double* matrix);




//Main function
int main(int argc, char **argv)
{
	//Generate a random number in a range
	srand(time(NULL));
	//change the maxsize if a bigger array is needed. 
	int maxSize = 7, minSize = 1; //max size is 7 bc the matrix would be too big to display.
	int n = rand()%(maxSize - minSize + 1) - minSize;


	if (n < 1)
	{
		n = rand()%(maxSize - minSize +1) - minSize;
	}

	printf("The matrix size: %dx%d\n",n,n);
	//scanf("%d",&n);

	//first array and second array
	double* f_matrix = (double*)malloc(n*n*sizeof(double));
	double* s_matrix = (double*)malloc(n*n*sizeof(double));
	
	if(f_matrix == NULL || s_matrix == NULL)
	{
		printf("Memory allocation failed!");
		return 0;
	}

// generates random numbers
	assignMatrix(f_matrix, n);
	assignMatrix(s_matrix, n);
	
	double* mMatrix = (double* )malloc(n*n*sizeof(double));
	mMatrix = mm(f_matrix, s_matrix, n);
	
	printMatrix(n, f_matrix);
	printMatrix(n, s_matrix);
	printMatrix(n, mMatrix);

	free(f_matrix);
	free(s_matrix);
	free(mMatrix);
	return 0;
}


void assignMatrix(double* mat, int n)
{
	int min = -100;
	int max = 100;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			mat [i * n + j]= rand() % (max - min + 1) + min;
			//mat[i * n + j] = j;
		}
	}
	
}

double* mm(double* A, double* B, int n)
{
	double* mult = (double* ) malloc(n*n* sizeof(double));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			mult[i * n + j] = 0;
			for (int k = 0; k < n; k++)
			{
				mult[i*n+j] +=A[i*n+k] * B[k*n +j]; 
			}
		}
	}
	return mult;
}

void printMatrix(int n, double* matrix) {
    for (int i = 0; i < n; i++) 
	{
        for (int j = 0; j < n; j++) 
		{
            printf("col%d, row%d: %6.2f\n",i+1,j+1,matrix[i * n + j]); // Properly format as a matrix
        }
       
    }
    printf("\n");
}


