#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) 
{
    int th, nth;
    double time;
    int buffer_count = 1000;
    int buffer[buffer_count];
    MPI_Request request;
    int request_complete = 0;
    MPI_Status  status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &th);
    MPI_Comm_size(MPI_COMM_WORLD, &nth);

    for (int i = 0; i < buffer_count; i++)
    {
        if (th ==0)
        {
            buffer[i] = 2*i;
        } 
        else
        {
            buffer[i] = 0;
        }
        
    }
    MPI_Barrier(MPI_COMM_WORLD);

    double time = MPI_Wtime();
    if (th == 0)
    {
        sleep(3);
        // Initialise the non-blocking send to process 1
        MPI_Isend(buffer, buffer_count, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, &request);
        double time_left = 6000.0;
        while (time_left > 0.0) 
        {
            usleep(1000);
            // Test if the request is finished (only if not already finished)
            if (!request_complete)
            {
                MPI_Test(&request, &request_complete, &status);
            }
            time_left -= 1000.0;
        }
        // If the request is not yet complete, wait here. 
        if (!request_complete)
        {
            MPI_Wait(&request, &status);
        }

        for (int i=0; i < buffer_count; ++i)
        {
            buffer[i] = -i;
        }
        time_left = 3000.0;
        while (time_left > 0.0)
        {
            usleep(1000);

        }
    
    
    
    }
    
    


    MPI_Finalize();
}