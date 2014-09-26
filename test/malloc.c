/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * test/malloc.c
 *
 * Tests for malloc
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
#include <malloc.h>
#include "test.h"

#define HSIZE 1024

char HEAP[HSIZE];

struct malloc_info_t {
	int total_seg;
	int free_seg;
	int used_seg;
	size_t total_mem;
	size_t free_mem;
	size_t used_mem;
};

#define MSIZE (sizeof(struct malloc_t))

#define malloc_info_set(info, ts, fs, us, tm, fm, um) \
{ \
	info.total_seg = ts; \
	info.free_seg = fs; \
	info.used_seg = us; \
	info.total_mem = tm; \
	info.free_mem = fm; \
	info.used_mem = um; \
}

#define malloc_info_eq(expect, result) \
	(\
	 expect.total_seg == result.total_seg && \
	 expect.free_seg== result.free_seg && \
	 expect.used_seg== result.used_seg && \
	 expect.total_mem == result.total_mem && \
	 expect.free_mem == result.free_mem && \
	 expect.used_mem == result.used_mem \
	)

struct malloc_info_t malloc_info()
{
	struct malloc_info_t info = { 0, 0, 0, 0, 0, 0 };
	struct list_t *iter, *heap_list=((struct malloc_t *)HEAP)->heap_list.prev;
	list_foreach(heap_list, iter) {
		struct malloc_t *cur_malloc = container_of(iter, struct malloc_t, heap_list);
		++info.total_seg;
		if (cur_malloc->free == 0) {
			++info.used_seg;
			info.used_mem += cur_malloc->size;
		} else {
			++info.free_seg;
			info.free_mem += cur_malloc->size;
		}
	}
	info.total_mem  = info.used_mem + info.free_mem + info.total_seg*MSIZE;
	return info;
}

int main(void) {
	struct malloc_info_t info, expect;

	TEST_PRE("Initializing heap");
	malloc_init(HEAP, HSIZE);
	malloc_info_set(expect, 1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	TEST_POST(malloc_info_eq(expect, info));

	return 0;
}
