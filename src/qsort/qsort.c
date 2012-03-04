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
#include "comp428a2.h"

int main()
{
  int size;
  int *values;
  
  write(1, "Loading values... ", 18);
  values = loadFromFile(INPUTFILE, &size);
  write(1, "done.\n", 6);

  if (values == NULL)
  {
    fprintf(stderr, "Could not load input file\n");
    return EXIT_FAILURE;
  }
 
  write(1, "Sorting values... ", 18);
  qsort(values, size, sizeof(int), compare);
  write(1, "done.\n", 6);

  write(1, "Writing to file... ", 19);
  writeToFile(OUTPUTFILE, values, size);
  write(1, "done.\n", 6);

  return EXIT_SUCCESS;
}
