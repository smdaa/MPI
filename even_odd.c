#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int th, nth;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    if ((th % 2) == 0){
        printf("even\n");
    } else {
        printf("odd\n");
    }

    MPI_Finalize();
}