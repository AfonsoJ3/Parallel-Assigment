#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <omp.h>
#include <stdbool.h>

extern void matToImage(char* name, int* mat, int* dims);

int main(int argc, char** argv) {
    int rank, numRanks;
    bool runOnce = true;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

    // Mandelbrot parameters
    int nx = 7680 ;
    int ny = 4320;
    int maxIter = 255;
    double xStart = -2;
    double xEnd = 1;
    double yStart = -1; 
    double yEnd = 1;

    int* matrix = (int*)malloc(nx * ny * sizeof(int));
    int numele = 100; // Number of rows per task
    int workers = numRanks - 1;

    if (rank == 0) {
        // Master process
        int* master_matrix = (int*)malloc(nx * ny * sizeof(int));
        int nextRow = 0, activeWorkers = 0;

        // Assign initial work to workers
        for (int i = 1; i < numRanks; i++) {
            if (nextRow < ny) {
                int startRow = nextRow;
                int endRow = startRow + numele;
                if (endRow > ny) 
                {
                    endRow = ny;
                }


                MPI_Send(&startRow, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&endRow, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                nextRow = endRow;
                activeWorkers++;
            } 
            else 
            {
                int killSignal = -1;
                MPI_Send(&killSignal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }

        // Receive results and assign new work dynamically
        while (activeWorkers > 0) {
            int startRow, endRow;
            MPI_Status status;

            // Receive completed chunk from a worker
            MPI_Recv(matrix, numele * nx, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
            int workerRank = status.MPI_SOURCE;
            MPI_Recv(&startRow, 1, MPI_INT, workerRank, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&endRow, 1, MPI_INT, workerRank, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Store received results in the master matrix
            for (int i = startRow; i < endRow; i++) 
            {
                for (int j = 0; j < nx; j++) 
                {
                    master_matrix[i * nx + j] = matrix[(i - startRow) * nx + j];
                }
            }

            // Assign new work or terminate worker
            if (nextRow < ny) 
            {
                startRow = nextRow;
                endRow = startRow + numele;
                if (endRow > ny) endRow = ny;

                MPI_Send(&startRow, 1, MPI_INT, workerRank, 0, MPI_COMM_WORLD);
                MPI_Send(&endRow, 1, MPI_INT, workerRank, 1, MPI_COMM_WORLD);
                nextRow = endRow;
            } 
            else 
            {
                int killSignal = -1;
                MPI_Send(&killSignal, 1, MPI_INT, workerRank, 0, MPI_COMM_WORLD);
                activeWorkers--;
            }
        }

        // Save final image
        int dims[2] = {ny, nx};
        matToImage("mandelbrot.jpg", master_matrix, dims);
        free(master_matrix);
    } 

    else
    {
        // Worker processes
        int startRow, endRow;
        while (1) 
        {
            MPI_Recv(&startRow, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (startRow == -1) 
            {
                break; // Termination signal
            }


            MPI_Recv(&endRow, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int chunkSize = (endRow - startRow) * nx;
            int* local_matrix = (int*)malloc(chunkSize * sizeof(int));

            double RankStart = MPI_Wtime();
            #pragma omp parallel
            {
                #pragma omp parallel for nowait schedule(dynamic)
                int tid = omp_get_thread_num();
                double start = omp_get_wtime();
                for (int i = startRow; i < endRow; i++) 
                {
                    for (int j = 0; j < nx; j++) 
                    {
                    double x0 = xStart + (1.0 * j / nx) * (xEnd - xStart);
                    double y0 = yStart + (1.0 * i / ny) * (yEnd - yStart);
                    double x = 0, y = 0;
                    int iter = 0;

                    while (iter < maxIter) 
                    {
                        iter++;
                        double temp = x * x - y * y + x0;
                        y = 2 * x * y + y0;
                        x = temp;
                        if (x * x + y * y > 4) break;
                    }
                    local_matrix[(i - startRow) * nx + j] = iter;
                    }
                }
                double end = omp_get_wtime();
                if (rank == 2) 
                {
                    printf("Rank: %d,TID: %d, Time taken: %f seconds\n", rank, tid, end - start);
                }
            }
            double RankEnd = MPI_Wtime();
           
            if (startRow == 1000 && endRow == 1100)
            {
                if (runOnce) 
                {
                    printf("Rank: %d, Time taken: %f seconds\n\n", rank, RankEnd - RankStart);
                    runOnce = false;
                }
            }
            // Send results back to the master
            MPI_Send(local_matrix, chunkSize, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Send(&startRow, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
            MPI_Send(&endRow, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);

            free(local_matrix);
        }
    }

    free(matrix);
    MPI_Finalize();
    return 0;
}
