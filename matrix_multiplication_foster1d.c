/* Foster 1-D matrix data decomposition */
/*
    1-D column wise decomposition
    Each task 
        Utilizes subset of cols of A, B, C.
        Responsible for calculating its Cij
        Requires full copy of A
        Requires (N^2)/P data from each of the other (P − 1) tasks.
*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define n 4
#define k 4
#define m 4

int main(int argc, char *argv[]) {
    int th, nth;
    int A_loc_col[n];    
    int B_loc_col[k];
    int C_loc_col[n];

    int A[n][k];
    int B[k][m];
    int C[n][m];
    int AT[k][n];
    int BT[m][k];
    int CT[m][n];

    int A_loc[n][k];
    int AT_loc[n][k];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    if (th == 0){
        /* Read A and B */
		FILE *fp;
		fp =fopen( "A.dat", "r" );
		if (!fp) MPI_Abort( MPI_COMM_WORLD, 1 );
		for (int i=0; i<n; i++) {
			for (int j=0; j<k; j++) {
				fscanf( fp, "%d", &A[i][j] );
			}
			fscanf( fp, "\n" );
		}

		fp =fopen( "B.dat", "r" );
		if (!fp) MPI_Abort( MPI_COMM_WORLD, 1 );
		for (int i=0; i<k; i++) {
			for (int j=0; j<m; j++) {
				fscanf( fp, "%d", &B[i][j] );
			}
			fscanf( fp, "\n" );
		}

        /* Transpose */
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < n; j++)
            {
                AT[i][j] = A[j][i];
            }
            
        }
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < k; j++)
            {
                BT[i][j] = B[j][i];
            }
            
        }
        

    }
    
    MPI_Scatter(AT, n, MPI_INTEGER, A_loc_col, n, MPI_INTEGER, 0, MPI_COMM_WORLD);
    MPI_Scatter(BT, k, MPI_INTEGER, B_loc_col, k, MPI_INTEGER, 0, MPI_COMM_WORLD);

    /* Calculate C_loc_col */
    
    MPI_Allgather(A_loc_col, n, MPI_INTEGER, AT_loc, n, MPI_INTEGER, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            A_loc[i][j] = AT_loc[j][i];
        }
        
    }

    for (int i = 0; i < n; i++)
    {
        C_loc_col[i] = 0;
        for (int j = 0; j < k; j++)
        {
            C_loc_col[i] = C_loc_col[i] + A_loc[i][j] * B_loc_col[j];
        }
        
    }

    MPI_Gather(C_loc_col, n, MPI_INTEGER, CT, n, MPI_INTEGER, 0, MPI_COMM_WORLD);

    if (th == 0)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                C[i][j] = CT[j][i];
            }
            
        }
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                printf("%d, ", C[i][j]);
            }
            printf("\n");
        }
        
    }
    MPI_Finalize();
}