//
//  pqsort.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-22
//  (c) Copyright 2012 Grégoire Morpain. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <mpi.h>
#include "comp428a2.h"
#include "pqsort.h"

int main(int argc, const char *argv[])
{
  int id, p;
  
  id = 0;
  p = 0;
  
  MPI_Init(&argc, (char***)&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  // int MPI_Cart_create(MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart)
  int ndims = 4;
  int dims[4] = {2, 2, 2, 2};
  int periods[4] = {1, 1, 1, 1};
  int reorder = 1;
  MPI_Comm comm_cart;
    
  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &comm_cart);

  MPI_Finalize();

  return EXIT_SUCCESS;
}
