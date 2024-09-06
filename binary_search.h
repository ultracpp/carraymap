/*
 * carraymap - dynamic array-based hash map implementation in C
 * Copyright (c) 2024 Eungsuk Jeon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
