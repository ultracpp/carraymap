#include "carraymap.h"
#include "clib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary_search.h"

#define C_ARRAY_MAP_INIT_CAPACITY	8
#define C_ARRAY_MAP_ELEM_LEN	sizeof(c_array_map_node)
#define C_ARRAY_MAP_HASH_MIX	0

static unsigned long long quickhash64_(const char* str, unsigned long long mix)
{
	const unsigned long long mulp = 2654435789;

	mix ^= 104395301;

	while (*str)
		mix += (*str++ * mulp) ^ (mix >> 23);

	return mix ^ (mix << 37);
}

static void gc(c_array_map* parr)
{
	int n = parr->limit;
	int o = 0;

	for (int i = 0; i < n; i++)
	{
		void* val = parr->array[i].value;

		if (val != C_ARRAY_MAP_NODE_DELETED)
		{
			if (i != o)
			{
				parr->hashes[o] = parr->hashes[i];
				strcpy(parr->array[o].key, parr->array[i].key);
				parr->array[o].value = val;

				parr->array[i].value = C_ARRAY_MAP_NODE_DELETED;
			}
			o++;
		}
	}

	parr->have_garbage = 0;
	parr->limit = o;
}

c_array_map* c_array_map_new(int capacity, int value_free)
{
	int len = sizeof(c_array_map);
	c_array_map* parr = (c_array_map*)malloc(len);
	memset(parr, 0xff, len);

	c_array_map_init(parr, capacity, value_free);
	return parr;
}

void c_array_map_init(c_array_map* parr, int capacity, int value_free)
{
	parr->capacity = capacity;
	parr->value_free = value_free;

	if (parr->capacity < 1)
	{
		parr->capacity = C_ARRAY_MAP_INIT_CAPACITY;
	}

	int len = sizeof(int) * parr->capacity;
	parr->hashes = (int*)malloc(len);
	memset(parr->hashes, 0xff, len);

	len = C_ARRAY_MAP_ELEM_LEN * parr->capacity;
	parr->array = (c_array_map_node*)malloc(len);
	memset(parr->array, 0xff, len);

	parr->limit = 0;
	parr->have_garbage = 0;
}

void c_array_map_destroy(c_array_map* parr)
{
	c_array_map_clear(parr);

	if (parr->array != NULL)
	{
		free(parr->array);
		parr->array = NULL;
	}

	if (parr->hashes != NULL)
	{
		free(parr->hashes);
		parr->hashes = NULL;
	}
}

void c_array_map_move(c_array_map* dst, c_array_map* src)
{
	gc(src);

	*dst = *src;

	c_array_map_init(src, C_ARRAY_MAP_INIT_CAPACITY, src->value_free);
}

void c_array_map_swap(c_array_map* dst, c_array_map* src)
{
	gc(src);
	gc(dst);

	c_array_map tmp = *dst;
	*dst = *src;
	*src = tmp;
}

static int indexOf(c_array_map* parr, char* key, int hash)
{
	if (parr->limit == 0)
	{
		return ~0;
	}

	int index = binary_search_int(parr->hashes, parr->limit, hash);

	if (index < 0)
	{
		return index;
	}

	if (strcmp(key, parr->array[index].key) == 0)
	{
		return index;
	}

	int end;

	for (end = index + 1; end < parr->limit && parr->hashes[end] == hash; end++)
	{
		if (strcmp(key, parr->array[end].key) == 0)
		{
			return end;
		}
	}

	for (int i = index - 1; i >= 0 && parr->hashes[i] == hash; i--)
	{
		if (strcmp(key, parr->array[i].key) == 0)
		{
			return i;
		}
	}

	return ~end;
}

void c_array_map_insert(c_array_map* parr, char* key, void* value)
{
	int hash = quickhash64_(key, C_ARRAY_MAP_HASH_MIX);
	int index = indexOf(parr, key, hash);

	if (index < 0)
	{
		index = ~index;

		if (index < parr->limit && parr->array[index].value == C_ARRAY_MAP_NODE_DELETED)
		{
			parr->hashes[index] = hash;
			strcpy(parr->array[index].key, key);
			parr->array[index].value = value;
			return;
		}

		if (parr->have_garbage && parr->limit >= parr->capacity)
		{
			gc(parr);

			index = ~indexOf(parr, key, hash);
		}

		if (parr->limit + 1 <= parr->capacity)
		{
			for (int i = parr->limit - index - 1; i >= 0; i--)
			{
				parr->hashes[index + 1 + i] = parr->hashes[index + i];
				strcpy(parr->array[index + 1 + i].key, parr->array[index + i].key);
				parr->array[index + 1 + i].value = parr->array[index + i].value;
			}

			parr->hashes[index] = hash;
			strcpy(parr->array[index].key, key);
			parr->array[index].value = value;

			parr->limit++;
			return;
		}

		int n = parr->capacity;
		int newCapacity = n << 1;

		int len = sizeof(int) * newCapacity;
		int* newHashes = (int*)malloc(len);
		memset(newHashes, 0xff, len);

		len = C_ARRAY_MAP_ELEM_LEN * newCapacity;
		c_array_map_node* newArray = (c_array_map_node*)malloc(len);
		memset(newArray, 0xff, len);

		for (int i = 0; i < index; i++)
		{
			newHashes[i] = parr->hashes[i];
			strcpy(newArray[i].key, parr->array[i].key);
			newArray[i].value = parr->array[i].value;
		}

		newHashes[index] = hash;
		strcpy(newArray[index].key, key);
		newArray[index].value = value;

		for (int i = 0, j = parr->capacity - index; i < j; i++)
		{
			newHashes[index + 1 + i] = parr->hashes[index + i];
			strcpy(newArray[index + 1 + i].key, parr->array[index + i].key);
			newArray[index + 1 + i].value = parr->array[index + i].value;
		}

		free(parr->hashes);
		free(parr->array);

		parr->hashes = newHashes;
		parr->array = newArray;
		parr->capacity = newCapacity;

		parr->limit++;
		return;
	}
	else
	{
		if (parr->value_free)
		{
			free(parr->array[index].value);
		}

		parr->hashes[index] = hash;
		strcpy(parr->array[index].key, key);
		parr->array[index].value = value;
	}
}

void c_array_map_remove(c_array_map* parr, char* key)
{
	int hash = quickhash64_(key, C_ARRAY_MAP_HASH_MIX);
	int index = indexOf(parr, key, hash);

	if (index >= 0)
	{
		if (parr->array[index].value != C_ARRAY_MAP_NODE_DELETED)
		{
			if (parr->value_free)
			{
				free(parr->array[index].value);
			}

			parr->array[index].value = C_ARRAY_MAP_NODE_DELETED;

			parr->have_garbage = 1;
		}
	}
}

c_array_map_node* c_array_map_find(c_array_map* parr, char* key)
{
	int hash = quickhash64_(key, C_ARRAY_MAP_HASH_MIX);
	int index = indexOf(parr, key, hash);

	if (index < 0 || parr->array[index].value == C_ARRAY_MAP_NODE_DELETED)
	{
		return NULL;
	}
	else
	{
		return &parr->array[index];
	}
}

int c_array_map_size(c_array_map* parr)
{
	if (parr->have_garbage)
	{
		gc(parr);
	}

	return parr->limit;
}

void c_array_map_clear(c_array_map* parr)
{
	int n = parr->limit;

	if (parr->value_free)
	{
		for (int i = 0; i < n; i++)
		{
			if (parr->array[i].value != C_ARRAY_MAP_NODE_DELETED)
			{
				free(parr->array[i].value);
			}

			parr->array[i].value = C_ARRAY_MAP_NODE_DELETED;
		}
	} else
	{
		for (int i = 0; i < n; i++)
		{
			parr->array[i].value = C_ARRAY_MAP_NODE_DELETED;
		}
	}

	parr->limit = 0;
	parr->have_garbage = 0;
}
