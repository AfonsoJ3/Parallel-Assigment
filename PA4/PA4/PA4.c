#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

extern int* imageToMat(char* name, int* dims);
extern void matToImage(char* name, int* mat, int* dims);



int main(int argc, char* argv[])
{
    int* matrix = NULL;
    //int* m;
    int* temp; //temporary file
    char* name = "image.jpg"; // fale name.
    int* dims = (int*) malloc(2*sizeof(int)); //image dimension
    int height;
    int width;

    double totalB = 0;
    double totalB_m = 0;
    double totalcal = 0;
    double totalg = 0;

    int rank, numranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD , &numranks);

    if (rank == 0)
    {
        matrix = imageToMat(name, dims);
        height = dims[0];
        width = dims[1];
    }
    

    double Bstart = MPI_Wtime();
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);        
    double Bend = MPI_Wtime();

    if (rank != 0) {
        matrix = (int*) malloc(height * width * sizeof(int));
    }

    double Bstart_m = MPI_Wtime();
    MPI_Bcast(matrix, height * width, MPI_INT, 0, MPI_COMM_WORLD);
    double Bend_m = MPI_Wtime();

    int numRows = height / numranks;
    int myStart = rank * numRows;
    int myEnd = myStart + numRows;

    if (rank == numranks - 1)
    {
        myEnd = height;
    }

    int k = 20;
    temp=(int*) malloc(height*width*sizeof(int));

    double calStart = MPI_Wtime();
    for(int i = myStart; i < myEnd; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = i * width + j;
            int sum = 0;
            int counter = 0;
            for(int u = -k; u <= k; u++)
            {
                for (int v = -k; v <= k; v++)
                {
                    int cindex=(i-u)*width+(j-v);
                    if(i-u<0 || i-u>=height ||j-v<0 || j-v>=width)
                    {
                        continue;
                    }
                    sum+=matrix[cindex];
                    counter++;
                }
                
            }

            temp[index] = sum / counter;
        }
    }
    double calEnd = MPI_Wtime();

    double gStart = MPI_Wtime();
    MPI_Gather( temp + myStart * width, numRows * width, 
    MPI_INT, temp, numRows * width, MPI_INT, 0, MPI_COMM_WORLD);
    double gEnd = MPI_Wtime();

    if (rank == 0)
    {
        matToImage("processedImage.jpg", temp, dims);
        printf("\nThe Bcast with %d ranks, took %.10f seconds\n", numranks, (Bend-Bstart)+(Bend_m-Bstart));
        printf("The calculation with %d ranks, took %.10f seconds\n", numranks, (calEnd - calStart));
        printf("The gather with %d ranks, took %.10f seconds\n", numranks, (gEnd - gStart));

    }

    if (rank != 0)
    {
        free(matrix);
    }

    free(dims);
    free(temp);


    MPI_Finalize();

    return 0;
}
