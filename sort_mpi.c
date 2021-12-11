#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int compare(const void * first, const void * second) {
    int firstInt = * (const int *) first;
    int secondInt = * (const int *) second;
    return firstInt - secondInt;
}

int main(int argc, char *argv[]) {
    int th, nth;
    int n, nlocal;
    int x[n];
    int xlocal[nlocal];
    int sendcounts[nth];
    int displs[nth];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);
    if (th == 0)
    {
        /* Read array */
        FILE *fp;
		fp =fopen( "x.dat", "r" );
		if (!fp) MPI_Abort(MPI_COMM_WORLD, 1);
        fscanf(fp, "%d", &n);
        fscanf(fp, "\n");
        for (int i=0; i<n; i++)
        {
            fscanf(fp, "%d", &x[i]);
        }
    }
    MPI_Bcast(&n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    if (th == nth - 1)
    {
        nlocal = n - (nth - 1)*(n/nth);
    } else
    {
        nlocal = n/nth;
    }
    MPI_Gather(&nlocal, 1, MPI_INTEGER, sendcounts, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
    if (th == 0)
    {
        displs[0] = 0;
        for (int i = 1; i < nth; i++)
        {
            displs[i] = displs[i - 1] + sendcounts[i - 1];
        }               
    }
    MPI_Scatterv(x, sendcounts, displs, MPI_INTEGER, xlocal, nlocal, MPI_INTEGER, 0, MPI_COMM_WORLD);
    qsort(xlocal, nlocal, sizeof(int), compare);    
    if (th > nth - 1)
    {
        MPI_Send(xlocal, nlocal, MPI_INTEGER, th + 1, MPI_ANY_TAG, MPI_COMM_WORLD);
    }
    
    if (th > 0)
    {
        int xlocal2[nlocal + n/nth];
        for (int i = 0; i < nlocal; i++)
        {
            xlocal2[i] = xlocal[i];
        }
        
        MPI_Recv(&xlocal2[nlocal], n/nth, MPI_INTEGER, th - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        qsort(xlocal2, nlocal + n/nth, sizeof(int), compare);

    }
    
    
        
    MPI_Finalize();
}