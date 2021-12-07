#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    long nbblock;
    long i;
    double width;
    double sum;
    double sum_g;
    double x;
    int th, nth;
    long start;
    long end;
    double t1, t2, delta_t, delta_tg;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    nbblock = 3*1000*1000LL*100;
    width = 1.0/nbblock;
    start = (th*nbblock)/nth;
    end = (th+1)*(nbblock/nth);
    sum = 0;
    t1 = MPI_Wtime();
    for (i=start; i<end; i++) {
        x = width*(i+0.5);
        sum = sum + width*(4.0 / (1.0 + x*x));
    }
    t2 = MPI_Wtime();
    delta_t = t2 - t1;
    MPI_Reduce(&sum, &sum_g, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&delta_t, &delta_tg, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (th == 0) {
        printf("global time : %f\n", delta_tg);
        printf("Pi = %.12lf\n", sum_g);
    }
    i = end-start;
    MPI_Allreduce(MPI_IN_PLACE,&i,1,MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);
    if (th == 0){
        printf("%ld == %ld\n", i, nbblock);
    }
    MPI_Finalize();

}