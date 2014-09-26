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

#define TEST_PRE(name) \
	printf("%s ... ", name);

#define TEST_POST(result) \
	printf("[%s]\n", (result) ? "PASS" : "FAIL");

#endif /* TEST_H */
