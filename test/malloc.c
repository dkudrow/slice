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

#include <malloc.h>

#define MSIZE (sizeof(struct malloc_t))
#define HSIZE ((MSIZE+64) * 10)

const char *test_name = "MALLOC";

static char HEAP[HSIZE];

struct malloc_info_t {
	int total_seg;
	int free_seg;
	int used_seg;
	size_t total_mem;
	size_t free_mem;
	size_t used_mem;
};

#define malloc_info_eq(expect, result) (\
		expect.total_seg == result.total_seg && \
		expect.free_seg == result.free_seg && \
		expect.used_seg == result.used_seg && \
		expect.total_mem == result.total_mem && \
		expect.free_mem == result.free_mem && \
		expect.used_mem == result.used_mem)

static struct malloc_info_t malloc_info_init(int ts, int fs, int us, size_t tm, size_t fm, size_t um)
{
	struct malloc_info_t info;
	info.total_seg = ts;
	info.free_seg = fs;
	info.used_seg = us;
	info.total_mem = tm;
	info.free_mem = fm;
	info.used_mem = um;
	return info;
}

static struct malloc_info_t malloc_info()
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

char *run_test()
{
	int i;
	char *ptr[100];
	struct malloc_info_t info, expect;

	/* initializing heap */
	malloc_init(HEAP, HSIZE);
	expect = malloc_info_init(1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "initializing heap";

	/* freeing an unused segment */
	free(HEAP+MSIZE);
	expect = malloc_info_init(1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "freeing an unused segment";

	/* freeing an invalid pointer */
	free(HEAP+HSIZE/2);
	expect = malloc_info_init(1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "freeing an invalid pointer";

	/* allocating the first segment */
	ptr[0] = malloc(64);
	expect = malloc_info_init(2, 1, 1, HSIZE, HSIZE-64-2*MSIZE, 64);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "allocating the first segment";

	/* freeing the first segment */
	free(ptr[0]);
	expect = malloc_info_init(1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "freeing the first segment";

	/* allocating multiple segments */
	for (i=0; i<4; i++)
		ptr[i] = malloc(64);
	expect = malloc_info_init(5, 1, 4, HSIZE, HSIZE-4*64-5*MSIZE, 4*64);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "allocating multiple segments";

	/* merging freed segment with previous */
	malloc_init(HEAP, HSIZE);
	for (i=0; i<3; i++)
		ptr[i] = malloc(64);
	free(ptr[0]);
	free(ptr[1]);
	expect = malloc_info_init(3, 2, 1, HSIZE, HSIZE-64-3*MSIZE, 64);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "merging freed segment with previous";

	/* freeing multiple segments */
	malloc_init(HEAP, HSIZE);
	for (i=0; i<4; i++)
		ptr[i] = malloc(64);
	for (i=0; i<4; i++)
		free(ptr[i]);
	expect = malloc_info_init(1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "freeing multiple segments";

	/* filling all memory */
	for (i=0; i<10; i++)
		ptr[i] = malloc(64);
	expect = malloc_info_init(10, 0, 10, HSIZE, 0, 64*10);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "filling all memory";

	/* allocating from full heap */
	ptr[10] = (char *)0xDEADBEEF;
	ptr[10] = malloc(64);
	if (ptr[10] != NULL)
		return "allocating from full heap";

	/* freeing all memory */
	for (i=0; i<10; i++)
		free(ptr[i]);
	expect = malloc_info_init(1, 1, 0, HSIZE, HSIZE-MSIZE, 0);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "freeing all memory";

	/* allocating w/out memory for new header */
	ptr[0] = malloc(10);
	ptr[1] = malloc(HSIZE-2*MSIZE-10);
	free(ptr[0]);
	ptr[0] = malloc(5); /* will actually get 10 bytes */
	expect = malloc_info_init(2, 0, 2, HSIZE, 0, HSIZE-2*MSIZE);
	info = malloc_info();
	if (!malloc_info_eq(expect, info))
		return "allocating w/out memory for new header";

	return NULL;
}
