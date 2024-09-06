#include "cavltreemap.h"
#include "clib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef max
static inline int max(int a, int b)
{
	return (a > b) ? a : b;
}
#endif

static inline int height(c_avl_tree_map_node* node)
{
	return (node == NULL) ? 0 : node->height;
}

static c_avl_tree_map_node* c_avl_tree_map_node_new(void* key, void* value)
{
	c_avl_tree_map_node* new_node = (c_avl_tree_map_node*)malloc(sizeof(c_avl_tree_map_node));

	if (new_node == NULL)
	{
		return NULL;
	}

	new_node->key = key;
	new_node->value = value;
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->height = 1;
	return new_node;
}

static void c_avl_tree_map_node_destroy(c_avl_tree_map* ptree, c_avl_tree_map_node* pnode)
{
	if (pnode != NULL)
	{
		if (ptree->value_free)
		{
			free(pnode->value);
		}

		free(pnode);
		pnode = NULL;
	}
}

c_avl_tree_map* c_avl_tree_map_new(int value_free, int (*compareTo)(const void*, const void*))
{
	c_avl_tree_map* ptree = (c_avl_tree_map*)malloc(sizeof(c_avl_tree_map));

	c_avl_tree_map_init(ptree, value_free, compareTo);
	return ptree;
}

void c_avl_tree_map_init(c_avl_tree_map* ptree, int value_free, int (*compareTo)(const void*, const void*))
{
	ptree->root = NULL;
	ptree->count = 0;
	ptree->value_free = value_free;
	ptree->compareTo = compareTo;
}

void c_avl_tree_map_destroy(c_avl_tree_map* ptree)
{
	if (ptree == NULL)
	{
		return;
	}

	c_avl_tree_map_clear(ptree);
	free(ptree);
}

void c_avl_tree_map_move(c_avl_tree_map* dst, c_avl_tree_map* src)
{
	c_avl_tree_map_node* root = dst->root;

	*dst = *src;

	src->root = root;
	src->count = 0;
}

void c_avl_tree_map_swap(c_avl_tree_map* dst, c_avl_tree_map* src)
{
	c_avl_tree_map tmp = *dst;
	*dst = *src;
	*src = tmp;
}

static int avl_update_height(c_avl_tree_map_node* pnode)
{
	return pnode ? 1 + max(height(pnode->left), height(pnode->right)) : 0;
}

static int avl_balance_factor(c_avl_tree_map_node* pnode)
{
	return pnode ? height(pnode->left) - height(pnode->right) : 0;
}

static c_avl_tree_map_node* avl_rotate_left(c_avl_tree_map_node* pnode)
{
	c_avl_tree_map_node* right_child = pnode->right;
	pnode->right = right_child->left;
	right_child->left = pnode;

	pnode->height = avl_update_height(pnode);
	right_child->height = avl_update_height(right_child);

	return right_child;
}

static c_avl_tree_map_node* avl_rotate_right(c_avl_tree_map_node* pnode)
{
	c_avl_tree_map_node* left_child = pnode->left;
	pnode->left = left_child->right;
	left_child->right = pnode;

	pnode->height = avl_update_height(pnode);
	left_child->height = avl_update_height(left_child);

	return left_child;
}

static c_avl_tree_map_node* avl_insert(c_avl_tree_map* ptree, c_avl_tree_map_node* pnode, void* key, void* value)
{
	if (pnode == NULL)
	{
		c_avl_tree_map_node* new_node = c_avl_tree_map_node_new(key, value);

		if (new_node == NULL)
		{
			return NULL;
		}

		ptree->count++;

		return new_node;
	}
	else
	{
		int result = ptree->compareTo(key, pnode->key);

		if (result > 0)
		{
			pnode->right = avl_insert(ptree, pnode->right, key, value);

			if (avl_balance_factor(pnode) == -2)
			{
				if (ptree->compareTo(key, ((c_avl_tree_map_node*)pnode->right)->key) > 0)
				{
					pnode = avl_rotate_left(pnode);
				}
				else
				{
					pnode->right = avl_rotate_right(pnode->right);
					pnode = avl_rotate_left(pnode);
				}
			}

			pnode->height = avl_update_height(pnode);
			return pnode;
		}
		else if (result < 0)
		{
			pnode->left = avl_insert(ptree, pnode->left, key, value);

			if (avl_balance_factor(pnode) == 2)
			{
				if (ptree->compareTo(key, ((c_avl_tree_map_node*)pnode->left)->key) < 0)
				{
					pnode = avl_rotate_right(pnode);
				}
				else
				{
					pnode->left = avl_rotate_left(pnode->left);
					pnode = avl_rotate_right(pnode);
				}
			}

			pnode->height = avl_update_height(pnode);
			return pnode;
		}
		else
		{
			if (ptree->value_free)
			{
				free(pnode->value);
			}

			pnode->value = value;
			return pnode;
		}
	}
}

static c_avl_tree_map_node* avl_remove(c_avl_tree_map* ptree, c_avl_tree_map_node* pnode, void* key)
{
	if (pnode == NULL)
	{
		return NULL;
	}

	int cmp = ptree->compareTo(key, pnode->key);

	if (cmp > 0)
	{
		pnode->right = avl_remove(ptree, pnode->right, key);
	}
	else if (cmp < 0)
	{
		pnode->left = avl_remove(ptree, pnode->left, key);
	}
	else
	{
		if (pnode->left == NULL)
		{
			c_avl_tree_map_node* temp = pnode->right;

			c_avl_tree_map_node_destroy(ptree, pnode);
			ptree->count--;

			return temp;
		} else if (pnode->right == NULL)
		{
			c_avl_tree_map_node* temp = pnode->left;

			c_avl_tree_map_node_destroy(ptree, pnode);
			ptree->count--;

			return temp;
		}
		else
		{
			c_avl_tree_map_node* temp = c_avl_tree_map_first(pnode->right);

			void* old_key = pnode->key;
			void* old_value = pnode->value;

			pnode->key = temp->key;
			pnode->value = temp->value;

			temp->key = old_key;
			temp->value = old_value;

			pnode->right = avl_remove(ptree, pnode->right, temp->key);
		}
	}

	if (pnode == NULL)
		return pnode;

	pnode->height = avl_update_height(pnode);

	int balance = avl_balance_factor(pnode);

	if (balance > 1 && avl_balance_factor(pnode->left) >= 0)
		return avl_rotate_right(pnode);

	if (balance > 1 && avl_balance_factor(pnode->left) < 0)
	{
		pnode->left = avl_rotate_left(pnode->left);
		return avl_rotate_right(pnode);
	}

	if (balance < -1 && avl_balance_factor(pnode->right) <= 0)
		return avl_rotate_left(pnode);

	if (balance < -1 && avl_balance_factor(pnode->right) > 0)
	{
		pnode->right = avl_rotate_right(pnode->right);
		return avl_rotate_left(pnode);
	}

	return pnode;
}

static c_avl_tree_map_node* avl_find(c_avl_tree_map* ptree, c_avl_tree_map_node* pnode, void* key)
{
	while (pnode != NULL)
	{
		int cmp = ptree->compareTo(key, pnode->key);

		if (cmp == 0)
		{
			return pnode;
		}
		else if (cmp > 0)
		{
			pnode = pnode->right;
		}
		else
		{
			pnode = pnode->left;
		}
	}

	return NULL;
}

static void avl_clear(c_avl_tree_map* ptree, c_avl_tree_map_node* pnode)
{
	if (pnode)
	{
		avl_clear(ptree, pnode->left);
		avl_clear(ptree, pnode->right);
		c_avl_tree_map_node_destroy(ptree, pnode);
	}
}

static void avl_inorder(c_avl_tree_map_node* pnode)
{
	if (pnode)
	{
		avl_inorder(pnode->left);
		printf("%d ", pnode->key);
		avl_inorder(pnode->right);
	}
}

static void avl_preorder(c_avl_tree_map_node* pnode)
{
	if (pnode)
	{
		//printf("%d ", pnode->key);
		avl_preorder(pnode->left);
		avl_preorder(pnode->right);
	}
}

static void avl_postorder(c_avl_tree_map_node* pnode)
{
	if (pnode)
	{
		avl_postorder(pnode->left);
		avl_postorder(pnode->right);
		//printf("%d ", pnode->key);
	}
}

void c_avl_tree_map_insert(c_avl_tree_map* ptree, void* key, void* value)
{
	ptree->root = avl_insert(ptree, ptree->root, key, value);
}

c_avl_tree_map_node* c_avl_tree_map_find(c_avl_tree_map* ptree, void* key)
{
	return avl_find(ptree, ptree->root, key);
}

void c_avl_tree_map_remove(c_avl_tree_map* ptree, void* key)
{
	ptree->root = avl_remove(ptree, ptree->root, key);
}

void c_avl_tree_map_clear(c_avl_tree_map* ptree)
{
	avl_clear(ptree, ptree->root);
	ptree->root = NULL;
	ptree->count = 0;
}

c_avl_tree_map_node* c_avl_tree_map_lower_bound(c_avl_tree_map* ptree, void* key)
{
	c_avl_tree_map_node* current = ptree->root;
	c_avl_tree_map_node* result = NULL;

	while (current != NULL)
	{
		int cmp = ptree->compareTo(key, current->key);

		if (cmp <= 0)
		{
			result = current;
			current = current->left;
		}
		else
		{
			current = current->right;
		}
	}

	return result;
}

c_avl_tree_map_node* c_avl_tree_map_upper_bound(c_avl_tree_map* ptree, void* key)
{
	c_avl_tree_map_node* current = ptree->root;
	c_avl_tree_map_node* result = NULL;

	while (current != NULL)
	{
		int cmp = ptree->compareTo(key, current->key);

		if (cmp < 0)
		{
			result = current;
			current = current->left;
		}
		else
		{
			current = current->right;
		}
	}

	return result;
}

void c_avl_tree_map_inorder(c_avl_tree_map* ptree)
{
	avl_inorder(ptree->root);
}

void c_avl_tree_map_preorder(c_avl_tree_map* ptree)
{
	avl_preorder(ptree->root);
}

void c_avl_tree_map_postorder(c_avl_tree_map* ptree)
{
	avl_postorder(ptree->root);
}

c_avl_tree_map_node* c_avl_tree_map_first(c_avl_tree_map_node* pnode)
{
	if (pnode != NULL)
	{
		while (pnode->left != NULL)
		{
			pnode = pnode->left;
		}
	}

	return pnode;
}

c_avl_tree_map_node* c_avl_tree_map_last(c_avl_tree_map_node* pnode)
{
	if (pnode != NULL)
	{
		while (pnode->right != NULL)
		{
			pnode = pnode->right;
		}
	}

	return pnode;
}

void c_avl_tree_map_foreach(c_avl_tree_map_node* pnode, void (*func)(const void*, const void*))
{
	if (pnode)
	{
		c_avl_tree_map_foreach(pnode->left, func);
		func(pnode->key, pnode->value);
		c_avl_tree_map_foreach(pnode->right, func);
	}
}
