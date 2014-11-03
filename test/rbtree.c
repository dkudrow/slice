/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * test/rbtree.c
 *
 * Tests for red-black tree
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	October 28 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <rbtree.h>

#define TEST_SIZE 26
#define rbtree_test_clear(tree) (tree)->root = NULL

struct rbtree_test_t {
	int key;
	struct rb_node_t rb_node;
};

static int rbtree_test_insert(struct rb_tree_t *tree, struct rbtree_test_t *ins)
{
	int ins_key = ins->key;
	struct rb_node_t *parent=tree->root, **cur=&tree->root;

	while (*cur != NULL) {
		parent = *cur;
		int cur_key = rb_item(parent, struct rbtree_test_t, rb_node)->key;
		if (ins_key > cur_key)
			cur = &parent->right;
		else if (ins_key < cur_key)
			cur = &parent->left;
		else
			return -1;
	}

	rb_link(&ins->rb_node, parent, cur);
	rb_insert(tree, &ins->rb_node);

	return 0;
}

static char *rbtree_test_preorder_r(struct rb_node_t *node, char *result)
{
	int key;
	if (node != NULL) {
		key = rb_item(node, struct rbtree_test_t, rb_node)->key;
		if (rb_red(node))
			*result++ = 'a' + key;
		else 
			*result++ = 'A' + key;
		result = rbtree_test_preorder_r(node->left, result);
		result = rbtree_test_preorder_r(node->right, result);
	}
	return result;
}

static int rbtree_test_preorder(struct rb_tree_t *tree, char* expected, char *result)
{
	int i;
	for (i=0; i<TEST_SIZE; i++)
		result[i] = 0;

	rbtree_test_preorder_r(tree->root, result);

	while (*expected == *result) {
		if (*expected == '\0')
			return 1;
		expected++;
		result++;
	}
	return 0;
}

#define ERR_DOUBLE_RED -1
#define ERR_BALANCE -2
int rb_enforce_r(struct rb_node_t *node, struct rb_node_t **err)
{
	int black_height_l, black_height_r;

	if (node == NULL)
		return 1;

	if (rb_red(node) && rb_red(node->parent)) {
		*err = node;
		return ERR_DOUBLE_RED;
	}

	black_height_l = rb_enforce_r(node->left, err);
	black_height_r = rb_enforce_r(node->right, err);

	if (black_height_l < 0)
		return black_height_l;

	if (black_height_r < 0)
		return black_height_r;

	if (black_height_l != black_height_r) {
		*err = node;
		return ERR_BALANCE;
	}

	return black_height_l + (rb_black(node) ? 1 : 0);
}

int rb_enforce(struct rb_tree_t *tree, struct rb_node_t **err)
{
	struct rb_node_t *node = tree->root;

	/* make sure root is black or tree is empty */
	if (rb_red(node))
		return -1;

	return rb_enforce_r(node, err);
}

char *rbtree_test()
{
	struct rb_tree_t rbtree;
	struct rbtree_test_t items[TEST_SIZE];
	char buf[TEST_SIZE];
	int i;

	rb_tree_init(&rbtree);
	for (i=0; i<TEST_SIZE; i++)
		items[i].key = i;

	/* Insertion tests */

	/* Case 0) empty tree */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[0]);
	if (rbtree_test_preorder(&rbtree, "A", buf) == 0)
		return "insert case 0";

	/* Case 1) black parent */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[1]);
	rbtree_test_insert(&rbtree, &items[0]);
	rbtree_test_insert(&rbtree, &items[2]);
	if (rbtree_test_preorder(&rbtree, "Bac", buf) == 0)
		return "insert case 1";

	/* Case 2) red uncle */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[1]);
	rbtree_test_insert(&rbtree, &items[0]);
	rbtree_test_insert(&rbtree, &items[2]);
	rbtree_test_insert(&rbtree, &items[3]);
	if (rbtree_test_preorder(&rbtree, "BACd", buf) == 0)
		return "insert case 2";

	/* Case 3) parent is left child, node is right child */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[1]);
	rbtree_test_insert(&rbtree, &items[0]);
	rbtree_test_insert(&rbtree, &items[4]);
	rbtree_test_insert(&rbtree, &items[2]);
	rbtree_test_insert(&rbtree, &items[3]);
	if (rbtree_test_preorder(&rbtree, "BADce", buf) == 0)
		return "insert case 3";

	/* Case 4) parent is left child, node is left child */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[1]);
	rbtree_test_insert(&rbtree, &items[0]);
	rbtree_test_insert(&rbtree, &items[4]);
	rbtree_test_insert(&rbtree, &items[3]);
	rbtree_test_insert(&rbtree, &items[2]);
	if (rbtree_test_preorder(&rbtree, "BADce", buf) == 0)
		return "insert case 4";

	/* Case 5) parent is right child, node is left child */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[3]);
	rbtree_test_insert(&rbtree, &items[4]);
	rbtree_test_insert(&rbtree, &items[0]);
	rbtree_test_insert(&rbtree, &items[2]);
	rbtree_test_insert(&rbtree, &items[1]);
	if (rbtree_test_preorder(&rbtree, "DBacE", buf) == 0)
		return "insert case 5";

	/* Case 6) parent is right child, node is right child */
	rbtree_test_clear(&rbtree);
	rbtree_test_insert(&rbtree, &items[3]);
	rbtree_test_insert(&rbtree, &items[4]);
	rbtree_test_insert(&rbtree, &items[0]);
	rbtree_test_insert(&rbtree, &items[1]);
	rbtree_test_insert(&rbtree, &items[2]);
	if (rbtree_test_preorder(&rbtree, "DBacE", buf) == 0)
		return "insert case 6";

	return NULL;
}
