#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int th, nth;
    int n;
    double h, sum, x, pi, pi_g;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    if (th == 0)
    {
        printf("enter number of intervals n:");
        fflush(stdout);
        scanf("%d",&n);
    }
    MPI_Bcast(&n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
    h   = 1.0 / (double) n;
    sum = 0.0;
    for (int i = th + 1; i <= n; i += nth) {
	    x = h * ((double)i - 0.5);
	    sum += 4.0 / (1.0 + x*x);
    }
    pi = h * sum;
    MPI_Reduce(&pi, &pi_g, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (th == 0){
        printf("pi :%f\n", pi_g);
    }    

    MPI_Finalize();

}