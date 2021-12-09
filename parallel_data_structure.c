#include "mpi.h"
#include <stdio.h>
#define maxn 12

int main(int argc, char *argv[]) {

    int th, nth, errcnt, toterr;
    int tag = 99;
    MPI_Status status; 
    double xlocal[(maxn/4)+2][maxn];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);
    if (nth != 4) MPI_Abort(MPI_COMM_WORLD, 1);

    for (int i = 1; i <= maxn/nth; i++)
    {
        for (int j = 0; j < maxn; j++)
        {
            xlocal[i][j] = th;
        }   
    }
    for (int j = 0; j < maxn; j++)
    {
        xlocal[0][j] = -1;
        xlocal[maxn/nth + 1][j] = -1;
    }
    /*
    if (th < nth - 1) 
	MPI_Send( xlocal[maxn/nth], maxn, MPI_DOUBLE, th + 1, 0, 
		  MPI_COMM_WORLD );
    if (th > 0)
	MPI_Recv( xlocal[0], maxn, MPI_DOUBLE, th - 1, 0, MPI_COMM_WORLD, 
		  &status );
    if (th > 0) 
	MPI_Send( xlocal[1], maxn, MPI_DOUBLE, th - 1, 1, MPI_COMM_WORLD );
    if (th < nth - 1) 
	MPI_Recv( xlocal[maxn/nth+1], maxn, MPI_DOUBLE, th + 1, 1, 
		  MPI_COMM_WORLD, &status );
    */
   if (th > 0)
   {
       MPI_Sendrecv(xlocal[1], maxn, MPI_DOUBLE, th-1, 1, xlocal[0], maxn, MPI_DOUBLE, th-1, 0, MPI_COMM_WORLD, &status);
   }
   if (th < nth - 1)
   {
       MPI_Sendrecv(xlocal[maxn/nth], maxn, MPI_DOUBLE, th+1, 0, xlocal[maxn/nth+1], maxn, MPI_DOUBLE, th+1, 1, MPI_COMM_WORLD, &status);
   }
   
    /* Check that we have the correct results */
    errcnt = 0;
    for (int i=1; i<=maxn/nth; i++) 
	for (int j=0; j<maxn; j++) 
	    if (xlocal[i][j] != th) errcnt++;
    for (int j=0; j<maxn; j++) {
	if (xlocal[0][j] != th - 1) errcnt++;
	if (th < nth-1 && xlocal[maxn/nth+1][j] != th + 1) errcnt++;
    }

    MPI_Reduce( &errcnt, &toterr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
    if (th == 0) {
	if (toterr)
	    printf( "! found %d errors\n", toterr );
	else
	    printf( "No errors\n" );
    }

    MPI_Finalize();

}