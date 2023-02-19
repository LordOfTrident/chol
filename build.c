#include <string.h> /* strcmp */
#include <malloc.h> /* free */

/* Directories */
#define SRC "examples"
#define BIN "bin"

#define CARGS "-O2", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic", \
              "-Wno-deprecated-declarations", "-I./"

#define CBUILDER_IMPLEMENTATION
#include "cbuilder.h"

char *cc = CC;

void clean(void) {
	bool found = false;
	int  status;
	FOREACH_VISIBLE_IN_DIR(BIN, dir, ent, {
		if (!found)
			found = true;

		char *path = FS_JOIN_PATH(dir.path, ent.name);
		if (path == NULL)
			FATAL_FUNC_FAIL("malloc");

		fs_remove_file(path);
		free(path);
	}, status);

	build_cache_delete();

	if (status != 0)
		LOG_FATAL("Failed to open directory '%s'", BIN);

	if (!found)
		LOG_INFO("Nothing to clean");
	else
		LOG_INFO("Cleaned '%s'", BIN);
}

void build(void) {
	if (!fs_exists(BIN))
		fs_create_dir(BIN);

	build_cache_t c;
	build_cache_load(&c);

	bool nothing_to_compile = true;

	int status;
	FOREACH_VISIBLE_IN_DIR(SRC, dir, ent, {
		char *path = FS_JOIN_PATH(dir.path, ent.name);
		if (path == NULL)
			FATAL_FUNC_FAIL("malloc");

		int status;
		FOREACH_VISIBLE_IN_DIR(path, dir, ent, {
			char *out_name = fs_remove_ext(ent.name);
			char *out      = FS_JOIN_PATH(BIN,  out_name);
			char *src      = FS_JOIN_PATH(path, ent.name);
			if (out_name == NULL || out == NULL || src == NULL)
				FATAL_FUNC_FAIL("malloc");

			free(out_name);

			if (build_cache_update(&c, src)) {
				CMD(cc, src, "-o", out, CARGS);
				if (nothing_to_compile)
					nothing_to_compile = false;

				build_cache_save(&c);
			}

			free(out);
			free(src);
		}, status);

		free(path);

		if (status != 0)
			LOG_FATAL("Failed to open directory '%s/%s'", SRC, ent.name);
	}, status);

	if (status != 0)
		LOG_FATAL("Failed to open directory '%s'", SRC);

	if (nothing_to_compile)
		LOG_INFO("Nothing to build");

	build_cache_free(&c);
}

int main(int argc, const char **argv) {
	args_t a = build_init(argc, argv);
	/* Add the 'clean' subcommand to the usage */
	build_set_usage("[clean] [OPTIONS]");

	flag_cstr(NULL, "CC", "The C compiler path", &cc);

	args_t stripped;
	build_parse_args(&a, &stripped);

	const char *subcmd = args_shift(&stripped);
	if (subcmd != NULL) {
		if (stripped.c > 0) {
			build_arg_error("Unexpected argument '%s' for '%s'", stripped.v[0], subcmd);
			return EXIT_FAILURE;
		}

		if (strcmp(subcmd, "clean") == 0)
			clean();
		else {
			build_arg_error("Unknown subcommand '%s'", subcmd);
			exit(EXIT_FAILURE);
		}
	} else
		build();

	free(stripped.base);
	return EXIT_SUCCESS;
}
