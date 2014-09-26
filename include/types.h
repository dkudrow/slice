/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/types.h
 *
 * C types
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 25 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef TYPES_H
#define TYPES_H

/*
 * if we are building a test we want the local type headers
 */
#ifdef TEST

#include <stddef.h>

#else

typedef unsigned size_t;

#define NULL ((void *)0)

#define offsetof(type, field) \
	((size_t) &((type *)0)->field)

#endif /* TEST */

#define container_of(ptr, type, field) \
	((type *)((char *)(ptr)-offsetof(type, field)))

#endif /* TYPES_H */
