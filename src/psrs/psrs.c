//
//  psrs.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-22
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include "comp428a2.h"
#include "psrs.h"

int main(int argc, const char *argv[])
{
  int size, s, psqr;
  int *values;
  int *fvalues, *chunk, *rsamples, *gsamples, *pivots, *rpartitions;
  int **partitions;
  int id, p, mypsize, rpartsize;

  MPI_Init(&argc, (char***)&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  if (id == ROOT)
  {
    write(1, "Loading values... ", 18);
    values = loadFromFile(INPUTFILE, &size);
    write(1, "done.\n", 6);

    if (values == NULL)
    {
      fprintf(stderr, "Could not load input file\n");
      return EXIT_FAILURE;
    }

    s = (int)(size / p);

    printf("[P%d] START\n", id);
  }

  // Broadcast size of array that each slave has to sort
  MPI_Bcast(&s, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  chunk = malloc(s * sizeof(int));

  // Scatter the values from array to the slaves
  MPI_Scatter(values, s, MPI_INT, chunk, s, MPI_INT, ROOT, MPI_COMM_WORLD);

  printf("[P%d] STEP1 > START qsort\n", id);

  // Each process sorts its own sub-list of [N/P]
  qsort(chunk, s, sizeof(int), compare);

  printf("[P%d] COMPLETED STEP1\n", id);

  //Each process selects P items from its local sorted sub-list
  rsamples = malloc(p * sizeof(int));
  psqr = (int)(pow(p, 2));
  for (int i = 0; i < p; i++)
    rsamples[i] = chunk[(int)((size * i) / psqr)];

  if (id == ROOT)
    gsamples = malloc(psqr * sizeof(int));

  // Process P0 collects the regular samples from the P processes (which includes itself)
  MPI_Gather(rsamples, p, MPI_INT, gsamples, p, MPI_INT, ROOT, MPI_COMM_WORLD);

  printf("[P%d] COMPLETED STEP2\n", id);

  // * free memory
  free(rsamples);

  pivots = malloc((p - 1) * sizeof(int));
  if (id == ROOT)
  {
    // Process P0 sorts the regular samples using quick sort
    qsort(gsamples, psqr, sizeof(int), compare);
  
    // Process P0 chooses (P - 1) pivot values
    for (int i = 1; i < p; i++)
      pivots[i - 1] = gsamples[((p * i) + (p / 2) - 1)];

    printf("[P%d] COMPLETED STEP3\n", id);
    
    // * free memory
    free(gsamples);
  }
  
  // Broadcast the pivots to the P processes
  MPI_Bcast(pivots, (p - 1), MPI_INT, ROOT, MPI_COMM_WORLD);


  // Each process partitions its local sorted sub-list into P partitions,
  // with the P - 1 pivots as separators
  // TODO: Try MPI_Isend
  // int MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
  MPI_Request send_requests[p];

  printf("[P%d] STEP4 > START sending partitions to other processes.\n", id);
 
  partitions = malloc(p * sizeof(int*));
  for (int i = 0, z = 0, partitioned = 0; i <= s; i++)
  {
    if ((z >= (p - 1)) || (pivots[z] < chunk[i]))
    { 
      // If last partition, get to the end of chunk
      if (z >= (p - 1))
        for (; i < s; i++);
      
      int psize = (i - partitioned);
      partitions[z] = malloc(((psize + 1) * sizeof(int)));
      memcpy(partitions[z], &(chunk[i - psize]), (psize * sizeof(int)));
      
      if (z != id)
      {
        MPI_Status status;
        // MPI_Send(partitions[z], psize, MPI_INT, z, 0, MPI_COMM_WORLD);
        MPI_Isend(partitions[z], psize, MPI_INT, z, 0, MPI_COMM_WORLD, &(send_requests[z]));
        // printf("\t[P%d] Waiting for partition to be sent...\n", id);
        // MPI_Wait(&(send_requests[z]), &status);
        // printf("\t[P%d] Partition sent\n", id);        
      }
      else
        mypsize = psize;

      z++;
      partitioned += psize;
    }
  }

  printf("[P%d] COMPLETED STEP4\n", id);
  
  // * free memory
  free(chunk);
  free(pivots);

  // Merge partitions from other processes
  // TODO: Try MPI_Irecv
  // int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
  MPI_Request recv_requests[p];
  
  rpartitions = malloc((s * 2) * sizeof(int));
  rpartsize = 0;
  for (int i = 0; i < p; i++)
  {
    if (i != id)
    {
      MPI_Status status;
      // MPI_Recv(&(rpartitions[rpartsize]), s, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      MPI_Irecv(&(rpartitions[rpartsize]), s, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &(recv_requests[i]));
      MPI_Wait(&recv_requests[i], &status);
      rpartsize += status._count / sizeof(int);
    }
    else
    {
      memmove(&(rpartitions[rpartsize]), partitions[id], (mypsize * sizeof(int)));
      rpartsize += mypsize;
    }
  }

  printf("[P%d] STEP5 > DONE RECV\n", id);
  
  // * free memory
  for (int i = 0; i < p; i++)
    free(partitions[i]);
  free(partitions);
  
  qsort(rpartitions, rpartsize, sizeof(int), compare);

  // Each process sends its partition to P0
  // TODO: Try MPI_Isend
  // int MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
  if (id != ROOT) {
    // MPI_Request request;
    MPI_Send(rpartitions, rpartsize, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
    // MPI_Isend(rpartitions, rpartsize, MPI_INT, ROOT, 0, MPI_COMM_WORLD, &request);
  }
  
  // P0 merge all received partitions
  // TODO: Try MPI_Irecv
  if (id == ROOT)
  {
    fvalues = malloc(size * sizeof(int));    
    memcpy(fvalues, rpartitions, (rpartsize * sizeof(int)));
    
    for (int i = 1, idx = rpartsize; i < p; i++)
    {
      MPI_Status status;
      MPI_Recv(&(fvalues[idx]), size, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      idx += status._count / sizeof(int);
    }

    printf("[P%d] COMPLETED STEP5\n", id);

    write(1, "Writing to file... ", 19);
    writeToFile(OUTPUTFILE, fvalues, size);
    write(1, "done.\n", 6);
  
    // * free memory
    free(fvalues);
    free(values);
  }

  // * free memory
  free(rpartitions);
  
  MPI_Finalize();
  return EXIT_SUCCESS;
}
