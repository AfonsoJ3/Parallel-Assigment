#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int is_prime(int n);

int main(int argc, char** argv)
{
    int rank, numranks; 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 1000;  // Upper limit for prime checking
    int numprimes = 0; // Worker prime count
    int result = 0;    // Master final count
    int rankResult = 0;
    int readyWorker;

    // Master Process
    if (rank == 0)
    {
        if (numranks < 3)
        {
            printf("WARNING: NUMRANKS MUST BE 3 OR MORE. INCREASE NODES IN PBS FILE. TERMINATING PROGRAM.\n");
            MPI_Finalize();
            exit(0);
        }

        int numele = 100; // Larger batch size for efficiency
        int start = 1, end;
        int workers = numranks - 1;

        // Assign initial work to each worker
        for (int i = 1; i < numranks; i++)
        {
            if (start <= n)
            {
                end = start + numele - 1;
                if (end > n) end = n;

                MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
              	//printf("Start: %d - End: %d - Rank: %d.\n", start, end, i);
		        start = end + 1;
            }
            else
            {
                int killSignal = -1;
                MPI_Send(&killSignal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                workers--;
            }
        }

        // Receive results and dynamically assign new tasks
        while (workers > 0)
        {
            MPI_Recv(&readyWorker , 1, MPI_INT, MPI_ANY_SOURCE, 1,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&rankResult, 1, MPI_INT, readyWorker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += rankResult;

            if (start <= n)
            {
                end = start + numele - 1;
                if (end > n) end = n;

                MPI_Send(&start, 1, MPI_INT, readyWorker, 0, MPI_COMM_WORLD);
                MPI_Send(&end, 1, MPI_INT, readyWorker, 0, MPI_COMM_WORLD);
                start = end + 1;
            }
            else
            {
                int killSignal = -1;
                MPI_Send(&killSignal, 1, MPI_INT, readyWorker, 0, MPI_COMM_WORLD);
                workers--;
            }
        }

        printf("Number of Primes: %d\n", result);
    }

    // Worker Processes
    if (rank != 0)
    {
        int start, end;
        double Tstart, Tend;
        while (1)
        {

            MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (start == -1) break; // Termination condition

            MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            numprimes = 0; // Reset count for each batch

            Tstart = MPI_Wtime();
            for (int i = start; i <= end; i++)
            {
                
                if (is_prime(i) == 1)
                {
                    numprimes++;
                }
                
            }
            Tend = MPI_Wtime();
            
            printf("Recived. rank %d have start: %d - end: %d.\n", rank, start, end);

            // FILE *fp = fopen("worker_output.txt", "a");  // Open file in append mode
           
            // if (fp == NULL) 
            // {
            //     printf("Error opening file!\n");
            //     MPI_Finalize();
            //     exit(1);
            // }

            // fprintf(fp, "Rank: %d processed #%d-#%d. Time %.6f\n", rank, start, end, Tend - Tstart);
            // fclose(fp);

            // printf("\nRank: %d #%d-#%d. Time %d\n", rank, start, end, Tend - Tstart);
            // MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            // MPI_Send(&numprimes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                
        }
    }

    MPI_Finalize();
    return 0;
}

int is_prime(int n)
{
    /* Handle special cases */
    if (n == 0) return 0;
    else if (n == 1) return 0;
    else if (n == 2) return 1;

    int i;
    for (i = 2; i <= (int)(sqrt((double) n)); i++)
        if (n % i == 0) return 0;

    return 1;
}
