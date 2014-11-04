/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/rbtree.c
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
 *
 * Red-black trees are useful data structures for implementing sets and
 * maps. They are simple enough to explain if not to implement. A red-black
 * tree is a normal binary search tree with the following additional
 * properties:
 *
 *	1. Every node is either red or black
 *	2. The root node is always black
 *	3. A red node cannot have a red parent
 *	4. The number of black nodes along all paths originating at a node
 *		must be equal
 *
 * We insert and delete the same way we would in a normal BST and then we
 * modify the tree by recoloring nodes and rotating subtrees to enforce the
 * above properties. As with our linked list, nodes are embedded into the
 * data structures that are being stored. This is doubly convenient as it
 * allows the user to define their own insertion criteria and we don't have
 * to deal with user-defined comparators.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <rbtree.h>

#define RB_RED 0
#define RB_BLACK 1

/*
 * determine whether a node is red or black -- NULLs are black
 */
#define rb_red(n) ((n) != NULL &&  (n)->color == RB_RED)
#define rb_black(n) ((n) == NULL || (n)->color == RB_BLACK)

/*
 * get a pointer to a node's uncle
 */
#define rb_uncle(p, g) ((g)->left == (p) ? (g)->right : (g)->left)

/*
 * get a pointer to a node's sibling
 */
#define rb_sibling(n) \
	((n)->parent->left == (n) ? (n)->parent->right : (n)->parent->left)

/*
 * initialize a red-black tree
 */
void rb_tree_init(struct rb_tree_t *tree)
{
	tree->root = NULL;
}

/*
 * link a node into the tree
 */
void rb_link(struct rb_node_t *node, struct rb_node_t *parent,
		struct rb_node_t **link)
{
	node->color = RB_RED;
	node->parent = parent;
	node->left = NULL;
	node->right = NULL;
	*link = node;
}

/*
 * return the next node in the tree
 */
struct rb_node_t* rb_next(struct rb_node_t *node)
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
 * return the previous node in the tree
 */
struct rb_node_t* rb_prev(struct rb_node_t *node)
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
 * rotate a subtree around its root node
 *
 *	    B   --> rb_rot_right(B) -->   A
 *	   / \                           / \
 *	  A   c                         a   B
 *	 / \                               / \
 *	a   b    <-- rb_rot_left(A) <--   b   c
 */
static inline void rb_rot_left(struct rb_node_t **Aptr)
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

static inline void rb_rot_right(struct rb_node_t **Bptr)
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
 * replace one node with another
 */
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
 * Insertion into a red-black tree is straightforward. Newly inserted nodes
 * are colored red and inserted as they would be in a normal BST. To
 * maintain the red-black properties we recolor and restructure the tree
 * based on the node's position relative to its parent, grandparent and
 * uncle. All cases are terminal except for when the node's uncle is red in
 * which case we must iterate up the tree.
 */
void rb_insert(struct rb_tree_t *tree, struct rb_node_t *ins)
{
	struct rb_node_t **great, *grand, *uncle, *parent, *cur = ins;

	while (1) {
		parent = cur->parent;

		/* case 0) insert root -- recolor */
		if (parent == NULL) {
			cur->color = RB_BLACK;
			break;
		}

		/* case 1) black parent -- done */
		if (parent->color == RB_BLACK)
			break;

		grand = parent->parent;
		uncle = rb_uncle(parent, grand);

		/* case 2) uncle is red -- recolor and iterate */
		if (rb_red(uncle)) {
			grand->color = RB_RED;
			parent->color = RB_BLACK;
			uncle->color = RB_BLACK;
			cur = grand;
			continue;
		}

		great = rb_ptr(grand, tree->root);
		if (grand->left == parent) {

			/* case 3) parent is left child, node is right child */
			if (parent->right == cur) {
				rb_rot_left(&grand->left);
				rb_rot_right(great);
				cur->color = RB_BLACK;
				grand->color = RB_RED;
			}

			/* case 4) parent is left child, node is left child */
			else {
				rb_rot_right(great);
				parent->color = RB_BLACK;
				grand->color = RB_RED;
			}
		} else {

			/* case 5) parent is right child, node is left child */
			if (parent->left == cur) {
				rb_rot_right(&grand->right);
				rb_rot_left(great);
				cur->color = RB_BLACK;
				grand->color = RB_RED;
			}

			/* case 6) parent is right child, node is right child */
			else {
				rb_rot_left(great);
				parent->color = RB_BLACK;
				grand->color = RB_RED;
			}
		}
		break;
	}
}

/*
 * There's a lot going on here so let's define terms first:
 *	rem -- the node that is being removed from the tree
 *	prev -- the rightmost node in rem's left subtree
 *	cur -- the node currently causing the black height imbalance
 *	link -- link connecting the node we are removing
 *
 * If the node we are removing has two children, we replace it with the
 * previous node in the tree (max of the left subtree) and then remove that
 * node. Since replacing a node doesn't violate red-blackness we can then
 * focus on the removal of the previous node which we know has one or fewer
 * children. There are a few trivial cases but the real fun begins with
 * removing black nodes with no children. We identify a number of cases
 * based on the color of the node's parent, sibling and nephews. All cases
 * are terminal except for the rare case in which we are removing a node
 * whose parent whose parent and sibling are black and who has no nephews.
 * In this case we recolor and repeat for its parent.
 *
 * To aid in identifying removal cases we use the following legend:
 *	n = node to remove
 *	p = n's parent
 *	s = n's sibling
 *	l = s's left child
 *	r = s's right child
 *
 * Upper-case letters indicate black nodes, lower-case red nodes.
 */
void rb_remove(struct rb_tree_t *tree, struct rb_node_t *rem)
{
	struct rb_node_t **link, *sibling, *child, *parent, *cur, *prev=rem;


	/* node has two children -- find max of left subtree */
	if (prev->left != NULL && prev->right != NULL) {
		prev = rem->left;
		while (prev->right != NULL)
			prev = prev->right;
	}

	link = rb_ptr(prev, tree->root);
	child = prev->right == NULL ? prev->left : prev->right;

	/* case 0) node is red -- must be leaf so delete it */
	if (rb_red(prev)) {
		*link = NULL;

	/* case 1) node is black with red child -- replace it with child */
	} else if (rb_red(child)) {
		rb_replace(link, prev, child);
		if (prev->parent == NULL)
			child->color = RB_BLACK;

	/* node is a black leaf -- remove it and rebalance */
	} else {
		cur = prev;
		while (1) {
			parent = cur->parent;

			/* case 2) root node -- we are done */
			if (parent == NULL)
				break;

			/* Current node is left child */
			if (parent->left == cur) {
				sibling = parent->right;

				/* case 3) sibling is black with red right child */
				if (rb_red(sibling->right)) {
					/* case 3.1) N P S r */
					if (rb_black(parent)) {
						sibling->right->color = RB_BLACK;
					/* case 3.2) N p l S r */
					} else if (rb_red(sibling->left)) {
						parent->color = RB_BLACK;
						sibling->color = RB_RED;
						sibling->right->color = RB_BLACK;
					}
					/* case 3.3) N p S r */
					rb_rot_left(rb_ptr(parent, tree->root));
					break;
				}

				/* case 4) sibling is black with red left child */
				if (rb_red(sibling->left)) {
					/* case 4.1) N P l S */
					if (rb_black(parent))
						sibling->left->color = RB_BLACK;
					/* case 4.2) N p l S */
					else
						parent->color = RB_BLACK;
					rb_rot_right(&parent->right);
					rb_rot_left(rb_ptr(parent, tree->root));
					break;
				}

				/* case 5) sibling is black leaf */
				sibling->color = RB_RED;
				if (rb_red(parent)) {
					parent->color = RB_BLACK;
					break;
				}
				cur = cur->parent;
			}

			/* current node is right child */
			else {
				sibling = parent->left;

				/* case 3) sibling is black with red left child */
				if (rb_red(sibling->left)) {
					/* case 3.1) N P S r */
					if (rb_black(parent)) {
						sibling->left->color = RB_BLACK;
					/* case 3.2) N p l S r */
					} else if (rb_red(sibling->right)) {
						parent->color = RB_BLACK;
						sibling->color = RB_RED;
						sibling->left->color = RB_BLACK;
					}
					/* case 3.3) N p S r */
					rb_rot_right(rb_ptr(parent, tree->root));
					break;
				}

				/* case 4) sibling is black with red right child */
				if (rb_red(sibling->right)) {
					/* case 4.1) N P l S */
					if (rb_black(parent))
						sibling->right->color = RB_BLACK;
					/* case 4.2) N p l S */
					else
						parent->color = RB_BLACK;
					rb_rot_left(&parent->left);
					rb_rot_right(rb_ptr(parent, tree->root));
					break;
				}

				/* case 5) sibling is black leaf */
				sibling->color = RB_RED;
				if (rb_red(parent)) {
					parent->color = RB_BLACK;
					break;
				}
				cur = cur->parent;
			}
		}
		/* remove the leaf */
		*link = NULL;
	}
	/* if original node had two children replace it with its predecessor */
	if (prev != rem)
		rb_replace(rb_ptr(rem, tree->root), rem, prev);
}
