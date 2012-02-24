//
//  compare.c
//  COMP428-A2
//
//  Created by Grégoire Morpain on 2012-02-23.
//  (c) Copyright 2012 Grégoire Morpain. All Rights Reserved.
//

int compare(const void *a, const void *b)
{
	return (*(int*)a - *(int*)b);
}