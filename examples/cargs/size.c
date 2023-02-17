#include <stdio.h>  /* printf, fprintf, stderr */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE*/

/* cargs is an STB-style library, so CARGS_IMPLEMENTATION has to be defined for the implementation
   to be included */
#define CARGS_IMPLEMENTATION
#include <cargs.h>

int main(int argc, const char **argv) {
	args_t a = new_args(argc, argv);       /* the command line arguments structure */
	const char *app_name = args_shift(&a); /* remove the app name argument and save it */

	bool version = false; /* the values assigned to flag variables are the defaults */
	bool help    = false;
	flag_bool("v", "version", "Show the version", &version);
	flag_bool("h", "help",    "Show the usage",   &help);

	size_t size = 5;
	flag_size("s", "size", "Change the size", &size);

	int    where;    /* if you dont care about the error location, pass in NULL */
	args_t stripped; /* to not allocate the stripped arguments, pass in NULL */
	int    err = args_parse_flags(&a, &where, &stripped); /* parse the flags */
	if (err != ARG_OK) {
		switch (err) {
		case ARG_OUT_OF_MEM:    assert(0 && "malloc() fail");
		case ARG_UNKNOWN:       fprintf(stderr, "Error: Unknown flag '%s'\n", a.v[where]);  break;
		case ARG_MISSING_VALUE: fprintf(stderr, "Error: '%s' missing value\n", a.v[where]); break;

		/* there are other specific type errors that you can handle */

		default: fprintf(stderr, "Error: Incorrect type for flag '%s'\n", a.v[where]);
		}

		free(stripped.base); /* the stripped arguments array is allocated on the heap, so it has to
		                        be freed */
		return EXIT_FAILURE;
	}

	if (help) {
		/* cargs can automatically generate the usage, or you can print your own and
		   use args_print_flags() to print all the flags */
		args_print_usage(stdout, app_name, "[OPTIONS]");
		free(stripped.base);
		return EXIT_SUCCESS;
	} else if (version) {
		printf("version\n");
		free(stripped.base);
		return EXIT_SUCCESS;
	}

	/* Print all the non-flag arguments */
	FOREACH_IN_ARGS(stripped, arg, {
		printf("'%s',\n", arg);
	});

	printf("%zu\n", size);
	for (size_t i = 0; i < size; ++ i)
		putchar('|');
	printf("\n");

	free(stripped.base);
	return EXIT_SUCCESS;
}
