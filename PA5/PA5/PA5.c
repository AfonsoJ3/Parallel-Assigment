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

    //Master 
    if (rank == 0)
    {
        int numele=N/(numranks - 1);
        int start, end;

        for(int i=1;i<numranks;i++)
        { 
            start=(i-1)*numele+1;
            end=start+numele-1;
            if(rank==numranks-1) 
            { 
                end=N;
            }

            MPI_Send(&start, 1, MPI_INT ,  i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT ,  i, 0, MPI_COMM_WORLD);

        }
    }

    double result = 0;
    double rankResult = 0;
    for (int i=1; i<numranks; i++)
    {
        MPI_Recv(&rankResult,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,MPI_STATUS_I    GNORE);
    }
    printf("Number of Primes: %d\n",numprimes);

    //Worker
    if(rank!=0)
    {
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Funtion Call -> calculation. 
        for (i = 1; i <= n; i++)
        {
            if (is_prime(i) == 1)
            {
                numprimes ++;
            }
        }
        MPI_Send(&numprimes, 1, MPI_DOUBLE, MPI_COMM_WORLD);        
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
