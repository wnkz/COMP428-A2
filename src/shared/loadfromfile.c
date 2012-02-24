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

int			*loadFromFile(const char *filename, int *nSize)
{
	FILE	*f;
	long	fSize;
	char 	*buffer, *numbers;
	int* 	values;
	
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
	if ((buffer = malloc(fSize * sizeof(char))) == NULL)
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
	for (int i = 0; buffer[i]; i++)
		if (buffer[i] == ',')
			(*nSize)++;

	if ((values = malloc((*nSize) * sizeof(int))) == NULL)
	{
		fprintf(stderr, "Cannot allocate memory\n");
		return NULL;
	}

	numbers = strtok(buffer, ",");
	for (int i = 0; numbers != NULL; i++)
	{
		values[i] = atoi(numbers);
		numbers = strtok(NULL, ",");
	}
	
	fclose(f);
	free(buffer);
	return values;
}
