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

#define RB_RED 0
#define RB_BLACK 1

/*
 * Get a pointer to the recording containing the node
 */
#define rb_item(node, type, field) container_of(node, type, field)

/*
 * Get a pointer to the link to a node from it's parent
 */
#define rb_child_ptr(n, p) ((p)->left == (n) ? &(p)->left : &(p)->right)

/*
 * Get a pointer to a node's uncle
 */
#define rb_uncle(p, g) ((g)->left == (p) ? (g)->right : (g)->left)

struct rb_node_t {
	int color;
	struct rb_node_t *parent;
	struct rb_node_t *left;
	struct rb_node_t *right;
};

struct rb_tree_t {
	struct rb_node_t *root;
};

/*
 * Initialize a red-black tree
 */
static inline void rb_tree_init(struct rb_tree_t *tree)
{
	tree->root = NULL;
}

/*
 * Link a new node into the tree
 */
static inline void rb_link(struct rb_node_t *node, struct rb_node_t *parent, struct rb_node_t **link)
{
	node->color = RB_RED;
	node->parent = parent;
	node->left = NULL;
	node->right = NULL;
	*link = node;
}

/*
 * Return the next node in the tree
 */
static inline struct rb_node_t* rb_next(struct rb_node_t *node)
{
	if (node->right != NULL) {
		node = node->right;
		while (node->left != NULL)
			node = node->left;
		return node;
	}

	while (node->parent != NULL && node->parent->left != node)
		node = node->parent;
	return node->parent;
}

/*
 * Return the previous node in the tree
 */
static inline struct rb_node_t* rb_prev(struct rb_node_t *node)
{
	if (node->left != NULL) {
		node = node->left;
		while (node->right != NULL)
			node = node->right;
		return node;
	}

	while (node->parent != NULL && node->parent->right != node)
		node = node->parent;
	return node->parent;
}

/*
 * Perform a left or right subtree rotation
 *
 *	    B   --> right(B) -->   A
 *	   / \                    / \
 *	  A   c                  a   B
 *	 / \                        / \
 *	a   b    <-- left(A) <--   b   c
 */
static inline void rb_rotate_left(struct rb_node_t **Aptr)
{
	struct rb_node_t *A = *Aptr;
	struct rb_node_t *B = A->right;
	A->right = B->left;
	if (A->right != NULL)
		A->right->parent = A;
	B->left = A;
	B->parent = A->parent;
	A->parent = B;
	*Aptr = B;
}

static inline void rb_rotate_right(struct rb_node_t **Bptr)
{
	struct rb_node_t *B = *Bptr;
	struct rb_node_t *A = B->left;
	B->left = A->right;
	if (B->left != NULL)
		B->left->parent = B;
	A->right = B;
	A->parent = B->parent;
	B->parent = A;
	*Bptr = A;
}

/*
 * Enforce red-black tree properties on a tree with a newly inserted node
 */
static inline void rb_insert(struct rb_tree_t *tree, struct rb_node_t *ins)
{
	struct rb_node_t *cur = ins;

	while (1) {
		struct rb_node_t *parent = cur->parent;

		/* Case 0) insert root */
		if (parent == NULL) {
			cur->color = RB_BLACK;
			break;
		}

		/* Case 1) black parent */
		if (parent->color == RB_BLACK)
			break;

		struct rb_node_t *grand = parent->parent;
		struct rb_node_t *uncle = rb_uncle(parent, grand);

		/* Case 2) uncle is red */
		if (uncle != NULL && uncle->color == RB_RED) {
			grand->color = RB_BLACK;
			parent->color = RB_RED;
			uncle->color = RB_RED;
			cur = grand;
			continue;
		}

		struct rb_node_t **great = grand->parent == NULL ? &tree->root : rb_child_ptr(grand, grand->parent);

		if (grand->left == parent) {
			/* Case 3) parent is left child, node is right child */
			if (parent->right == cur)
				rb_rotate_left(&grand->left);

			/* Case 4) parent is left child, node is left child */
			rb_rotate_right(great);
		}

		else {
			/* Case 5) parent is right child, node is left child */
			if (parent->left == cur)
				rb_rotate_right(&grand->right);

			/* Case 6) parent is right child, node is right child */
			rb_rotate_left(great);
		}
		parent->color = RB_BLACK;
		grand->color = RB_RED;
		break;
	}
}

#endif /* RBTREE_H */

