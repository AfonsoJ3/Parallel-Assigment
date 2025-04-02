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
    int* worker_matrix = (int*) malloc ( nx * ny * sizeof(int));
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
    int* master_Matrix = NULL;
    int numele = nx / numRank;
    int WmyStart, WmyEnd;

    //Sending job to workers
    if (rank == 0)
    {
       master_Matrix = (int*) malloc (nx * ny * sizeof(int));
       
       for (int i = 0; i < numRank; i++)
       {
            int myStart = i * numele;
            int myEnd = myStart + numele;
            
            if (i == numRank -1)
            {
                myEnd = nx;
            }

            printf("Debug: Rank:%d, numRank:%d, numele:%d, myStart:%d, myEnd:%d.\n",i,numRank, numele, myStart, myEnd);
            

            MPI_Send(&myStart, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&myEnd, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
       }
    }
    else
    {
        MPI_Recv( &WmyStart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv( &WmyEnd, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //printf("Debug: Rank:%d, myStart:%d, myEnd:%d.\n", rank, myStart, myEnd);
        #pragma omp parallel 
        {
            //create mandelbrot here
            #pragma omp for nowait schedule(dynamic)
            for(int i = WmyStart; i < WmyEnd; i++)
            {
//                printf("TCS:%d, myStart:%d, myEnd: %d.\n", i, WmyStart, WmyEnd);
    
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
                    worker_matrix[i * nx + j] = iter;
                }
            }
        }
    }

    if (rank != 0)
    {
        MPI_Send(worker_matrix, (WmyEnd - WmyStart) * ny, MPI_INT, 0, 0, MPI_COMM_WORLD);

        printf("\nworker matrix send.\n");
    }
    else
    {
        MPI_Recv(master_Matrix, nx * ny, MPI_INT, 0, MPI_ANY_SOURCE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\nmatrix recived.\n");
        
        //save image
        int dims[2]={ny,nx};
        matToImage("mandelbrot.jpg",master_Matrix,dims);
        free(master_Matrix);
    }

    free(worker_matrix);
    MPI_Finalize();
    return 0;
}
