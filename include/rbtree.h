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

#define rb_red(n) ((n) != NULL &&  (n)->color == RB_RED)
#define rb_black(n) ((n) == NULL || (n)->color == RB_BLACK)

/*
 * Get a pointer to the recording containing the node
 */
#define rb_item(node, type, field) container_of(node, type, field)

/*
 * Get a pointer to the link to a node from it's parent
 */
#define rb_ptr_to_node(n, r) \
	((n)->parent ? ((n)->parent->left == (n) ? &(n)->parent->left : &(n)->parent->right) : &r)


/*
 * Get a pointer to a node's uncle
 */
#define rb_uncle(p, g) ((g)->left == (p) ? (g)->right : (g)->left)

/*
 * Get a pointer to a node's brother sibling
 */
#define rb_sibling(n) \
	((n)->parent->left == (n) ? (n)->parent->right : (n)->parent->left)

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
static inline void rb_link(struct rb_node_t *node, struct rb_node_t *parent,
		struct rb_node_t **link)
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

static inline void rb_replace(struct rb_node_t **link,
		struct rb_node_t *old, struct rb_node_t *new)
{
	*link = new;
	new->color = old->color;
	new->parent = old->parent;
	new->left = old->left;
	new->right = old->right;
}

/*
 * Enforce red-black tree properties on a tree with a newly inserted node
 */
static inline void rb_insert(struct rb_tree_t *tree, struct rb_node_t *ins)
{
	struct rb_node_t **great, *grand, *uncle, *parent, *cur = ins;

	while (1) {
		parent = cur->parent;

		/* Case 0) insert root */
		if (parent == NULL) {
			cur->color = RB_BLACK;
			break;
		}

		/* Case 1) black parent */
		if (parent->color == RB_BLACK)
			break;

		grand = parent->parent;
		uncle = rb_uncle(parent, grand);

		/* Case 2) uncle is red */
		if (rb_red(uncle)) {
			grand->color = RB_BLACK;
			parent->color = RB_RED;
			uncle->color = RB_RED;
			cur = grand;
			continue;
		}

		great = rb_ptr_to_node(grand, tree->root);

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

static inline struct rb_node_t *rb_

static inline void rb_remove(struct rb_tree_t *tree, struct rb_node_t *rem)
{
	struct rb_node_t **link, *rep, *sibling, *child, *parent;

	rep = rem;

	/* node has two children */
	if (rep->left != NULL && rep->right != NULL) {
		rep = rep->left;
		while (rep->right != NULL)
			rep = rep->right;
	}
	link = rb_ptr_to_node(rep, tree->root);

	/* node is red with no children */
	if (rb_red(rep)) {
		*link = NULL;
		return
	}

	child = rep->left == NULL ? rep->right : rep->left;

	/* node is black with one red child */
	if (rb_red(child)) {
		rb_replace(link, rep, child);
		return;
	}

	*link = NULL;

	/* node is root (black) and has no children */
	if (rep->parent == NULL)
		return;

	if (rep->parent->left == rep) {
		sibling = rep->parent->right;
		/* node is black lead with red sibling */
		if (rb_red(sibling)) {
			rep->parent->color = RB_RED;
			sibling>->color = RB_BLACK;
			rb_rotate_left(rep->parent);
		} else if (rb_red(sibling->
	} else {
		sibling = rep->parent->left;
		/* leaf with red sibling */
		if (rb_red(sibling)) {
			rep->parent->color = RB_RED;
			sibling>->color = RB_BLACK;
			rb_rotate_right(parent);
		}
	}

}

#endif /* RBTREE_H */

