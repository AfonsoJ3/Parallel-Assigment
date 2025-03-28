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
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int n=10000000;
    int numprimes = 0;
    int i;
    int result = 0;
    int rankResult = 0;

    //Master 
    if (rank == 0)
    {
        if (numranks < 3)
        {
            printf("WARNING: NUMRANKS MUST BE MORE OR EQUAL TO 3. CHANGE NODES AT PBS FILE. END PROGRAM.");
            return 0;
        }

        int numele= 1000; //the amount each rank will take
        int start, end, myStart, workers;
        start = (i-1) * numele + 1;
        workers = numranks - 1;

        for(int i=1;i<numranks;i++)
        {             
            if (start <= n) // check if start did not passed the upper limit
            {
                //myStart = start; // holds the start position
                end = start + numele - 1; // holds the last numnber of the range. example -> start = 0 end = 999. start = 1000 end = 1999. 
                if(end > n)
                {
                    end = n;
                }
                //start = end + 1; //gets the new start of the chuck.
                MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("Debug: Send start and end to rank %d\n", i);
                
                start = end + 1; //gets the new start of the chuck.
                printf("Debug: start updated. From %d to %d.\n", start - end, start);

                if (workers > 0)
                {
                    MPI_Recv(&rankResult,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                    printf("Debug: Reviced prime. Adding to final result.\n");
                    result += rankResult;
                }
            }
            else //if passed send a kill signal 
            {
                int killSignal = -1;
                MPI_Send( &killSignal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                workers --;
            }

        }
        printf("Number of Primes: %d\n", result);
    }

    //Worker
    if(rank!=0)
    {
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int prime = end + start;

        // //Funtion Call -> calculation. 
        // for (i = start; i <= end; i++)
        // {
        //     if (is_prime(i) == 1)
        //     {
        //         numprimes ++;
        //     }
        // }
        printf("Debug: Recived start %d and end %d.\n", start, end);
        MPI_Send(&prime, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);        
    }
    MPI_Finalize();
    return 0;
}

int is_prime(int n)
{
    /* handle special cases */
    if      (n == 0) return 0;
    else if (n == 1) return 0;
    else if (n == 2) return 1;

    int i;
    for(i=2;i<=(int)(sqrt((double) n));i++)
        if (n%i==0) return 0;

    return 1;
}
