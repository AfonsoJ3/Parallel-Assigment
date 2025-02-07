#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv){

    int rank,numranks;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);



    MPI_Finalize();

    return 0;
}
