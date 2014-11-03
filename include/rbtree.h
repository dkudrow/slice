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
#define rb_ptr(n, r) \
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
	if (old->left != new) {
		new->left = old->left;
		if (old->left != NULL)
			old->left->parent = new;
	}
	if (old->right != new) {
		new->right = old->right;
		if (old->right != NULL)
			old->right->parent = new;
	}
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
			grand->color = RB_RED;
			parent->color = RB_BLACK;
			uncle->color = RB_BLACK;
			cur = grand;
			continue;
		}

		great = rb_ptr(grand, tree->root);

		if (grand->left == parent) {
			/* Case 3) parent is left child, node is right child */
			if (parent->right == cur) {
				rb_rotate_left(&grand->left);
				rb_rotate_right(great);
				cur->color = RB_BLACK;
				grand->color = RB_RED;
			}

			/* Case 4) parent is left child, node is left child */
			else {
				rb_rotate_right(great);
				parent->color = RB_BLACK;
				grand->color = RB_RED;
			}
		}

		else {
			/* Case 5) parent is right child, node is left child */
			if (parent->left == cur) {
				rb_rotate_right(&grand->right);
				rb_rotate_left(great);
				cur->color = RB_BLACK;
				grand->color = RB_RED;
			}

			/* Case 6) parent is right child, node is right child */
			else {
				rb_rotate_left(great);
				parent->color = RB_BLACK;
				grand->color = RB_RED;
			}
		}
		break;
	}
}

/*
 * Capital letter indicates red node
 * n = node to remove
 * p = n's parent
 * c = n's child
 * s = n's sibling
 * l = s's left child
 * r = s's right child
 *
 * There's a lot going on here so let's define terms first;
 * `rem' is the node that we are trying to remove. If it has two children,
 * then we will replace with the max node in it's left subtree, `max_left'. 
 *
 */
static inline void rb_remove(struct rb_tree_t *tree, struct rb_node_t *rem)
{
	struct rb_node_t **link, *sibling, *child, *parent, *cur, *max_left=rem;


	/* Node has two children -- find max of left subtree */
	if (max_left->left != NULL && max_left->right != NULL) {
		max_left = rem->left;
		while (max_left->right != NULL)
			max_left = max_left->right;
	}

	link = rb_ptr(max_left, tree->root);
	child = max_left->right == NULL ? max_left->left : max_left->right;

	/* Case 0) node is red -- must be leaf so delete it */
	if (rb_red(max_left)) {
		*link = NULL;

	/* Case 1) node is black w/ one red child -- replace it with its child */
	} else if (rb_red(child)) {
		rb_replace(link, max_left, child);
		if (max_left->parent == NULL)
			child->color = RB_BLACK;

	/* Node is a black leaf -- remove it and rebalance */
	} else {
		/* remove the leaf */
		*link = NULL;

		cur = max_left;
		while (1) {
			parent = cur->parent;

			/* Case 2) root node -- we are done */
			if (parent == NULL)
				break;

			/* Current node is left child */
			if (parent->left == cur) {
				sibling = parent->right;

				/* Sibling is black with red right child */
				if (rb_red(sibling->right)) {
					rb_rotate_left(rb_ptr(parent, tree->root));
					if (rb_black(parent))
						sibling->right->color = RB_BLACK;
					else if (rb_red(sibling->left))
						sibling->left->color = RB_BLACK;
					break;
				}

				/* sibling is black with red left child */
				if (rb_red(sibling->left)) {
					rb_rotate_right(&parent->right);
					rb_rotate_left(rb_ptr(parent, tree->root));
					if (rb_black(parent))
						parent->color = RB_BLACK;
					else
						sibling->left->color = RB_RED;
					break;
				}

				/* sibling is black leaf */
				sibling->color = RB_RED;
				if (rb_red(parent)) {
					parent->color = RB_BLACK;
					break;
				}
				cur = cur->parent;
			}

			/* current node is LEFT child */
			else {
				sibling = parent->left;

				/* sibling is black with red left child */
				if (rb_red(sibling->left)) {
					rb_rotate_right(rb_ptr(parent, tree->root));
					if (rb_black(parent))
						sibling->left->color = RB_BLACK;
					else if (rb_red(sibling->right))
						sibling->right->color = RB_BLACK;
					break;
				}

				/* sibling is black with red right child */
				if (rb_red(sibling->right)) {
					rb_rotate_left(&parent->left);
					rb_rotate_right(rb_ptr(parent, tree->root));
					if (rb_black(parent))
						parent->color = RB_BLACK;
					else
						sibling->right->color = RB_RED;
					break;
				}

				/* sibling is black leaf */
				sibling->color = RB_RED;
				if (rb_red(parent)) {
					parent->color = RB_BLACK;
					break;
				}
				cur = parent;
			}
		}
	}

	if (max_left != rem)
		rb_replace(rb_ptr(rem, tree->root), rem, max_left);
}

#endif /* RBTREE_H */

