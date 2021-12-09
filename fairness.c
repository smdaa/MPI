#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int th, nth, buf[1];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    if (th == 0){
        for (int i = 0; i < 100*(nth-1); i++)
        {
            MPI_Recv(buf, 1, MPI_INTEGER, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf( "msg from %d with tag %d\n", status.MPI_SOURCE, status.MPI_TAG);
        }

    } else {
        for (int i = 0; i < 100; i++)
        {
            MPI_Send(buf, 1, MPI_INTEGER, 0, i, MPI_COMM_WORLD);
        }
        
    }
    MPI_Finalize();
}