#pragma once

#include <string.h>
#include <stdlib.h>

// P should point to a structure of the form
// struct {
//	int len;
//  <type> ** v;
// };

typedef int(*ptr_search_fn)(const void *key, const void **obj);
typedef int(*ptr_sort_fn)(const void **a, const void **b);
typedef int(*c_compare_fn)(const void *a, const void *b);

static inline void *ptr_array_search(void **v, int len, ptr_search_fn fn, const void *key) {
	void *res = bsearch(key, v, len, sizeof(void*), (c_compare_fn)fn);
	return res ? *(void**)res : NULL;
}

static inline void ptr_array_sort(void **v, int len, ptr_sort_fn fn) {
	qsort(v, len, sizeof(void*), (c_compare_fn)fn);
}

#define pa_append(P, V)			((P)->v[(P)->len++] = (V))
#define pa_remove(P, IDX)		memmove((void*) &(P)->v[IDX], (void*) &(P)->v[(IDX)+1], sizeof(void*) * (((P)->len--) - (IDX) - 1))
#define pa_sort(P, FN)			ptr_array_sort((void**) (P).v, (P).len, FN)
#define pa_search(P, FN, KEY)	ptr_array_search((void**) (P).v, (P).len, FN, KEY)
