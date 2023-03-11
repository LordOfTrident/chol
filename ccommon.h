/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define COMMON_IMPLEMENTATION
 *
 * This library provides some common macros/functions that do not really fit into any specific
 * library.
 */

#ifndef CCOMMON_HEADER_GUARD
#define CCOMMON_HEADER_GUARD
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, realloc, free */
#include <string.h> /* memset, strlen, strcpy */

#define CCOMMON_VERSION_MAJOR 1
#define CCOMMON_VERSION_MINOR 0
#define CCOMMON_VERSION_PATCH 2

/*
 * 1.0.0: Common macros (like TODO, UNUSED, ARRAY_SIZE...) and malloc, realloc and free wrappers
 * 1.0.1: Change FOREACH_IN_ARRAY iterator variable name from i to _i
 * 1.0.2: Fix C++ errors
 * 1.1.2: Add ZERO_STRUCT
 * 1.2.2: Add strcpy_to_heap
 */

#define UNREACHABLE(MSG)      assert(0 && "Unreachable: " MSG)
#define TODO(MSG)             assert(0 && "TODO: " MSG)
#define FATAL_FUNC_FAIL(FUNC) assert(0 && FUNC "() fail")

/*
 * These functions fail exit the program with assert.
 *
 * UNREACHABLE(MSG)
 *     This function is used for places that should never be reached. It outputs 'MSG' in assert.
 *
 * TODO(MSG)
 *     This function is used for places/features that are to be done. It outputs 'MSG' in assert.
 *
 * FATAL_FUNC_FAIL(FUNC)
 *     This function is used to panic exit the program because of a failure of functions that
 *     the program cannot recover from, for example a malloc() fail. 'FUNC' is the name of the
 *     function that failed. Example of usage:
 *         | void *ptr = malloc(1);
 *         | if (ptr == NULL)
 *         |     FATAL_FUNC_FAIL("malloc");
 */

#define UNUSED(VAR)      (void)VAR

#define TO_STR(X)  _TO_STR(X)
#define _TO_STR(X) #X

#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define FOREACH_IN_ARRAY(ARR, TYPE, SIZE, VAR, BODY) \
	do { \
		for (size_t _i = 0; _i < SIZE; ++ _i) { \
			TYPE *VAR = &(ARR)[_i]; \
			BODY \
		} \
	} while (0)

#ifndef CONSTRUCT
#	ifdef __cplusplus
#		define CONSTRUCT(TYPE, ...) TYPE({__VA_ARGS__})
#	else
#		define CONSTRUCT(TYPE, ...) (TYPE){__VA_ARGS__}
#	endif
#endif

/*
 * UNUSED(VAR)
 *     Marks the 'VAR' variable as unused to avoid compiler warnings.
 *
 * TO_STR(X)
 *     Turns 'X' into a string during the preprocessor.
 *
 * ARRAY_SIZE(ARR)
 *     Returns the amount of elements in the 'ARR' array.
 *
 * FOREACH_IN_ARRAY(ARR, TYPE, SIZE, VAR, BODY)
 *     Loops through each element in array 'ARR' of size 'SIZE'. 'VAR' is the name of the element
 *     pointer variable, which is of type void* and 'BODY' is the code to run on each element.
 *     Example:
 *         | int numbers[] = {1, 5, 4, 2};
 *         | FOREACH_IN_ARRAY(numbers, int, ARRAY_SIZE(numbers), number, {
 *         |     printf("%i\n", *number);
 *         | });
 *
 * CONSTRUCT(TYPE, ...)
 *     Constructions a structure of type 'TYPE' with fields '...'. 'TYPE' has to be a typedef name,
 *     so 'struct my_struct' is not allowed. Example of usage:
 *         | typedef struct {
 *         |     int first, second;
 *         | } int_pair_t;
 *         |
 *         | int_pair_t new_int_pair(int first, int second) {
 *         |     return CONSTRUCT(int_pair_t, first, second);
 *         | }
 *
 *     This macro exists because C++ does not allow '(TYPE){...}' and C does not know about
 *     'TYPE({...})'.
 */

#define ZERO_STRUCT(STRUCT) memset(&(STRUCT), 0, sizeof(STRUCT))

#define SALLOC(PTR, TYPE, COUNT) (PTR = (TYPE*)salloc(sizeof(TYPE), COUNT))
#define SREALLOC(PTR, COUNT)     srealloc((void**)&(PTR), sizeof(*PTR), COUNT)
#define SFREE(PTR)               sfree((void**)&(PTR))

char *strcpy_to_heap(const char *str);

/*
 * ZERO_STRUCT(STRUCT)
 *     Zero initializes struct variable 'STRUCT'.
 *
 * Macros SALLOC, SREALLOC and SFREE call functions salloc, srealloc and sfree respectively.
 *
 * SALLOC(PTR, COUNT)
 *     Allocates an array of size 'COUNT' into the pointer 'PTR'. Returns NULL on fail. Example:
 *         | int *numbers;
 *         | if (SALLOC(numbers, int, 5) == NULL)
 *         |     FATAL_FUNC_FAIL("SALLOC");
 *         | numbers[0] = 32;
 *
 * SREALLOC(PTR, COUNT)
 *     Reallocates the array pointed to by 'PTR' with new size 'COUNT'. If it succeeded, 'PTR'
 *     points to the reallocated memory and 0 is returned. If it failed, 'PTR' is freed and set to
 *     NULL. Example:
 *         | if (SREALLOC(numbers, 2) != 0)
 *         |     FATAL_FUNC_FAIL("SREALLOC");
 *
 * SFREE(PTR)
 *     Frees the memory pointer to by 'PTR' and sets 'PTR' to NULL. If 'PTR' is NULL, it fails and
 *     returns a non-zero result. Example:
 *         | if (SFREE(numbers) != 0)
 *         |     UNREACHABLE("'numbers' is NULL");
 *
 * char *strcpy_to_heap(const char *str)
 *     Copies the string 'str' into a heap-allocated buffer and returns a pointer to it. On
 *     allocation fail, NULL is returned.
 */

void *salloc(size_t memb_size, size_t count);
int   srealloc(void **ptr, size_t memb_size, size_t count);
int   sfree(void **ptr);

#ifdef __cplusplus
}
#endif
#endif

#ifdef CCOMMON_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

char *strcpy_to_heap(const char *str) {
	char *copy = (char*)malloc(strlen(str) + 1);
	if (copy == NULL)
		return NULL;

	strcpy(copy, str);
	return copy;
}

void *salloc(size_t memb_size, size_t count) {
	return malloc(memb_size * count);
}

int srealloc(void **ptr, size_t memb_size, size_t count) {
	if (*ptr == NULL)
		return -1;

	void *new_ = realloc(*ptr, memb_size * count);
	if (new_ == NULL) {
		sfree(ptr);
		return -1;
	} else
		*ptr = new_;

	return 0;
}

int sfree(void **ptr) {
	if (*ptr == NULL)
		return -1;

	free(*ptr);
	*ptr = NULL;

	return 0;
}

#ifdef __cplusplus
}
#endif
#endif
