#include <stdio.h>

#define CHOL_DARRAY_IMPLEMENTATION
#include <darray.h>

int main(void) {
	darray_t nums;
	DARRAY_INIT(&nums, int);

	DARRAY_ADD_LIT(&nums, 5,  int);
	DARRAY_ADD_LIT(&nums, 2,  int);
	DARRAY_ADD_LIT(&nums, 10, int);

	int to_add = 1024;
	DARRAY_ADD(&nums, &to_add);

	printf("nums = {\n");
	FOREACH_IN_DARRAY(&nums, int, num, {
		printf("\t%i,\n", *num);
	});
	printf("}\n");

	size_t idx = 2;
	printf("\nnums[%zu] = %i\n", idx, *DARRAY_AT(&nums, idx, int));

	DARRAY_FREE(&nums);
	return 0;
}
