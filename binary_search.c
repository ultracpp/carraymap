#include "binary_search.h"

int binary_search_int(int* array, int size, int value)
{
	int lo = 0;
	int hi = size - 1;

	int mid = 2147483647;
	int midVal = 2147483647;

	while (lo <= hi)
	{
		mid = ((unsigned int)lo + (unsigned int)hi) >> 1;
		midVal = array[mid];

		if (midVal < value)
		{
			lo = mid + 1;
		} else if (midVal > value)
		{
			hi = mid - 1;
		} else
		{
			return mid;
		}
	}
	return ~lo;
}

int binary_search_long(long long* array, int size, long long value)
{
	int lo = 0;
	int hi = size - 1;

	int mid = 2147483647;
	long long midVal = 9223372036854775807;

	while (lo <= hi) {
		mid = ((unsigned int)lo + (unsigned int)hi) >> 1;
		midVal = array[mid];

		if (midVal < value)
		{
			lo = mid + 1;
		} else if (midVal > value)
		{
			hi = mid - 1;
		} else
		{
			return mid;
		}
	}
	return ~lo;
}
