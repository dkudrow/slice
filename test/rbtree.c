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

const char *test_name = "RBTREE";

#define TEST_SIZE 26

#define rb_test_clear(tree) (tree)->root = NULL

struct rb_test_t {
	int key;
	struct rb_node_t rb_node;
};

/*
 * insert based on integer key
 */
static int rb_test_insert(struct rb_tree_t *tree, struct rb_test_t *ins)
{
	int ins_key = ins->key;
	struct rb_node_t *parent=tree->root, **cur=&tree->root;

	while (*cur != NULL) {
		parent = *cur;
		int cur_key = rb_item(parent, struct rb_test_t, rb_node)->key;
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

/*
 * A pre-order traversal uniquely identifies a binary tree. This function
 * and its recursive helper generate a string out of the nodes in the
 * traversal with black nodes as upper case letter and red nodes as lower
 * case. We use this to test against an expected tree after performing a
 * set operations.
 */
static char *rb_test_preorder_r(struct rb_node_t *node, char *result)
{
	int key;
	if (node != NULL) {
		key = rb_item(node, struct rb_test_t, rb_node)->key;
		if (node->color == 0)
			*result++ = 'a' + key;
		else 
			*result++ = 'A' + key;
		result = rb_test_preorder_r(node->left, result);
		result = rb_test_preorder_r(node->right, result);
	}
	return result;
}

static int rb_test_preorder(struct rb_tree_t *tree, char* expected, char *result)
{
	int i;
	for (i=0; i<TEST_SIZE; i++)
		result[i] = 0;

	rb_test_preorder_r(tree->root, result);

	while (*expected == *result) {
		if (*expected == '\0')
			return 1;
		expected++;
		result++;
	}
	return 0;
}

/*
 * red-black tree tests
 */ 
const char *run_test()
{
	struct rb_tree_t rbtree;
	struct rb_test_t items[TEST_SIZE];
	char buf[TEST_SIZE+1];
	int i;

	/* Populate test nodes */
	rb_tree_init(&rbtree);
	for (i=0; i<TEST_SIZE; i++)
		items[i].key = i;

	/* Insert case 0) empty tree */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[0]);
	if (rb_test_preorder(&rbtree, "A", buf) == 0)
		return "insert case 0";

	/* Insert case 1) black parent */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	if (rb_test_preorder(&rbtree, "Bac", buf) == 0)
		return "insert case 1";

	/* Insert case 2) red uncle */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[3]);
	if (rb_test_preorder(&rbtree, "BACd", buf) == 0)
		return "insert case 2";

	/* Insert case 3) parent is left child, node is right child */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[3]);
	if (rb_test_preorder(&rbtree, "BADce", buf) == 0)
		return "insert case 3";

	/* Insert case 4) parent is left child, node is left child */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[2]);
	if (rb_test_preorder(&rbtree, "BADce", buf) == 0)
		return "insert case 4";

	/* Insert case 5) parent is right child, node is left child */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[1]);
	if (rb_test_preorder(&rbtree, "DBacE", buf) == 0)
		return "insert case 5";

	/* Insert case 6) parent is right child, node is right child */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[2]);
	if (rb_test_preorder(&rbtree, "DBacE", buf) == 0)
		return "insert case 6";

	/* Delete case 0) -- red leaf */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_remove(&rbtree, &items[0].rb_node);
	if (rb_test_preorder(&rbtree, "Bc", buf) == 0)
		return "delete case 0";

	/* Delete case 1) -- black node w/ red child */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[1]);
	rb_remove(&rbtree, &items[0].rb_node);
	if (rb_test_preorder(&rbtree, "B", buf) == 0)
		return "delete case 1";

	/* Delete case 2) -- singleton tree */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[0]);
	rb_remove(&rbtree, &items[0].rb_node);
	if (rb_test_preorder(&rbtree, "", buf) == 0)
		return "delete case 2";

	/* Delete case 3.1) -- N P S r */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[4]);
	rb_remove(&rbtree, &items[0].rb_node);
	if (rb_test_preorder(&rbtree, "DBE", buf) == 0)
		return "delete case 3.1";

	/* Delete case 3.2) -- N p l S r */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[5]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[7]);
	rb_test_insert(&rbtree, &items[8]);
	rb_test_insert(&rbtree, &items[6]);
	rb_remove(&rbtree, &items[4].rb_node);
	if (rb_test_preorder(&rbtree, "DBachFgI", buf) == 0)
		return "delete case 3.2";

	/* Delete case 3.3) -- N p S r */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[5]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[7]);
	rb_test_insert(&rbtree, &items[8]);
	rb_remove(&rbtree, &items[4].rb_node);
	if (rb_test_preorder(&rbtree, "DBacHfi", buf) == 0)
		return "delete case 3.3";

	/* Delete case 4.1) -- N P l S */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[2]);
	rb_remove(&rbtree, &items[0].rb_node);
	if (rb_test_preorder(&rbtree, "CBD", buf) == 0)
		return "delete case 4.1";

	/* Delete case 4.2) -- N p l S */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[5]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[7]);
	rb_test_insert(&rbtree, &items[6]);
	rb_remove(&rbtree, &items[4].rb_node);
	if (rb_test_preorder(&rbtree, "DBacgFH", buf) == 0)
		return "delete case 4.2";

	/* Delete case 5.1) -- N P S */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[3]);
	rb_remove(&rbtree, &items[0].rb_node);
	if (rb_test_preorder(&rbtree, "CBD", buf) == 0)
		return "delete case 5.1";

	/* Delete case 5.2 -- N P S (not child of root) */
	rb_test_clear(&rbtree);
	rb_test_insert(&rbtree, &items[7]);
	rb_test_insert(&rbtree, &items[3]);
	rb_test_insert(&rbtree, &items[11]);
	rb_test_insert(&rbtree, &items[1]);
	rb_test_insert(&rbtree, &items[5]);
	rb_test_insert(&rbtree, &items[9]);
	rb_test_insert(&rbtree, &items[13]);
	rb_test_insert(&rbtree, &items[0]);
	rb_test_insert(&rbtree, &items[2]);
	rb_test_insert(&rbtree, &items[4]);
	rb_test_insert(&rbtree, &items[6]);
	rb_test_insert(&rbtree, &items[8]);
	rb_test_insert(&rbtree, &items[10]);
	rb_test_insert(&rbtree, &items[12]);
	rb_test_insert(&rbtree, &items[14]);
	rb_test_insert(&rbtree, &items[15]);
	rb_remove(&rbtree, &items[0].rb_node);
	rb_remove(&rbtree, &items[2].rb_node);
	rb_remove(&rbtree, &items[4].rb_node);
	rb_remove(&rbtree, &items[6].rb_node);
	rb_remove(&rbtree, &items[1].rb_node);
	if (rb_test_preorder(&rbtree, "LHDfJikNMOp", buf) == 0)
		return "delete case 5.2";

	return NULL;
}
