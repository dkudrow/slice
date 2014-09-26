/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * test/test.h
 *
 * Testing helpers
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 26 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static inline void test_run(char *name, char *result)
{
	printf("[%s]\t", name);
	if (result == NULL)
		printf("passed all tests\n");
	else
		printf("failed test '%s'\n", result);
}

#endif /* TEST_H */
