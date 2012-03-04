//
//  writetofile.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-23.
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

#include <stdio.h>
#include "comp428a2.h"

int writeToFile(const char *filename, int *values, size_t nSize)
{
  FILE *f;

  if ((f = fopen(filename, "w")) == NULL)
  {
    fprintf(stderr, "Cannot open file: %s for writing\n", filename);
    return -1;
  }

  size_t i;
  for (i = 0; i < (nSize - 1); i++)
    fprintf(f, "%d,", values[i]);
  fprintf(f, "%d\n", values[i]);

  fclose(f);
  return 0;
}
