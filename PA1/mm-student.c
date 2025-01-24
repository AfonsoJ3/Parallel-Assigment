#include <stdio.h>
#include <stdlib.h>
#include "mm-student.h"
#include <time.h>

//Main function
int main(int argc, char **argv)
{
	int n = 10;

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
	mMtrix = mm(f_matrix, m_matrix, n);
	

	for (int i = 0; i < n; i++)
	{
		printf("Array%d: %f\n", i, f_matrix[i]);
	}

	free(f_matrix);
	free(s_matrix);
	free(mMatrix)
	return 0;
}

void assignMatrix(double* mat, int n)
{
	int min = -100;
	int max = 100;

	//Generate a number that is in a rnage
	srand(time(NULL));

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			 mat [i * n + j]= rand() % (max - min + 1) + min;
		}
	}
}

double* mm(double* A, double* B, int n)
{
	double* mult = (double* ) malloc(n*n* sizeof(double))
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < n; k++)
			{
				mult[i*n+j] =A[i*n+k] * B[k*n +j]; 
			}
		}
	}
	return mult;
}

