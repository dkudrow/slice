/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * test/main.c
 *
 * Test harness
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 26 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <stdio.h>

extern const char *test_name;
const char *run_test();

int main() {
	const char *result = run_test();

	printf("[%s]\t", test_name);

	if (result != NULL) {
		printf("failed test '%s'\n", result);
		return 1;
	}

	printf("passed all tests\n");
	return 0;
}
