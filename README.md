# C Array Map

`c_array_map` is a simple and memory-efficient implementation of an associative array (hash map) using an array of nodes in C. It provides basic functionalities such as insertion, removal, and searching of key-value pairs. The map automatically resizes itself when needed and supports garbage collection to manage deleted entries.

## Features

- **Dynamic Resizing**: Automatically resizes the underlying storage as needed to optimize memory usage.
- **Garbage Collection**: Efficiently removes deleted entries during resize operations to save memory.
- **Custom Value Management**: Optionally frees the values associated with the keys, helping to manage memory usage.
- **Efficient Search**: Uses a hash function to quickly locate keys.
- **Memory Efficiency**: Designed to minimize memory overhead by resizing dynamically and managing deleted entries effectively.

## Data Structures

- **`c_array_map_node`**: A structure representing a key-value pair with a fixed-size key and a pointer to the value.
- **`c_array_map`**: The main structure containing the hash array, node array, and metadata.

## Functions

### Creation and Initialization

- `c_array_map* c_array_map_new(int capacity, int value_free)`: Creates a new `c_array_map` instance with the given initial capacity and value-free flag.
- `void c_array_map_init(c_array_map* parr, int capacity, int value_free)`: Initializes an existing `c_array_map` instance.

### Modification

- `void c_array_map_insert(c_array_map* parr, char* key, void* value)`: Inserts a key-value pair into the map.
- `void c_array_map_remove(c_array_map* parr, char* key)`: Removes a key-value pair from the map.

### Access

- `c_array_map_node* c_array_map_find(c_array_map* parr, char* key)`: Finds and returns the node associated with the key.
- `int c_array_map_size(c_array_map* parr)`: Returns the number of entries in the map.

### Management

- `void c_array_map_clear(c_array_map* parr)`: Clears all entries in the map.
- `void c_array_map_destroy(c_array_map* parr)`: Frees the resources associated with the map.
- `void c_array_map_move(c_array_map* dst, c_array_map* src)`: Moves the contents of one map to another.
- `void c_array_map_swap(c_array_map* dst, c_array_map* src)`: Swaps the contents of two maps.

## Example Usage

Here is an example of how to use the `c_array_map`:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "carraymap.h"

int main() {
    int capacity = 100;
    int rand_max = capacity * 2;
    srand((unsigned)time(NULL));

    char temp[64];

    for (int a = 0, b = 10; a < b; a++) {
        c_array_map carr;
        c_array_map_init(&carr, -1, 0);

        for (int i = 0; i < capacity; i++) {
            sprintf(temp, "test%d", rand() % rand_max);
            c_array_map_insert(&carr, temp, (void*)(i * 100));
            printf("%s\n", temp);
        }

        for (int i = 0, j = carr.limit; i < j; i++) {
            if (carr.array[i].value != C_ARRAY_MAP_NODE_DELETED) {
                printf("%d, %s=%ld\n", i, carr.array[i].key, (long)carr.array[i].value);
            }
        }
        printf("====iterate\n");

        for (int i = 0, j = capacity * 2; i < j; i++) {
            sprintf(temp, "test%d", rand() % rand_max);
            c_array_map_node* result = c_array_map_find(&carr, temp);

            if (result != NULL) {
                printf("contains, %s=%ld\n", result->key, (long)result->value);
            } else {
                printf("====not found, %s\n", temp);
            }

            c_array_map_remove(&carr, temp);
        }

        for (int i = 0, j = carr.limit; i < j; i++) {
            if (carr.array[i].value != C_ARRAY_MAP_NODE_DELETED) {
                printf("%d, %s=%ld\n", i, carr.array[i].key, (long)carr.array[i].value);
            }
        }

        printf("====remove\n");

        c_array_map carr2 = {0};
        c_array_map_move(&carr2, &carr);
        printf("====move\n");

        sprintf(temp, "test%d", 777);
        c_array_map_insert(&carr, temp, (void*)777);

        for (int i = 0, j = carr.limit; i < j; i++) {
            if (carr.array[i].value != C_ARRAY_MAP_NODE_DELETED) {
                printf("%d, %s=%ld\n", i, carr.array[i].key, (long)carr.array[i].value);
            }
        }
        printf("====carr, size:%d\n", c_array_map_size(&carr));

        for (int i = 0, j = carr2.limit; i < j; i++) {
            if (carr2.array[i].value != C_ARRAY_MAP_NODE_DELETED) {
                printf("%d, %s=%ld\n", i, carr2.array[i].key, (long)carr2.array[i].value);
            } else {
                printf("deleted, %d\n", i);
            }
        }
        printf("====carr2, size:%d\n", c_array_map_size(&carr2));

        c_array_map_destroy(&carr);
        c_array_map_destroy(&carr2);
    }

    return 0;
}
