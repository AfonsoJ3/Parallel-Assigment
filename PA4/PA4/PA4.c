#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

extern int* imageToMat(char* name, int* dims);
extern void matToImage(char* name, int* mat, int* dims);

int main(int argc, char* argv[])
{
    int* matrix;
    int* m;
    int* temp; //temporary file
    char* name = "image.jpg"; // fale name.
    int* dims = (int*) malloc(2*sizeof(int)); //image dimension
    int height;
    int width;

    int rank, numranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD , &numranks);

    if (rank == 0)
    {
        m = imageToMat(name, dims);
        height = dims[0];
        width = dims[1];
    }

    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        matrix = (int*) malloc(height*width*sizeof(int)); //memory alocation non root ranks
    }

    MPI_Bcast(matrix, height * width, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("The height of image.jpg is: %d\n", height);
        printf("The width of image.jpg is: %d\n", width);

    }

    if (rank != 0)
    {
        free(matrix);
    }

    free(m);
    free(dims);
    free(temp);


    MPI_Finalize();

    return 0;
}
