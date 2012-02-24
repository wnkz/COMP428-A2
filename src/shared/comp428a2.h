//
//  comp428a2.h
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-23.
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

#ifndef __COMP428_A2_H__
#define __COMP428_A2_H__

#define INPUTFILE   "./input.txt"
#define OUTPUTFILE  "./output.txt"

int compare(const void *a, const void *b);
int *loadFromFile(const char *filename, int *nSize);
int writeToFile(const char *filename, int *values, int nSize);

#endif