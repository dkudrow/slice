/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * test/test.c
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

struct unit_test_t {
	int no;
	char *name;
	char *(*func)(void);
};

extern char *malloc_test();
extern char *rbtree_test();

static struct unit_test_t unit_tests[] = {
	{1, "malloc", malloc_test},
	{2, "rbtree", rbtree_test},
	{0, "", NULL}
};

static inline void test_run(char *name, char *(*func)(void))
{
	char *result = func();
	printf("[%s]\t", name);
	if (result == NULL)
		printf("passed all tests\n");
	else
		printf("failed test '%s'\n", result);
}

int main() {
	int i;
	for (i=0; unit_tests[i].no != 0; i++) {
		test_run(unit_tests[i].name, unit_tests[i].func);
	}
}
