/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/malloc.h
 *
 * Simple malloc implementation
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 25 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include "../util/list.h"

/*
 * This macro determines the (de)allocation algorithm used. Choices are:
 *	- First fit (get_first_free_seg)
 */
#define get_free_seg(size) get_first_free_seg(size)

struct malloc_t {
	int free;		/* 0 if the segment is in use */
	size_t size;		/* size of segment */
	struct list_t list;	/* list of all malloc segment */
	struct list_t f_list;	/* list of free/used malloc segment */
};

void malloc_init(void *start, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void malloc_dump(void *start, size_t size);

#endif /* MALLOC_H */
