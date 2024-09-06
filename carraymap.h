#ifndef __C_ARRAY_MAP_H
#define __C_ARRAY_MAP_H

#define C_ARRAY_MAP_NODE_DELETED	((void*)(long long)(18446744073709551615))
#define C_ARRAY_MAP_NODE_KEY_LEN	24

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _tag_c_array_map_node
	{
		char key[C_ARRAY_MAP_NODE_KEY_LEN];
		void* value;
	} c_array_map_node;

	typedef struct _tag_c_array_map
	{
		int* hashes;
		c_array_map_node* array;
		int capacity;
		int limit;
		int value_free;
		int have_garbage;
	} c_array_map;

	c_array_map* c_array_map_new(int capacity, int value_free);
	void c_array_map_init(c_array_map* parr, int capacity, int value_free);
	void c_array_map_destroy(c_array_map* parr);
	void c_array_map_move(c_array_map* dst, c_array_map* src);
	void c_array_map_swap(c_array_map* dst, c_array_map* src);

	void c_array_map_insert(c_array_map* parr, char* key, void* value);
	void c_array_map_remove(c_array_map* parr, char* key);
	c_array_map_node* c_array_map_find(c_array_map* parr, char* key);
	int c_array_map_size(c_array_map* parr);
	void c_array_map_clear(c_array_map* parr);

#ifdef __cplusplus
}
#endif
#endif