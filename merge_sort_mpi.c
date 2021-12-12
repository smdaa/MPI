#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int compare(const void * first, const void * second) {
    int firstInt = * (const int *) first;
    int secondInt = * (const int *) second;
    return firstInt - secondInt;
}

int* merge(int half1[], int half2[], int mergeResult[], int size){
    int ai, bi, ci;
    ai = bi = ci = 0;
    // integers remain in both arrays to compare
    while ((ai < size) && (bi < size)){
        if (half1[ai] <= half2[bi]){
			mergeResult[ci] = half1[ai];
			ai++;
		} else {
			mergeResult[ci] = half2[bi];
			bi++;
		}
			ci++;
	}
	if (ai >= size){
        while (bi < size) {
			mergeResult[ci] = half2[bi];
			bi++; ci++;
		}
	}
	if (bi >= size){
		while (ai < size) {
			mergeResult[ci] = half1[ai];
			ai++; ci++;
		}
	}
	return mergeResult;
}

int* mergeSort(int height, int id, int localArray[], int size, MPI_Comm comm, int globalArray[]){
    int parent, rightChild, myHeight;
    int *half1, *half2, *mergeResult;
    int nth;

    myHeight = 0;
    qsort(localArray, size, sizeof(int), compare);
    half1 = localArray;
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    while (myHeight < height) {
        parent = (id & (~(1 << myHeight)));
        if (parent == id) {
            rightChild = (id | (1 << myHeight));
            half2 = (int*) malloc (size * sizeof(int));
            MPI_Recv(half2, size, MPI_INT, rightChild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            mergeResult = (int*) malloc (size * 2 * sizeof(int));
            mergeResult = merge(half1, half2, mergeResult, size);
            half1 = mergeResult;
            size = size * 2;
            free(half2);
            mergeResult = NULL;
            myHeight++;
        } 
        else
        {
            MPI_Send(half1, size, MPI_INT, parent, 0, MPI_COMM_WORLD);
            if(myHeight != 0) free(half1);
            myHeight = height;
        }
    }
    if(id == 0){
        globalArray = half1;
    }
    return globalArray;
}

int main(int argc, char *argv[]) {
    int th, nth;
    int n, nlocal;
    //int x[n];
    int* x;
    //int xlocal[nlocal];
    int* xlocal;
    int height;
    int sendcounts[nth];
    int displs[nth];
    double startTime, localTime, totalTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);
    /* Read array */
    if (th == 0)
    {
        FILE *fp;
		fp =fopen( "x.dat", "r" );
		if (!fp) MPI_Abort(MPI_COMM_WORLD, 1);
        fscanf(fp, "%d", &n);
        fscanf(fp, "\n");
        x = (int *)malloc(sizeof(int)*n);
        for (int i=0; i<n; i++)
        {
            fscanf(fp, "%d", &x[i]);
        }
    }
    MPI_Bcast(&n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
    /* Calculate total height of tree */
    height = log2(nth);
    /* In case of odd cases */
    if (th == nth - 1)
    {
        nlocal = n - (nth - 1)*(n/nth);
    } else
    {
        nlocal = n/nth;
    }
    xlocal = (int *)malloc(sizeof(int)*nlocal);
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

    
    startTime = MPI_Wtime();
    if (th == 0)
    {
        x = mergeSort(height, th, xlocal, nlocal, MPI_COMM_WORLD, x);
    }
    else
    {
        mergeSort(height, th, xlocal, nlocal, MPI_COMM_WORLD, NULL);
    }
    localTime = MPI_Wtime() - startTime;
    MPI_Reduce(&localTime, &totalTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (th == 0)
    {
        for (int i = 0; i < n; i++)
        {
            printf("%d ", x[i]);
        }
        printf("\n");
        printf("total time %f\n", totalTime);
        
    }
    MPI_Finalize();
}