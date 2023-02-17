#include <stdio.h>  /* printf */
#include <stdlib.h> /* free, EXIT_SUCCESS */
#include <assert.h> /* assert */

#define CFS_IMPLEMENTATION
#include <cfs.h>

int main(void) {
	char *path = FS_JOIN_PATH("this", "is", "a", "path");
	if (path == NULL)
		assert(0 && "malloc() fail");

	printf("Path: '%s'\n", path);

	free(path);
	return EXIT_SUCCESS;
}
