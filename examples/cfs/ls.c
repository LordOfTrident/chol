#include <stdio.h>  /* printf, fprintf, stderr */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */

#define CFS_IMPLEMENTATION
#include <cfs.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "List all files in a directory\nUsage: %s PATH\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *path = argv[1];
	if (!fs_exists(path)) {
		fprintf(stderr, "Error: directory '%s' does not exist\n", path);
		return EXIT_FAILURE;
	}

	fs_dir_t dir;
	if (fs_dir_open(&dir, path) != 0) {
		fprintf(stderr, "Error: could not open dir '%s'\n", path);
		return EXIT_FAILURE;
	}

	fs_ent_t ent; /* the directory file entry structure */
	while (fs_dir_next(&dir, &ent) == 0) {
		printf("'%s': ", ent.name); /* name of the file (not path) */

		if (ent.attr == FS_REGULAR) /* is it a regular file? */
			printf("regular, ");
		else {
			if (ent.attr & FS_HIDDEN) /* is it hidden? */
				printf("hidden, ");
			if (ent.attr & FS_DIR) /* is it a dir? */
				printf("dir, ");
			if (ent.attr & FS_LINK) /* is it a link? */
				printf("link, ");
		}

		printf("\n");
	}

	fs_dir_close(&dir);
	return EXIT_SUCCESS;
}
