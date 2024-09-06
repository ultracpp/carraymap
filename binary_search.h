#ifndef __C_BINARY_SEARCH_H
#define __C_BINARY_SEARCH_H
#ifdef __cplusplus
extern "C"
{
#endif

	int binary_search_int(int* array, int size, int value);
	int binary_search_long(long long* array, int size, long long value);

#ifdef __cplusplus
}
#endif
#endif