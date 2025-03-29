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
    int n = 10000000;
    int numprimes = 0;
    int result = 0;
    int rankResult = 0;

    // Master
    if (rank == 0)
    {
        if (numranks < 3)
        {
            printf("WARNING: NUMRANKS MUST BE MORE OR EQUAL TO 3. CHANGE NODES AT PBS FILE. END PROGRAM.\n");
            return 0;
        }

        int numele = 1000; // the amount each rank will take
        int start = 1, end, workers = numranks - 1;

        for (int i = 1; i < numranks; i++)
        {             
            if (start <= n) // check if start did not pass the upper limit
            {
                end = start + numele - 1; // holds the last number of the range
                if (end > n)
                {
                    end = n;
                }

                MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("Debug: Sent range from %d to %d to rank %d\n", start, end, i);
                
                start = end + 1; // gets the new start of the chunk
                printf("Debug: Start updated to %d.\n", start);

                // Receive result from worker
                MPI_Recv(&rankResult, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Debug: Received prime count from rank %d.\n", i);
                result += rankResult;
            }
            else // if passed, send a kill signal 
            {
                int killSignal = -1;
                MPI_Send(&killSignal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                workers--;
            }
        }

        while (workers > 0)
        {
            int killCount = 0;
            
            for (int i = 1; i < numranks; i++)
            {   
                MPI_Recv(&rankResult, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                result += rankResult;

                if (start <= n) // check if start did not pass the upper limit
                {
                    end = start + numele - 1;
                    if (end > n)
                    {
                        end = n;
                    }

                    MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    printf("Debug: Sent range from %d to %d to rank %d\n", start, end, i);
                    
                    start = end + 1; // gets the new start of the chunk
                    printf("Debug: Start updated to %d.\n", start);
                }
                else // if passed, send a kill signal 
                {
                    int killSignal = -1;

                    MPI_Send(&killSignal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    killCount++; // Increment the kill signal count
                }
            }

            workers -= killCount; // Update worker count after receiving kill signals
        }
        printf("Number of Primes: %d\n", result);
    }

    // Worker
    if (rank != 0)
    {
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        numprimes = 0;
        // Function call -> calculation
        for (int i = start; i <= end; i++)
        {
            if (is_prime(i) == 1)
            {
                numprimes++;
            }
        }

        printf("Debug: Received range from %d to %d.\n", start, end);
        MPI_Send(&numprimes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);        
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
