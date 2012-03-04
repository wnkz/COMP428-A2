//
//  qsort.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-23.
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "comp428a2.h"

int main()
{
  size_t size;
  int *values;
  clock_t start, end;

  // STEP indications
  write(1, "Loading values... ", 18);
  start = clock();
  values = loadFromFile(INPUTFILE, &size);
  // STEP indications
  end = clock();
  printf("done. (%.3fs)\n", ((double)(end - start)) / CLOCKS_PER_SEC);

  if (values == NULL)
  {
    fprintf(stderr, "Could not load input file\n");
    return EXIT_FAILURE;
  }

  // STEP indications
  write(1, "Sorting values... ", 18);
  start = clock();
  qsort(values, size, sizeof(int), compare);
  // STEP indications
  end = clock();
  printf("done. (%.3fs)\n", ((double)(end - start)) / CLOCKS_PER_SEC);

  // STEP indications
  write(1, "Writing to file... ", 19);
  start = clock();
  writeToFile(OUTPUTFILE, values, size);
  // STEP indications
  end = clock();
  printf("done. (%.3fs)\n", ((double)(end - start)) / CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
}
