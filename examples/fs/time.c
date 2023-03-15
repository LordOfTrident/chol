#include <stdio.h>  /* printf, stderr, fprintf */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */

#define CHOL_FS_IMPLEMENTATION
#include <fs.h>

int main(void) {
	const char *path = "./README";

	int64_t access, modif;
	if (fs_time(path, &modif, &access) != 0) {
		fprintf(stderr, "Failed to get modified and access time of file '%s'\n", path);
		return EXIT_FAILURE;
	}

	printf("Last modified time of '%s': %lu\n", path, (unsigned long)modif);
	printf("Last access time of '%s':   %lu\n", path, (unsigned long)access);

	return EXIT_SUCCESS;
}
