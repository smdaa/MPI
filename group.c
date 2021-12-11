#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int th, th1,  nth;
    MPI_Comm newcomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    int color = th % 2;
    printf(":%d, color:%d\n", th, color);
    MPI_Comm_split(MPI_COMM_WORLD, color, th, &newcomm);
    MPI_Comm_rank(newcomm, &th1);

    printf("I am %d in MPI_COMM_WORLD and %d in newcomm\n", th, th1);
    MPI_Comm_free(&newcomm);
    MPI_Finalize();
}