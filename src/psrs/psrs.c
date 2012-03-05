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
#include <strings.h>
#include <math.h>
#include <mpi.h>
#include "comp428a2.h"
#include "psrs.h"

int main(int argc, const char *argv[])
{
  size_t size, s;
  int *values;
  int id, p;
  double totaltime;

  MPI_Init(&argc, (char***)&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  size = 0;
  s = 0;

  if (id == ROOT)
  {
    // STEP indications
    write(1, "Loading values... ", 18);
    double t = MPI_Wtime();
    values = loadFromFile(INPUTFILE, &size);
    // STEP indications
    printf("done. (%.3fs)\n", MPI_Wtime() - t);

    // STEP indications
    write(1, "Sorting values... ", 18);
    totaltime = MPI_Wtime();

    if (values == NULL)
    {
      fprintf(stderr, "Could not load input file\n");
      return EXIT_FAILURE;
    }

    s = (size_t)(size / p);

    // STEP indications
    // printf("[P%d] START\n", id);
  }

  // Broadcast size of array that each slave has to sort
  MPI_Bcast(&size, 1, MPI_UNSIGNED, ROOT, MPI_COMM_WORLD);
  MPI_Bcast(&s, 1, MPI_UNSIGNED, ROOT, MPI_COMM_WORLD);

  int *chunk = malloc(s * sizeof(int));

  // Scatter the values from array to the slaves
  MPI_Scatter(values, s, MPI_INT, chunk, s, MPI_INT, ROOT, MPI_COMM_WORLD);

  // STEP indications
  // printf("[P%d] STEP1 > START qsort\n", id);

  // Each process sorts its own sub-list of [N/P]
  qsort(chunk, s, sizeof(int), compare);

  // STEP indications
  // printf("[P%d] COMPLETED STEP1\n", id);

  //Each process selects P items from its local sorted sub-list
  size_t psqr;
  int *regularsamples, *gatheredsamples;

  regularsamples = malloc(p * sizeof(int));
  psqr = (size_t)(pow(p, 2));

  for (size_t i = 0; i < (size_t)p; i++)
    regularsamples[i] = chunk[(size_t)((size * i) / psqr)];

  if (id == ROOT)
    gatheredsamples = malloc(psqr * sizeof(int));

  // Process P0 collects the regular samples from the P processes (which includes itself)
  MPI_Gather(regularsamples, p, MPI_INT, gatheredsamples, p, MPI_INT, ROOT, MPI_COMM_WORLD);

  // STEP indications
  // printf("[P%d] COMPLETED STEP2\n", id);

  // * free memory
  free(regularsamples);

  int *pivots = malloc((p - 1) * sizeof(int));

  if (id == ROOT)
  {
    // Process P0 sorts the regular samples using quick sort
    qsort(gatheredsamples, psqr, sizeof(int), compare);

    // Process P0 chooses (P - 1) pivot values
    for (size_t i = 1; i < (size_t)p; i++)
      pivots[i - 1] = gatheredsamples[((p * i) + (p / 2) - 1)];

    // STEP indications
    // printf("[P%d] COMPLETED STEP3\n", id);

    // * free memory
    free(gatheredsamples);
  }

  // Broadcast the pivots to the P processes
  MPI_Bcast(pivots, (p - 1), MPI_INT, ROOT, MPI_COMM_WORLD);

  // Each process partitions its local sorted sub-list into P partitions,
  // with the P - 1 pivots as separators

  // STEP indications
  // printf("[P%d] STEP4 > sending partitions to other processes\n", id);

  size_t mypsize;
  int **partitions = malloc(p * sizeof(int*));
  MPI_Request *send_requests = malloc(p * sizeof(MPI_Request));

  for (size_t i = 0, z = 0, partitioned = 0; i <= s; i++)
  {
    if ((z >= ((size_t)p - 1)) || (pivots[z] < chunk[i]))
    {
      // If last partition, get to the end of chunk
      if (z >= ((size_t)p - 1))
        for (; i < s; i++);

      size_t psize = (i - partitioned);
      partitions[z] = malloc(psize * sizeof(int));
      memcpy(partitions[z], &(chunk[i - psize]), (psize * sizeof(int)));

      if (z != (size_t)id)
      {
        MPI_Isend(&psize, 1, MPI_UNSIGNED, z, PSIZETAG, MPI_COMM_WORLD, &(send_requests[z]));
        MPI_Isend(partitions[z], psize, MPI_INT, z, PARTITIONTAG, MPI_COMM_WORLD, &(send_requests[z]));
      }
      else
        mypsize = psize;

      z++;
      partitioned += psize;
    }
  }

  // STEP indications
  // printf("[P%d] STEP4 > all partitions sent\n", id);
  // printf("[P%d] COMPLETED STEP4\n", id);

  // * free memory
  free(chunk);
  free(pivots);
  free(send_requests);

  // Merge partitions from other processes
  size_t *rpsize;
  int **rpartitions;
  MPI_Request *recv_psize_requests, *recv_partition_requests;

  rpsize = malloc(p * sizeof(size_t));
  bzero(rpsize, p * sizeof(size_t));

  // Make all allocations needed
  rpartitions = malloc((p - 1) * sizeof(int*));
  for (size_t i = 0; i < (size_t)(p - 1); i++)
    rpartitions[i] = malloc((size) * sizeof(int));
  recv_psize_requests = malloc((p - 1) * sizeof(MPI_Request));
  recv_partition_requests = malloc((p - 1) * sizeof(MPI_Request));

  for (size_t i = 0, idx = 0; i < (size_t)p; i++)
  {
    if (i != (size_t)id)
    {
      MPI_Irecv(&(rpsize[idx]), 1, MPI_UNSIGNED, i, PSIZETAG, MPI_COMM_WORLD, &(recv_psize_requests[idx]));
      MPI_Irecv(rpartitions[idx], s, MPI_INT, i, PARTITIONTAG, MPI_COMM_WORLD, &(recv_partition_requests[idx]));
      idx++;
    }
  }

  int *ripartition = malloc((s * 2) * sizeof(int));

  // Copy own sub-list to intermediate partition
  memcpy(ripartition, partitions[id], (mypsize * sizeof(int)));

  // Wait to receive from other processes
  MPI_Waitall((p - 1), recv_psize_requests, MPI_STATUSES_IGNORE);
  // STEP indications
  // printf("[P%d] STEP5 > All partitions' sizes received\n", id);
  MPI_Waitall((p - 1), recv_partition_requests, MPI_STATUSES_IGNORE);
  // STEP indications
  // printf("[P%d] STEP5 > All partitions received\n", id);

  // Copy received values to intermediate partition
  size_t rpartsize = mypsize;
  for (size_t i = 0; i < ((size_t)p - 1); i++)
  {
    memcpy(&(ripartition[rpartsize]), rpartitions[i], (rpsize[i] * sizeof(int)));
    rpartsize += rpsize[i];
  }

  // STEP indications
  // printf("[P%d] STEP5 > Done copying received partitions\n", id);

  // STEP indications
  // printf("[P%d] STEP5 > Start sorting intermediate partition\n", id);

  qsort(ripartition, rpartsize, sizeof(int), compare);

  // STEP indications
  // printf("[P%d] STEP5 > Done sorting intermediate partition\n", id);

  // Each process sends its partition to P0
  if (id != ROOT) {
    MPI_Request psize_request, partition_request;

    MPI_Isend(&rpartsize, 1, MPI_UNSIGNED, ROOT, PSIZETAG, MPI_COMM_WORLD, &psize_request);
    MPI_Isend(ripartition, rpartsize, MPI_INT, ROOT, PARTITIONTAG, MPI_COMM_WORLD, &partition_request);

    MPI_Wait(&psize_request, MPI_STATUS_IGNORE);
    MPI_Wait(&partition_request, MPI_STATUS_IGNORE);
  }

  // P0 merge all received partitions
  // TODO: Try MPI_Irecv
  if (id == ROOT)
  {
    int *fvalues;
    MPI_Request *fpsize_requests, *fpartition_requests;

    fpsize_requests = malloc((p - 1) * sizeof(MPI_Request));
    fpartition_requests = malloc((p - 1) * sizeof(MPI_Request));

    for (size_t i = 0, idx = 0; i < (size_t)p; i++)
    {
      if (i != (size_t)id)
      {
        MPI_Irecv(&(rpsize[idx]), 1, MPI_UNSIGNED, i, PSIZETAG, MPI_COMM_WORLD, &(fpsize_requests[idx]));
        MPI_Irecv(rpartitions[idx], size, MPI_INT, i, PARTITIONTAG, MPI_COMM_WORLD, &(fpartition_requests[idx]));
        idx++;
      }
    }

    fvalues = malloc(size * sizeof(int));

    // Wait to receive from other processes
    MPI_Waitall((p - 1), fpsize_requests, MPI_STATUSES_IGNORE);
    MPI_Waitall((p - 1), fpartition_requests, MPI_STATUSES_IGNORE);

    memcpy(fvalues, ripartition, (rpartsize * sizeof(int)));

    // Copy received values to intermediate partition
    for (size_t i = 0, idx = rpartsize; i < ((size_t)p - 1); i++)
    {
      memcpy(&(fvalues[idx]), rpartitions[i], (rpsize[i] * sizeof(int)));
      idx += rpsize[i];
    }

    // STEP indications
    // printf("[P%d] COMPLETED STEP5\n", id);
    printf("done. (%.3fs)\n", MPI_Wtime() - totaltime);

    // STEP indications
    write(1, "Writing to file... ", 19);
    double t = MPI_Wtime();
    writeToFile(OUTPUTFILE, fvalues, size);
    // STEP indications
    printf("done. (%.3fs)\n", MPI_Wtime() - t);

    // * free memory
    free(fvalues);
    free(values);
  }

  // * free memory
  free(ripartition);
  for (size_t i = 0; i < (size_t)p; i++)
    free(partitions[i]);
  free(partitions);
  for (size_t i = 0; i < (size_t)(p - 1); i++)
    free(rpartitions[i]);
  free(rpartitions);
  free(rpsize);
  free(recv_psize_requests);
  free(recv_partition_requests);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
