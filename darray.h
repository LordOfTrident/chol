/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define CHOL_DARRAY_IMPLEMENTATION
 *
 * This library provides a simple generic dynamic array structure.
 */

/* Simple example of the library:
#include <stdio.h>

#define CHOL_DARRAY_IMPLEMENTATION
#include <chol/darray.h>

int main(void) {
	darray_t nums;
	DARRAY_INIT(&nums, int);

	DARRAY_ADD_LIT(&nums, 5,  int);
	DARRAY_ADD_LIT(&nums, 10, int);
	printf("%i\n", *DARRAY_AT(&nums, 1, int));

	DARRAY_FREE(&nums);
	return 0;
}
*/

#ifndef CHOL_DARRAY_H_HEADER_GUARD
#define CHOL_DARRAY_H_HEADER_GUARD
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h> /* size_t, malloc, realloc, free */
#include <string.h> /* memcpy */

#define CHOL_DARRAY_VERSION_MAJOR 1
#define CHOL_DARRAY_VERSION_MINOR 0
#define CHOL_DARRAY_VERSION_PATCH 0

/*
 * 1.0.0: Dynamic array structure, appending, indexing, foreach...
 */

#ifndef DARRAY_CHUNK_SIZE
#	define DARRAY_CHUNK_SIZE 32
#endif

typedef struct {
	void  *buf;
	size_t count, size, elem_size;
} darray_t;

/*
 * DARRAY_CHUNK_SIZE
 *     The size of a chunk allocated by darray in elements. If not defined before including,
 *     the default is 32.
 *
 *     darray_t
 *         Dynamic array structure
 *
 *         void *buf
 *             The raw buffer
 *         size_t count
 *             Count of elements in the buffer
 *         size_t size
 *             Allocated size of the buffer (in elements)
 *         size_t elem_size
 *             Size of a single element in bytes
 */

#define DARRAY_INIT(D, TYPE) darray_init(D, sizeof(TYPE))
#define DARRAY_FREE(D)       darray_free(D)

#define DARRAY_ADD(D, DATA_PTR)  darray_add(D, (void*)DATA_PTR)
#define DARRAY_AT( D, IDX, TYPE) (TYPE*)darray_at(D, IDX)

#define DARRAY_ADD_LIT(D, LIT, TYPE) \
	do { \
		TYPE data = LIT; \
		DARRAY_ADD(D, &data); \
	} while (0)

#define FOREACH_IN_DARRAY(D, TYPE, VAR, BODY) \
	do { \
		for (size_t _i = 0; _i < (D)->count; ++ _i) { \
			TYPE *VAR = DARRAY_AT(D, _i, TYPE); \
			BODY \
		} \
	} while (0)

/*
 * DARRAY_INIT(D, TYPE)
 *     Initializes 'D' for the type 'TYPE'. Returns 0 on success.
 *
 * DARRAY_FREE(D)
 *     Frees 'D'.
 *
 * DARRAY_ADD(D, DATA_PTR)
 *     Copies data from 'DATA_PTR' into 'D' as a new element (appends).
 *
 * DARRAY_AT(D, IDX, TYPE)
 *     Returns a pointer to the element in 'D' at 'IDX' of type 'TYPE'.
 *
 * DARRAY_ADD_LIT(D, LIT, TYPE)
 *     Calls DARRAY_ADD, but takes a literal 'LIT'. Example:
 *         | DARRAY_ADD_LIT(&my_darray, 5, int);
 *
 * FOREACH_IN_DARRAY(D, TYPE, VAR, BODY)
 *     Loops through each element in 'D'. 'VAR' is the name of the element pointer variable, which
 *     is of type pointer to 'TYPE' and 'BODY' is the code to run on each iteration.
 */

int  darray_init(darray_t *d, size_t elem_size);
void darray_free(darray_t *d);

int   darray_add(darray_t *d, void  *data);
void *darray_at( darray_t *d, size_t idx);

#ifdef __cplusplus
}
#endif
#endif

#ifdef CHOL_DARRAY_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

int darray_init(darray_t *d, size_t elem_size) {
	d->count     = 0;
	d->size      = DARRAY_CHUNK_SIZE;
	d->elem_size = elem_size;
	d->buf       = malloc(d->size * d->elem_size);
	return d->buf == NULL? -1 : 0;
}

void darray_free(darray_t *d) {
	free(d->buf);
	d->count = 0;
	d->size  = 0;
	d->buf   = NULL;
}

int darray_add(darray_t *d, void *data) {
	if (d->count >= d->size) {
		d->size  *= 2;
		void *tmp = realloc(d->buf, d->size);
		if (tmp == NULL) {
			free(d->buf);
			return -1;
		} else
			d->buf = tmp;
	}

	memcpy((void*)((char*)d->buf + d->count * d->elem_size), data, d->elem_size);
	++ d->count;
	return 0;
}

void *darray_at(darray_t *d, size_t idx) {
	if (idx >= d->count)
		return NULL;
	else
		return (void*)((char*)d->buf + idx * d->elem_size);
}

#ifdef __cplusplus
}
#endif
#endif
