//
//  pqsort.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-22
//  (c) Copyright 2012 Grégoire Morpain. All rights reserved.
//

#include <stdlib.h>
#include <mpi.h>
#include "comp428a2.h"
#include "pqsort.h"

int main(int argc, const char *argv[])
{
  int id, p;
  
  MPI_Init(&argc, (char***)&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  int ndims = 3;
  int dims[3] = {1, 1, 1};
  int periods[3] = {1, 1, 1};
  int reorder = 1;
  MPI_Comm comm_cart;

  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &comm_cart);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
