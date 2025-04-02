#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

extern void matToImage(char* name, int* mat, int* dims);

int main( int argc, char** argv ) 
{
    //setup mandelbrot data
    int rank, numRank;

    MPI_Init( &argc , &argv);
    MPI_Comm_size( MPI_COMM_WORLD ,&numRank);
    MPI_Comm_rank( MPI_COMM_WORLD , &rank);
    
    int nx=900;
    int ny=600;

    double xStart=-2;
    double xEnd=1;
    double yStart=-1;
    double yEnd=1;
    int maxIter=255;
    int* matrix = NULL;
    //C=x0+iy0
    double x0=0;
    double y0=0;
    //Z=x+iy
    double x=0;
    double y=0;

    int iter=0;
    
    int startRow, endRow ;
    int numele = ny / (numRank - 1);

    //Inicially work 
    if (rank == 0)
    {
       matrix = (int*)malloc(nx*ny*sizeof(int));
       startRow = 0;
       endRow = numele;

       for (int i = 1; i < numRank - 1 ; i++)
       {
            int startingRow =  i * numele;
            int endingRow = startingRow + numele;

            if (i == numRank - 1)
            {
                endingRow= ny;
            }

            MPI_Send( &startingRow, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send( &endingRow, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
       }
    }
    else 
    {
        MPI_Recv( &startRow, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
        MPI_Recv( &endRow, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
    }

    int* matrix2 = (int*) malloc (nx*(endRow - startRow)*sizeof(int)); 

    #pragma omp parallel for schedule(dynamic) collapse(2)
    //distrubute the rows evenly among MPI ranks.
    //use the same method as in PA5.x
    //create mandelbrot here
    for(int i=startRow;i<endRow;i++)
    {
        for(int j=0;j<nx;j++)
        {
            //chosen a value for C
            x0=xStart+(1.0*j/nx)*(xEnd-xStart);
            y0=yStart+(1.0*i/ny)*(yEnd-yStart);
            x=0; y=0; //set Z to 0
            iter=0;

            while(iter<maxIter)
            {
                iter++;
                double temp=x*x-y*y+x0;
                y=2*x*y+y0;
                x=temp;
                if(x*x+y*y>4) break;
            }
            matrix2[(i - startRow)*nx+j]=iter;
        }
    }

   if (rank != 0)
   {
     MPI_Send( matrix2, nx*(endRow - startRow), MPI_INT , 0, 0, MPI_COMM_WORLD);
   }
    else
   {
        for (int i = 1; i < numRank - 1 ; i++)
        {
            int startingRow =  i * numele;
            int endingRow = startingRow + numele;

            if (i == numRank - 1)
            {
                endingRow= ny;
            }
            MPI_Recv( matrix + startingRow * nx , nx*(endingRow - startingRow), MPI_INT , i,  0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (int i = 0; i < endRow; i++) 
        {
            for (int j = 0; j < nx; j++) 
            {
                matrix[i * nx + j] = matrix2 [i * nx + j];
            }
        }

        int dims[2] = {ny, nx};
        matToImage("mandelbrot.jpg", matrix, dims);
        free(matrix);
   }

    free(matrix2);
    MPI_Finalize();
    return 0;
}
