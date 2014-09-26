/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/list.h
 *
 * Linux style linked list
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 19 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <types.h>

struct list_t {
	struct list_t *next;
	struct list_t *prev;
};

/*
 * Declare and initialize a list
 */
#define list_decl(list) struct list_t list = { &(list), &(list) }

/*
 * Initalize a pre-allocated list
 */
static inline void list_init(struct list_t *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Return 'true' if the list is empty, 'false' otherwise
 */
#define list_empty(list) ((list)->next == list)

/*
 * Insert 'ins' after 'node'
 */
static inline void list_insert_after(struct list_t *node, struct list_t *ins)
{
	ins->next = node->next;
	ins->prev = node;
	node->next = ins;
	ins->next->prev = ins;
}

/*
 * Insert 'ins' before 'node'
 */
static inline void list_insert_before(struct list_t *node, struct list_t *ins)
{
	ins->prev = node->prev;
	ins->next = node;
	node->prev = ins;
	ins->prev->next = ins;
}

/*
 * Remove node 'rem'
 */
static inline void list_remove(struct list_t *rem)
{
	rem->prev->next = rem->next;
	rem->next->prev = rem->prev;
}

static inline void list_replace(struct list_t *node, struct list_t *rep)
{
	rep->next = node->next;
	rep->prev = node->prev;
	rep->next->prev = rep;
	rep->prev->next = rep;
}

/*
 * Iterate over every node 'iter' in 'head'
 */
#define list_foreach(head, iter) \
	for (iter = (head)->next; iter != head; iter = (iter)->next)

/*
 * Get pointer to record containing node
 */
#define list_item(node, type, field) \
	container_of(node, type, field)

/*
 * Get first item or NULL if empty
 */
#define list_first_item(head, type, field) \
	(list_empty(head) ? NULL : list_item((head)->next, type, field))

/*
 * Get pointer to next item
 */
#define list_next_item(item, field) \
	list_item((item)->field.next, typeof(*item), field)

/*
 * Get pointer to previous item
 */
#define list_prev_item(item, field) \
	list_item((item)->field.prev, typeof(*item), field)

/*
 * Iterate over every item in list
 */
#define list_foreach_item(item, head, field) \
	for (item = list_item((head)->next, typeof(*item), field); \
			&(item)->field != head; \
			item = list_next_item(item, field))

/*
 * Find first item that satisfies condition
 */
/*#define list_find_item(item, head, field, cond) \*/
	/*list_foreach_item(item, head, field) { if (cond) break; }*/

#define list_find_item(item, head, field, cond) \
	({ \
		list_foreach_item(item, head, field) \
			if (cond) break; \
		if (&(item)->field == head) item = NULL; \
	})

/*
 * Get size of list
 */
static inline int list_size(struct list_t *head)
{
	int size = 0;
	struct list_t *node = head->next;
	while (node != head) {
		++size;
		node = node->next;
	}
	return size;
}
