/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/malloc.c
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
 *
 * This is a very simple implementation of malloc that will allow us to
 * allocate memory dynamically in other parts of the kernel. We start with
 * a contiguous region of memory called the heap and divide it into
 * segments as programs allocate and deallocate memory from of it. Three
 * linked list are used to keep track of these segments. When the heap is
 * initialized, there is a single free segment:
 *
 *	----------------------------------------------------------
 *	|free                                                    |
 *	----------------------------------------------------------
 *
 * When a call is made to malloc, the segment is divided into a used
 * segment and a free segment:
 *
 *	----------------------------------------------------------
 *	|used       |free                                        |
 *	----------------------------------------------------------
 *
 * When a call is made to free, the segment is merged with neighboring free
 * segments:
 *
 *	----------------------------------------------------------
 *	|free                                                    |
 *	----------------------------------------------------------
 *
 * Each segment has a small header of type malloc_t that keeps track of
 * it's status (free or used) and how large it is. We use three linked
 * lists to keep track of the segments. The first is heap_list which
 * keeps a linear map of all segments on the heap. Consecutive segments in
 * heap_list are consecutive in memory. free_list and used_list keep
 * track of free and used segments respectively in the order they are
 * (de)allocated. Note that the same list node in malloc_t is used for both
 * of these lists (this is safe because a segment cannot be in both lists
 * simultaneously.)
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <malloc.h>

static struct list_t heap_list;
static struct list_t free_list;
static struct list_t used_list;

/*
 * initialize the heap
 */
void malloc_init(void *start, size_t size)
{
	struct malloc_t *seg = (struct malloc_t *)start;

	/* the heap starts out as a single free segment */
	seg->size = size - sizeof(struct malloc_t);
	seg->free = 1;

	/* initialize the linked lists with the new segment */
	list_init(&heap_list);
	list_init(&free_list);
	list_init(&used_list);
	list_insert_after(&heap_list, &seg->heap_list);
	list_insert_after(&free_list, &seg->free_list);
}

/*
 * first fit allocation
 */
static inline struct malloc_t *get_first_fit_seg(size_t size)
{
	struct malloc_t *free_seg;

	/* find the first free segment that is large enough */
	list_find_item(free_seg, &free_list, free_list, free_seg->size >= size);

	return free_seg;
}

/*
 * best fit allocation
 */
static inline struct malloc_t *get_best_fit_seg(size_t size)
{
	size_t diff, min;
	struct malloc_t *free_seg, *best_seg;

	/* find the first free segment that is large enough */
	list_find_item(best_seg, &free_list, free_list, best_seg->size >= size);
	if (best_seg == NULL)
		return NULL; /* there are no segments large enough */
	min = best_seg->size - size;

	/* compare the remaining segments to find the closest match */
	list_foreach_item(free_seg, &best_seg->free_list, free_list) {
		if (free_seg->size > size) {
			diff = free_seg->size - size;
			if (diff < min) {
				best_seg = free_seg;
				min = diff;
			}
		}
	}

	return best_seg;
}

/*
 * worst fit allocation
 */
static inline struct malloc_t *get_worst_fit_seg(size_t size)
{
	size_t diff, max;
	struct malloc_t *free_seg, *best_seg;

	/* find the first free segment that is large enough */
	list_find_item(best_seg, &free_list, free_list, best_seg->size >= size);
	if (best_seg == NULL)
		return NULL; /* there are no segments large enough */
	max = best_seg->size - size;

	/* compare the remaining segments to find the closest match */
	list_foreach_item(free_seg, &best_seg->free_list, free_list) {
		if (free_seg->size > size) {
			diff = free_seg->size - size;
			if (diff > max) {
				best_seg = free_seg;
				max = diff;
			}
		}
	}

	return best_seg;
}

/*
 * allocate memory
 */
void *malloc(size_t size)
{
	/* find an free segment */
	struct malloc_t *new_seg, *free_seg = get_free_seg(size);
	if (free_seg == NULL)
		return NULL;	/* there are no segments large enough */

	/* mark the segment as used */
	free_seg->free = 0;
	list_remove(&free_seg->free_list);
	list_insert_after(&used_list, &free_seg->free_list);

	/* divide the segment if necessary */
	if (free_seg->size-size >= sizeof(struct malloc_t)) {
		new_seg = (struct malloc_t *)((size_t)(free_seg+1) + size);
		new_seg->free = 1;
		new_seg->size = free_seg->size - size - sizeof(struct malloc_t);
		list_insert_after(&free_seg->heap_list, &new_seg->heap_list);
		list_insert_after(&free_list, &new_seg->free_list);
		free_seg->size = size;
	}

	return (void *)(free_seg + 1);
}

/*
 * free memory
 */
void free(void *ptr)
{
	struct malloc_t *prev_seg, *next_seg, *ptr_seg;

	/* get a pointer to the correct segment */
	list_find_item(ptr_seg, &used_list, free_list, (void *)(ptr_seg+1) == ptr);
	if (ptr_seg == NULL)
		return; /* FIXME: fails silently */

	list_remove(&ptr_seg->free_list);

	/* if the next segment is free, this one should absorb it */
	next_seg = list_next_item(ptr_seg, heap_list);
	if (&next_seg->heap_list != &heap_list && next_seg->free) {
		ptr_seg->size += next_seg->size + sizeof(struct malloc_t);
		list_remove(&next_seg->heap_list);
		list_remove(&next_seg->free_list);
	}

	/* if the previous segment is free, it should absorb this one */
	prev_seg = list_prev_item(ptr_seg, heap_list);
	if (&prev_seg->heap_list != &heap_list && prev_seg->free) {
		prev_seg->size += ptr_seg->size + sizeof(struct malloc_t);
		list_remove(&ptr_seg->heap_list);
	} else {
		ptr_seg->free = 1;
		list_insert_after(&free_list, &ptr_seg->free_list);
	}
}
