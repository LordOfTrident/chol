#include <stdio.h>  /* printf, stderr, fprintf */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */

#define CFS_IMPLEMENTATION
#include <cfs.h>

int main(void) {
	const char *path = "./";

	int status;
	FOREACH_IN_DIR(path, dir, ent, {
		printf("'%s' -> '%s',\n", dir.path, ent.name);
	}, status);

	if (status != 0) {
		fprintf(stderr, "Failed to open directory '%s'\n", path);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
