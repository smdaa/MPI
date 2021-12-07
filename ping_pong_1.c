#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int th, nth;
    int nb_values=1000;
    int values[nb_values];
    int tag=99;
    int sum;
    MPI_Status status; 

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    if (th == 0) {
        sum = 0;
        for (int i = 0; i < nb_values; i++){
            values[i] = i;
            sum = sum + i;
        }
        
        MPI_Send(values, nb_values, MPI_INTEGER, 1, tag, MPI_COMM_WORLD);
        printf("id:%d, sum of values:%d\n", th, sum);
    } else if (th == 1){
        MPI_Recv(values, nb_values, MPI_INTEGER, 0, tag, MPI_COMM_WORLD, &status);
        sum = 0;
        for (int i = 0; i < nb_values; i++){
            sum = sum + values[i];
        }
        printf("id:%d, sum of values:%d\n", th, sum);
    }

    MPI_Finalize();
}