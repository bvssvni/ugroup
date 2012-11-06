
/*
 ugroup - Fast Boolean Algebra on unordered groups
 BSD license.
 by Sven Nilsen, 2012
 http://www.cutoutpro.com
 
 Version: 0.000
 Angular degrees version notation
 http://isprogrammingeasy.blogspot.no/2012/08/angular-degrees-versioning-notation.html
 
 */
/*
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 */

#ifndef UGROUP_GUARD
#define UGROUP_GUARD

#include <stdio.h>
#include <string.h>

#define DECL_UGROUP_TYPE(type) \
typedef struct \
{ \
	type *min; \
	type *max; \
	int n; \
	type *items[]; \
} type##_group;

typedef long long int bignum;

#define UGROUP_TYPE(type) \
type##_group *type##_group_Array(int n, type *arr) \
{ \
	type##_group *g = malloc(sizeof(type##_group)+sizeof(type*)*n); \
	g->n = n; g->min = arr; g->max = arr == NULL ? NULL : arr + n - 1; \
	int i; \
	/* Go backward for cache optimization. */ \
	for (i = n-1; i >= 0; i--) g->items[i] = arr+i; \
	\
	return g; \
} \
\
type##_group *type##_group_Slice(type *arr, int start, int end) \
{ \
	int n = end-start+1; \
	type##_group *g = malloc(sizeof(type##_group)+sizeof(type*)*n); \
	g->n = n; g->min = arr == NULL ? NULL : arr + start; \
	g->max = arr == NULL ? NULL : arr + end; \
	int i; \
	/* Go backward for cache optimization. */ \
	for (i = n; i >= 0; i--) g->items[i] = arr+start+i; \
	\
	return g; \
} \
\
type##_group *type##_group_And(const type##_group *a, const type##_group *b) \
{ \
	const int an = a->n; const int bn = b->n; \
	\
	if (an == 0 || bn == 0) { \
		type##_group *g = malloc(sizeof(type##_group)); \
		memset(g, 0, sizeof(type##_group)); \
		return g; \
	} \
	/* Use intersection of boundaries to save memory. */ \
	type *min = a->min > b->min ? a->min : b->min; \
	type *max = a->max < b->max ? a->max : b->max; \
	\
	int size = (max - min) / sizeof(type) / sizeof(bignum) + 1; \
	\
	/* Set bits from first group. */ \
	bignum arr[size]; \
	memset(arr, 0, size * sizeof(bignum)); \
	int i, block, bit, ind; \
	for (i = an-1; i >= 0; i--) { \
		ind = ((bignum)a->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		bit = ind - block * sizeof(bignum); \
		if (block < 0 || block >= size) continue; \
		\
		arr[block] |= 1ULL << bit; \
	} \
	\
	/* Count the matches. */ \
	int count = 0; \
	for (i = bn-1; i >= 0; i--) { \
		ind = ((bignum)b->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		if (block < 0 || block >= size) continue; \
		\
		bit = ind - block * sizeof(bignum); \
		if ((arr[block] >> bit) & 1) count++; \
	} \
	/* Copy pointers into new group. */ \
	type##_group *res = malloc(sizeof(type##_group)+sizeof(type*)*count); \
	res->min = NULL; res->max = NULL; \
	res->n = count; \
	type *item; \
	int dest = 0; \
	for (i = 0; i < bn; i++) { \
		ind = ((bignum)b->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		if (block < 0 || block >= size) continue; \
		\
		bit = ind - block * sizeof(bignum); \
		if ((arr[block] >> bit) & 1) { \
			item = b->items[i]; \
			res->items[dest++] = item; \
			res->min = res->min == NULL ? item : \
			res->min < item ? res->min : item; \
			res->max = res->max == NULL ? item : \
			res->max > item ? res->max : item; \
		} \
	} \
	\
	return res; \
} \
\
type##_group *type##_group_Except(const type##_group *b, const type##_group *a) \
{ \
	const int an = a->n; const int bn = b->n; \
	\
	if (an == 0 || bn == 0) { \
		type##_group *g = malloc(sizeof(type##_group)); \
		memset(g, 0, sizeof(type##_group)); \
		return g; \
	} \
	\
	/* Use intersection of boundaries to save memory. */ \
	type *min = a->min > b->min ? a->min : b->min; \
	type *max = a->max < b->max ? a->max : b->max; \
	\
	int size = (max - min) / sizeof(type) / sizeof(bignum) + 1; \
	\
	/* Set bits from first group. */ \
	bignum arr[size]; \
	memset(arr, 0, size * sizeof(bignum)); \
	int i, block, bit, ind; \
	for (i = an-1; i >= 0; i--) { \
		ind = ((bignum)a->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		bit = ind - block * sizeof(bignum); \
		if (block < 0 || block >= size) continue; \
		\
		arr[block] |= 1ULL << bit; \
	} \
	\
	/* Count the matches. */ \
	int count = 0; \
	for (i = bn-1; i >= 0; i--) { \
		ind = ((bignum)b->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		if (block < 0 || block >= size) {count++; continue;} \
		\
		bit = ind - block * sizeof(bignum); \
		if (((arr[block] >> bit) & 1) == 0) count++; \
	} \
	\
	/* Copy pointers into new group. */ \
	type##_group *res = malloc(sizeof(type##_group)+sizeof(type*)*count); \
	res->n = count; res->min = NULL; res->max = NULL; \
	type *item; \
	int dest = 0; \
	for (i = 0; i < bn; i++) { \
		ind = ((bignum)b->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		if (block < 0 || block >= size) { \
			item = b->items[i]; \
			res->items[dest++] = item; \
			res->min = res->min == NULL ? item : \
			res->min < item ? res->min : item; \
			res->max = res->max == NULL ? item : \
			res->max > item ? res->max : item; \
			continue; \
		} \
		\
		bit = ind - block * sizeof(bignum); \
		if (((arr[block] >> bit) & 1) == 0) { \
			item = b->items[i]; \
			res->items[dest++] = item; \
			res->min = res->min == NULL ? item : \
			res->min < item ? res->min : item; \
			res->max = res->max == NULL ? item : \
			res->max > item ? res->max : item; \
		} \
	} \
	\
	return res; \
} \
\
type##_group *type##_group_Or(const type##_group *a, const type##_group *b) \
{ \
	const int an = a->n; const int bn = b->n; \
	\
	if (an == 0 && bn == 0) { \
		type##_group *g = malloc(sizeof(type##_group)); \
		memset(g, 0, sizeof(type##_group)); \
		return g; \
	} \
	\
	type *min = a->min == NULL ? b->min : a->min < b->min ? a->min : b->min; \
	type *max = a->max == NULL ? b->max : a->max > b->max ? a->max : b->max; \
	\
	int size = (max - min) / sizeof(type) / sizeof(bignum) + 1; \
	\
	/* Set bits from first group. */ \
	bignum arr[size]; \
	memset(arr, 0, size * sizeof(bignum)); \
	int i, block, bit, ind; \
	for (i = an-1; i >= 0; i--) { \
		ind = ((bignum)a->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		bit = ind - block * sizeof(bignum); \
		arr[block] |= 1ULL << bit; \
	} \
	\
	/* Count the matches. */ \
	int count = an; \
	for (i = bn-1; i >= 0; i--) { \
		ind = ((bignum)b->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		bit = ind - block * sizeof(bignum); \
		if (((arr[block] >> bit) & 0x1) == 0) count++; \
	} \
	\
	/* Copy pointers into new group. */ \
	type##_group *res = malloc(sizeof(type##_group)+sizeof(type*)*count); \
	res->n = count; res->min = a->min; res->max = a->max; \
	memcpy(res->items, a->items, an*sizeof(type*)); \
	type *item; \
	int dest = an; \
	for (i = 0; i < bn; i++) { \
		ind = ((bignum)b->items[i] - (bignum)min) / sizeof(type); \
		block = ind / sizeof(bignum); \
		bit = ind - block * sizeof(bignum); \
		if (((arr[block] >> bit) & 0x1) == 0) { \
			item = b->items[i]; \
			res->items[dest++] = item; \
			res->min = res->min == NULL ? item : \
			res->min < item ? res->min : item; \
			res->max = res->max == NULL ? item : \
			res->max > item ? res->max : item; \
		} \
	} \
	\
	return res; \
}

#endif