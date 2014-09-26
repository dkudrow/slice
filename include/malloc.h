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

#include <stddef.h> /* need this for size_t */
#include "../util/list.h"

/*
 * This macro determines the allocation algorithm used. Choices are:
 *	- get_first_fit_seg 
 *	- get_best_fit_seg
 */
#define get_free_seg(size) get_best_fit_seg(size)

struct malloc_t {
	int free;			/* 0 if the segment is in use */
	size_t size;			/* size of segment */
	struct list_t heap_list;	/* list of all malloc segment */
	struct list_t free_list;	/* list of free/used malloc segment */
};

void malloc_init(void *start, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void malloc_dump(void *start, size_t size);

#endif /* MALLOC_H */
