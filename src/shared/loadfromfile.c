//
//  loadfromfile.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-23.
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comp428a2.h"

int *loadFromFile(const char *filename, size_t *nSize)
{
  FILE *f;
  unsigned long fSize;
  char *buffer, *numbers;
  int* values;

  if ((f = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "Cannot open file: %s for reading\n", filename);
    return NULL;
  }

  // Get file size
  fseek(f, 0, SEEK_END);
  fSize = ftell(f);
  rewind(f);

  // Allocate memory for the file
  if ((buffer = malloc(fSize * sizeof(char) + 1)) == NULL)
  {
    fprintf(stderr, "Cannot allocate memory\n");
    return NULL;
  }

  // Copy the file into the buffer
  if ((size_t)fSize != fread(buffer, 1, fSize, f))
  {
    fprintf(stderr, "fread() error\n");
    return NULL;
  }

  // Get the number of values for allocation
  (*nSize) = 1;
  for (size_t i = 0; i < fSize; i++)
    if (buffer[i] == ',')
      (*nSize)++;

  if ((values = malloc((*nSize) * sizeof(int))) == NULL)
  {
    fprintf(stderr, "Cannot allocate memory\n");
    return NULL;
  }

  const char *delim = ",";
  numbers = strtok(buffer, delim);
  for (size_t i = 0; numbers != NULL; i++)
  {
    values[i] = atoi(numbers);
    numbers = strtok(NULL, delim);
  }

  fclose(f);
  free(buffer);
  return values;
}
