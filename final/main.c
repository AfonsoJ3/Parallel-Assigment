#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

extern void matToImage(char* name, int* mat, int* dims);

int main(int argc, char** argv) 
{
    int rank, numRanks;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int nx = 900;
    int ny = 600;
    double xStart = -2, xEnd = 1, yStart = -1, yEnd = 1;
    int maxIter = 255;
    int* matrix = NULL;

    if (rank == 0) {
        matrix = (int*)malloc(nx * ny * sizeof(int));
        int nextRow = 0;
        int activeWorkers = numRanks - 1;
        
        for (int i = 1; i < numRanks; i++) {
            MPI_Send(&nextRow, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            nextRow++;
        }
        
        while (activeWorkers > 0) {
            int rowIdx;
            int* rowData = (int*)malloc(nx * sizeof(int));
            MPI_Status status;
            MPI_Recv(rowData, nx, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int workerRank = status.MPI_SOURCE;
            MPI_Recv(&rowIdx, 1, MPI_INT, workerRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            for (int j = 0; j < nx; j++) {
                matrix[rowIdx * nx + j] = rowData[j];
            }
            free(rowData);
            
            if (nextRow < ny) {
                MPI_Send(&nextRow, 1, MPI_INT, workerRank, 0, MPI_COMM_WORLD);
                nextRow++;
            } else {
                int stopSignal = -1;
                MPI_Send(&stopSignal, 1, MPI_INT, workerRank, 0, MPI_COMM_WORLD);
                activeWorkers--;
            }
        }
        
        int dims[2] = {ny, nx};
        matToImage("mandelbrot.jpg", matrix, dims);
        free(matrix);
    } else {
        while (1) {
            int rowIdx;
            MPI_Recv(&rowIdx, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (rowIdx == -1) break;
            
            int* rowData = (int*)malloc(nx * sizeof(int));
            double x0, y0, x, y;
            int iter;
            
            #pragma omp parallel 
            {
                #pragma omp for nowait schedule(dynamic)
                for(int i = rowIdx; i < rowIdx; i++)
                {
                    for(int j = 0; j < nx; j++)
                    {
                        x0 = xStart + (1.0 * j / nx) * (xEnd - xStart);
                        y0 = yStart + (1.0 * i / ny) * (yEnd - yStart);
                        x = 0;
                        y = 0;
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
                        rowData[j] = iter;
                    }
                }
            }
            
            MPI_Send(rowData, nx, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&rowIdx, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            free(rowData);
        }
    }
    
    MPI_Finalize();
    return 0;
}
