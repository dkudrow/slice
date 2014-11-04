/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/rbtree.h
 *
 * Red-black tree
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	October 28 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef RBTREE_H
#define RBTREE_H

#include <types.h>

/*
 * red-black tree node
 */
struct rb_node_t {
	int color;
	struct rb_node_t *parent;
	struct rb_node_t *left;
	struct rb_node_t *right;
};

/*
 * red-black tree root
 */
struct rb_tree_t {
	struct rb_node_t *root;
};

/*
 * get a pointer to the record in which the node is embedded
 */
#define rb_item(node, type, field) (container_of(node, type, field))

/*
 * get a pointer to link between a node and it's parent -- if it's the root
 * return the address of the tree
 */
#define rb_ptr(n, r) \
	((n)->parent ? ((n)->parent->left == (n) ? &(n)->parent->left : &(n)->parent->right) : &r)

void rb_tree_init(struct rb_tree_t *tree);
void rb_link(struct rb_node_t *node, struct rb_node_t *parent,
		struct rb_node_t **link);
void rb_insert(struct rb_tree_t *tree, struct rb_node_t *ins);
void rb_remove(struct rb_tree_t *tree, struct rb_node_t *rem);
struct rb_node_t* rb_next(struct rb_node_t *node);
struct rb_node_t* rb_prev(struct rb_node_t *node);

#endif /* RBTREE_H */

