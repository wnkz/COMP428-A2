//
//  qsort.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-23.
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "comp428a2.h"

int	main()
{
	int	size;
 	int	*values = loadFromFile(INPUTFILE, &size);
	
	if (values == NULL)
	{
		fprintf(stderr, "Could not load input file\n");
		return EXIT_FAILURE;
	}
	
	qsort(values, size, sizeof(int), compare);

	writeToFile(OUTPUTFILE, values, size);
  return EXIT_SUCCESS;
}
