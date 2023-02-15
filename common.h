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

#ifndef COMMON_HEADER_GUARD
#define COMMON_HEADER_GUARD

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, realloc, free */

#define COMMON_VERSION_MAJOR 1
#define COMMON_VERSION_MINOR 0
#define COMMON_VERSION_PATCH 0

/*
 * 1.0.0: Common macros (like TODO, UNUSED, ARRAY_SIZE...) and malloc, realloc and free wrappers
 */

#define UNREACHABLE(MSG)      assert(0 && "Unreachable: "MSG)
#define TODO(MSG)             assert(0 && "TODO: "MSG)
#define FATAL_FUNC_FAIL(FUNC) assert(0 && FUNC"() fail")

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
		for (size_t i = 0; i < SIZE; ++ i) { \
			TYPE *VAR = (ARR)[i]; \
			BODY \
		} \
	} while (0)

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
 */

#define SALLOC(PTR, COUNT) \
	(PTR = salloc(sizeof(*PTR), COUNT))

#define SREALLOC(PTR, COUNT) \
	srealloc(&PTR, sizeof(*PTR), COUNT)

#define SFREE(PTR) \
	sfree(&PTR)

/*
 * Macros SALLOC, SREALLOC and SFREE call functions salloc, srealloc and sfree respectively.
 *
 * SALLOC(PTR, COUNT)
 *     Allocates an array of size 'COUNT' into the pointer 'PTR'. Returns NULL on fail. Example:
 *         | int *numbers;
 *         | if (SALLOC(numbers, 5) == NULL)
 *         |     FATAL_FUNC_FAIL("salloc");
 *         | numbers[0] = 32;
 *
 * SREALLOC(PTR, COUNT)
 *     Reallocates the array pointed to by 'PTR' with new size 'COUNT'. If it succeeded, 'PTR'
 *     points to the reallocated memory and 0 is returned. If it failed, 'PTR' is freed and set to
 *     NULL. Example:
 *         | if (SREALLOC(numbers, 2) != 0)
 *         |     FATAL_FUNC_FAIL("srealloc");
 *
 * SFREE(PTR)
 *     Frees the memory pointer to by 'PTR' and sets 'PTR' to NULL. If 'PTR' is NULL, it fails and
 *     returns a non-zero result. Example:
 *         | if (SFREE(numbers) != 0)
 *         |     UNREACHABLE("'numbers' is NULL");
 */

void *salloc(size_t memb_size, size_t count);
int   srealloc(void **ptr, size_t memb_size, size_t count);
int   sfree(void **ptr);

#ifdef __cplusplus
}
#endif

#endif

#ifdef COMMON_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

void *salloc(size_t memb_size, size_t count) {
	return malloc(memb_size * count);
}

int srealloc(void **ptr, size_t memb_size, size_t count) {
	if (*ptr == NULL)
		return -1;

	void *new = realloc(*ptr, memb_size * count);
	if (new == NULL) {
		sfree(ptr);
		return -1;
	} else
		*ptr = new;

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
#ifndef
