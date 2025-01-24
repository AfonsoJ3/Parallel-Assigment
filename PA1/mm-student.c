#include <stdio.h>
#include <stdlib.h>
#include "mm-student.h"
#include <time.h>

void printMatrix(int n, double* matrix);




//Main function
int main(int argc, char **argv)
{
	int n;
	printf("Enter a size of the matrix\n>>");
	scanf("%d",&n);

	//Generate a number that is in a rnage
	srand(time(NULL));
	
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


