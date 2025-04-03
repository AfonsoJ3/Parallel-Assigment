#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <omp.h>

extern void matToImage(char* name, int* mat, int* dims);

int main( int argc, char** argv ) 
{
    int rank, numRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &numRank);

    //setup mandelbrot data
    int nx=900;
    int ny=600;
    int* matrix = (int*) malloc(nx* ny * sizeof(int));;
    double xStart=-2;
    double xEnd=1;
    double yStart=-1;
    double yEnd=1;
    int maxIter=255;
    //C=x0+iy0
    double x0=0;
    double y0=0;
    //Z=x+iy
    double x=0;
    double y=0;

    int iter=0;

    for(int i = 0; i < ny; i++)
    {
        for(int j = 0; j < nx; j++)
        {
            //chosen a value for C
            x0 = xStart + (1.0 * j / nx) * (xEnd - xStart);
            y0 = yStart + (1.0 * i / ny) * (yEnd - yStart);
            x = 0; 
            y = 0;//set Z to 0
            iter = 0;

            while(iter < maxIter)
            {
                iter++;
                double temp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = temp;
                if (x * x + y * y > 4) 
                    break;
            }
            matrix[i * nx + j] = iter;
        }
    }

    int dims[2]={ny,nx};
    matToImage("mandelbrot.jpg",matrix,dims);

    free(matrix);
    MPI_Finalize();
    return 0;
}