#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int th, nth;
    int nb_values=1000;
    int values[nb_values];
    int sum;
    int tag=99;
    MPI_Status status; 

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    if (th == 0){
        
        for (int i = 0; i < nb_values; i++){
            values[i] = i;
        }
        MPI_Send(values, nb_values, MPI_INTEGER, 1, tag, MPI_COMM_WORLD);

    } else {
        MPI_Recv(values, nb_values, MPI_INTEGER, th-1, tag, MPI_COMM_WORLD, &status);
        if (th < nth-1)
        {
            MPI_Send(values, nb_values, MPI_INTEGER, th+1, tag, MPI_COMM_WORLD);
        }
    }

    sum = 0;
    for (int i = 0; i < nb_values; i++){
        sum = sum + values[i];
    }
    printf("%d: sum=%d\n", th, sum);
    MPI_Finalize();

}