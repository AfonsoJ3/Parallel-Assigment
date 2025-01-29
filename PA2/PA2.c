#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv){

    int rank;
    int numranks;
    //initialize MPI
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);

    //Ring -- Your version here




    //make sure every rank is here before continuing 
    MPI_Barrier(MPI_COMM_WORLD);


    //Ping-Pong -- Your version here




    //finalize MPI
    MPI_Finalize();

    return 0;
}
