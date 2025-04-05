#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <omp.h>
#include <stdbool.h>

extern void matToImage(char* name, int* mat, int* dims);

int main( int argc, char** argv ) 
{
    int rank, numRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &numRank);

    //setup mandelbrot data
    int nx=7680;
    int ny=4320;
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
    int* temp_Matrix = (int*)malloc(nx * ny * sizeof(int));;
    int numele = 100;
    

    //Sending job to workers
    if (rank == 0)
    {
       master_Matrix = (int*) malloc (nx * ny * sizeof(int));
       int* matrix = (int*)malloc(nx * ny * sizeof(int));
       int myStart, myEnd;
       int activeWorkers = numRank - 1;

       
       for (int i = 1; i < numRank; i++)
       {
            myStart = (i - 1) * numele;
            myEnd = myStart + numele - 1;
            
            if (myEnd > ny)
            {
                myEnd = ny;
            }

            printf("Debug: Kick of.\n Rank:%d, numRank:%d, numele:%d, myStart:%d, myEnd:%d.\n",i,numRank, numele, myStart, myEnd);
            if (i == numRank)
            {
                printf("\n\n");
            }

            MPI_Send(&myStart, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&myEnd, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
       }

       while (activeWorkers > 0)
       {
            MPI_Status status;
            MPI_Recv(&myStart, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
            int workerRank = status.MPI_SOURCE;
            MPI_Recv(&myEnd, 1, MPI_INT, workerRank, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&master_Matrix[myStart * nx], (myEnd - myStart) * nx, MPI_INT, workerRank, 1 , MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //MPI_Recv(temp_Matrix, numele * nx, MPI_INT, workerRank, 2, MPI_COMM_WORLD, &status);
            
            // for (int i = startRow; i < endRow; i++) 
            // {
            //     for (int j = 0; j < nx; j++) 
            //     {
            //         master_matrix[i * nx + j] = temp_Matrix[(i - startRow) * nx + j];
            //     }
            // }

            printf("Debug: Received.\nRank:%d, numRank:%d, numele:%d, myStart:%d, myEnd:%d.\n",workerRank,numRank, numele, myStart, myEnd);

            //Bookkeeping
            if (myEnd < ny)
            {
                myStart = myEnd + 1;
                myEnd = myStart + numele - 1;
                if (myEnd > ny) 
                {
                    myEnd = ny;
                }
            }
            else 
            {
                myStart = -1;
                activeWorkers--;
                printf("Debug: Worker %d finished.\n", workerRank);
            }
            MPI_Send(&myStart, 1, MPI_INT, workerRank, 0, MPI_COMM_WORLD);
            MPI_Send(&myEnd, 1, MPI_INT, workerRank, 1, MPI_COMM_WORLD);

            printf("Debug: In While Loop. \nRank:%d, numRank:%d, numele:%d, myStart:%d, myEnd:%d.\n",rank,numRank, numele, myStart, myEnd);
       }
        int dims[2]={ny,nx};
        matToImage("mandelbrot.jpg",master_Matrix,dims);
        printf("Debug: Image created.\n");

        free(master_Matrix);
        MPI_Finalize();
        return 0;
    }
    else
    {
        int startRow, endRow;
        while (true)
        {
            MPI_Recv(&startRow, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            printf("Start row and end row received.\n");

            if (startRow == -1)
            {
                break;
            }
            MPI_Recv(&endRow, 1, MPI_INT, 0, 1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int* worker_matrix = (int*)malloc((endRow - startRow) * nx * sizeof(int));

            #pragma omp parallel 
            {
                //create mandelbrot here
                #pragma omp for nowait schedule(dynamic)
                for(int i = startRow; i < endRow; i++)
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
                        worker_matrix[(i - startRow) * nx + j] = iter;
                    }
                }
            }
            //send the result back to master
            MPI_Send(&startRow, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
            MPI_Send(&endRow, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
            MPI_Send(worker_matrix, (endRow - startRow) * nx, MPI_INT, 0, 1 , MPI_COMM_WORLD);
            free(worker_matrix);
        }
    }
}


               
