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

    int numRows = height / numranks;
    int myStart = rank * numRows;
    int myEnd = myStart + numRows;

    if (rank == numranks - 1)
    {
        myEnd = height;
    }

    int k = 10;
    temp=(int*) malloc(height*width*sizeof(int));

    for(int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = j * width * i;
            int sum = 0;
            int counter = 0;

            for(int u = -k; u <= k; u++)
            {
                for (int v = -k; v <= k; v++)
                {
                    int cindex = (i - u) * width + (j - v);
                    if(i-u<0 || i-u>=height ||j-v<0 || j-v>=width)
                    {
                        continue;
                    }
                    sum+=matrix[cindex];
                    counter++;
                }
                
            }

            temp[index] = sum / coumter;
        }
    }

    matToImage("processedImage.jpg", temp, dims);

    if (rank == 0)
    {
        printf("\nThe height of image.jpg is: %d\n", height);
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
