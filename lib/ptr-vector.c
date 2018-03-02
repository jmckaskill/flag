#include <ptr-vector.h>

#include <stdlib.h>

void pv_append(ptr_vector *v, void *value) {
	if (v->len + 1 >= v->cap) {
		v->cap = (v->cap + 16) * 3 / 2;
		v->v = (void**)realloc(v->v, v->cap * sizeof(void*));
	}
	v->v[v->len++] = value;
}

void pv_free(ptr_vector *v) {
	free(v->v);
}
