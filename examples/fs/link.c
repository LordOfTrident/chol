#include <stdio.h>  /* printf, fprintf, stderr */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */

#define CHOL_FS_IMPLEMENTATION
#include <fs.h>

int main(void) {
	const char *link   = "./mylink";
	const char *target = "./non_existant_file.txt";

	/* on Windows, fs_create_link only works with administrator privileges.
	   If you dont have admin, the link will not be created but the function will
	   return a success exit code (blame the Windows API for this), so you have to
	   use fs_exists and fs_attr to check if it was created. */
	if (fs_create_link(link, target, false) != 0) {
		fprintf(stderr, "Failed to create link '%s' pointing to '%s'\n", link, target);
		return EXIT_FAILURE;
	} else if (!fs_exists(link) || !(fs_attr(link) & FS_LINK)) {
		fprintf(stderr, "Link creation requires admin privileges on windows\n");
		return EXIT_FAILURE;
	}
	printf("Created link '%s' pointing to '%s'\n", link, target);

	char   buf[PATH_MAX] = {0}; /* link target path */
	size_t written       = 0;   /* length of the link target path. If you dont need it, pass NULL */
	if (fs_read_link(link, buf, sizeof(buf), &written) != 0) {
		fprintf(stderr, "Failed to read what link '%s' points to\n", link);
		return EXIT_FAILURE;
	}
	printf("Read %i bytes\n", (int)written);
	printf("Link '%s' points to '%s'\n", link, target);

	if (fs_remove_file(link) != 0) {
		fprintf(stderr, "Failed to delete link '%s'\n", link);
		return EXIT_FAILURE;
	}
	printf("Deleted link '%s'\n", link);

	return EXIT_SUCCESS;
}
