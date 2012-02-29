//
//  psrs.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-22
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

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
  int *values = loadFromFile(INPUTFILE, &size);
  int *chunk, *rsamples, *gsamples, *pivots, *rpartitions;
  int **partitions;
  int id, p;

  if (values == NULL)
  {
    fprintf(stderr, "Could not load input file\n");
    return EXIT_FAILURE;
  }

  MPI_Init(&argc, (char***)&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  s = (int)(size / p);

  // Broadcast size of array that each slave has to sort
  MPI_Bcast(&s, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  chunk = malloc(s * sizeof(int));

  // Scatter the values from array to the slaves
  MPI_Scatter(values, s, MPI_INT, chunk, s, MPI_INT, ROOT, MPI_COMM_WORLD);

  // <DEBUG>
  // printf("[P(%d)] Received:\n", id);
  // for (int i = 0; i < s; i++)
  //  printf("[P(%d)]%d ", id, chunk[i]);
  // printf("\n");
  // <DEBUG/>

  // Each process sorts its own sub-list of [N/P]
  qsort(chunk, s, sizeof(int), compare);

  // <DEBUG>
  // printf("[P(%d)] Sorted:\n", id);
  // for (int i = 0; i < s; i++)
  //  printf("[P(%d)]%d ", id, chunk[i]);
  // printf("\n");
  //
  // printf("[P(%d)] Done.\n", id);
  // <DEBUG/>

  //Each process selects P items from its local sorted sub-list
  rsamples = malloc(p * sizeof(int));
  psqr = (int)(pow(p, 2));
  for (int i = 0; i < p; i++)
  {
    rsamples[i] = chunk[(int)((size * i) / psqr)];
    // printf("[P(%d)] regular sample(%d): %d\n", id, (i + 1), rsamples[i]);
  }

  if (id == ROOT)
    gsamples = malloc(psqr * sizeof(int));

  // Process P0 collects the regular samples from the P processes (which includes itself)
  MPI_Gather(rsamples, p, MPI_INT, gsamples, p, MPI_INT, ROOT, MPI_COMM_WORLD);

  pivots = malloc((p - 1) * sizeof(int));
  if (id == ROOT)
  {
    // <DEBUG>
    // printf("[P(%d)] Collected regular samples:\n", id);
    // for (int i = 0; i < psqr; i++)
    //  printf("%d ", gsamples[i]);
    // printf("\n");
    // <DEBUG/>

    // Process P0 sorts the regular samples using quick sort
    qsort(gsamples, psqr, sizeof(int), compare);

    // <DEBUG>
    // printf("[P(%d)] Sorted collected regular samples:\n", id);
    // for (int i = 0; i < psqr; i++)
    //  printf("%d ", gsamples[i]);
    // printf("\n");
    // <DEBUG/>

    // Process P0 chooses (P - 1) pivot values
    for (int i = 1; i < p; i++)
    {
      pivots[i - 1] = gsamples[((p * i) + (p / 2) - 1)];
      // printf("[P(%d)] Chosen pivots(%d): %d\n", id, i, pivots[i - 1]);
    }
  }

  // Broadcast the pivots to the P processes
  MPI_Bcast(pivots, (p - 1), MPI_INT, ROOT, MPI_COMM_WORLD);

  // <DEBUG>
  // printf("[P(%d)] Received pivots:\n", id);
  // for (int i = 0; i < (p - 1); i++)
  // {
  //  printf("[P(%d)] Pivot(%d): %d\n", id, (i + 1), pivots[i]);
  // }
  // <DEBUG/>

  // Each process partitions its local sorted sub-list into P partitions,
  // with the P - 1 pivots as separators
  partitions = malloc(p * sizeof(int));
  for (int i = 0, z = 0, partitioned = 0; i <= s; i++)
  {
    if ((z >= (p - 1)) || (pivots[z] < chunk[i]))
    {
      // If last partition, get to the end of chunk
      if (z >= (p - 1))
        for (; i < s; i++);

      int psize = (i - partitioned);
      partitions[z] = malloc(psize * sizeof(int));
      memcpy(partitions[z], &(chunk[i - psize]), (psize * sizeof(int)));

      // Each process keeps i(th) partition and sends j(th) partition to process P(j)
      if (z != id)
        MPI_Send(partitions[z], psize, MPI_INT, z, 0, MPI_COMM_WORLD);

      z++;
      partitioned += psize;
    }
  }

  rpartitions = malloc(s * sizeof(int));
  for (int i = 0; i < p; i++)
  {
    if (i != id)
    {
      MPI_Status status;
      MPI_Recv(rpartitions, s, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      // printf("P%d recieving from P%d | elements = %ld\n", id, status.MPI_SOURCE, status._count / sizeof(int));
    }
  }

  MPI_Finalize();
  writeToFile(OUTPUTFILE, values, size);
  return EXIT_SUCCESS;
}