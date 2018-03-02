#pragma once

#include <stdlib.h>

typedef struct ptr_vector ptr_vector;
typedef int(*pv_compare_fn)(const void **a, const void **b);

struct ptr_vector {
	int len;
	int cap;
	void **v;
};

void pv_append(ptr_vector *v, void *value);
void pv_remove(ptr_vector *v, int idx);
void pv_free(ptr_vector *v);
