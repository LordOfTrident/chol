/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define CARGS_IMPLEMENTATION
 *
 * This library provides functions to easily parse and handle command line arguments and flags.
 */

/* Simple example of the library:
#include <stdio.h>

#define CARGS_IMPLEMENTATION
#include <cargs.h>

int main(int argc, const char **argv) {
	args_t a = new_args(argc, argv);
	const char *app_name = args_shift(&a);

	bool help = false;
	flag_bool("h", "help", "Show the usage", &help);

	args_parse_flags(&a, NULL, NULL);
	if (help) {
		args_print_usage(stdout, app_name, "[OPTIONS]");
		return 0;
	}

	printf("Hello, world!\n");

	return 0;
}
*/

#ifndef CARGS_H_HEADER_GUARD
#define CARGS_H_HEADER_GUARD

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>   /* fprintf, stderr, FILE */
#include <stdlib.h>  /* size_t, malloc, free, strtol, strtoull, strtod */
#include <stdbool.h> /* bool, true, false */
#include <string.h>  /* strcmp, strlen, strdup, strcpy, strncpy */
#include <assert.h>  /* assert */

#define CARGS_VERSION_MAJOR 1
#define CARGS_VERSION_MINOR 2
#define CARGS_VERSION_PATCH 3

/*
 * 1.0.0: Parsing flags, returning stripped arguments
 * 1.0.1: Do not show default string flag value if its NULL
 * 1.1.1: Support C++
 * 1.2.1: Add FOREACH_IN_ARGS macro
 * 1.2.2: Change FOREACH_IN_ARGS iterator variable name from i to _i
 * 1.2.3: Fix get_flag_by_short_name and get_flag_by_long_name
 */

#ifndef CONSTRUCT
#	ifdef __cplusplus
#		define CONSTRUCT(TYPE, ...) TYPE({__VA_ARGS__})
#	else
#		define CONSTRUCT(TYPE, ...) (TYPE){__VA_ARGS__}
#	endif
#endif

#define FOREACH_IN_ARGS(ARGS, ARG_VAR, BODY) \
	do { \
		for (int _i = 0; _i < (ARGS).c; ++ _i) { \
			const char *ARG_VAR = (ARGS).v[_i]; \
			BODY \
		} \
	} while (0)

typedef struct {
	int          c;
	const char **v;
	char       **base;
} args_t;

args_t new_args(int argc, const char **argv);

bool arg_is_flag(     const char *arg);
bool arg_is_flag_long(const char *arg);
bool arg_is_flags_end(const char *arg);

/*
 * args_t
 *     Command-line arguments structure
 *
 *     int c
 *         The count of the arguments
 *     const char **v
 *         Pointer to the arguments list
 *     char **base
 *         Pointer to the base (start) of the arguments list (this exists for freeing the arguments)
 *
 * FOREACH_IN_ARGS(ARGS, ARG_VAR, BODY)
 *     Loops through each argument in 'ARGS'. 'ARG_VAR' is the name of the argument variable, and
 *     'BODY' is the code to run on each argument. Example:
 *         | args_t a = new_args(argc, argv);
 *         | FOREACH_IN_ARGS(a, arg, {
 *         |     printf("%s\n", arg);
 *         | });
 *
 * args_t new_args(int argc, const char **argv)
 *     Constructs an args_t structure from 'argc' and 'argv'.
 *
 * bool arg_is_flag(const char *arg)
 *     Returns true if 'arg' is a flag (prefixed with '-') and its NOT flags end '--'
 *
 * bool arg_is_flag_long(const char *arg)
 *     Returns true if 'arg' is a long flag (prefixed with '--') and its NOT flags end '--'
 *
 * bool arg_is_flags_end(const char *arg)
 *     Returns true if 'arg' is flags end '--'
 */

/* args_parse_flags return values enum */
enum {
	ARG_OK = 0,
	ARG_UNKNOWN,
	ARG_MISSING_VALUE,
	ARG_EXPECTED_CSTR,
	ARG_EXPECTED_CHAR,
	ARG_EXPECTED_INT,
	ARG_EXPECTED_SIZE,
	ARG_EXPECTED_FLOAT,
	ARG_EXPECTED_BOOL,
	ARG_OUT_OF_MEM,
};

const char *args_shift(      args_t *a);
int         args_parse_flags(args_t *a, int *where, args_t *stripped);

/*
 * const char *args_shift(args_t *a)
 *     Shifts the arguments 'a' by removing the first argument and subtracting the count by 1.
 *     Returns the argument that was removed. The original pointer to the entire arguments array is
 *     stored in the 'base' field of args_t.
 *
 * int args_parse_flags(args_t *a, int *where, args_t *stripped)
 *     Parses the flags registered by the flag_* functions in arguments 'a'. On success, 0 is
 *     returned. On failure, if 'where' is not NULL, it will contain the number of the argument on
 *     which the parsing failed, and one of the ARG_* enum values is returned. If 'stripped' is not
 *     NULL, it will contain the stripped arguments (arguments with flags removed).
 */

void flag_cstr( const char *short_name, const char *long_name, const char *desc, char  **var);
void flag_char( const char *short_name, const char *long_name, const char *desc, char   *var);
void flag_int(  const char *short_name, const char *long_name, const char *desc, int    *var);
void flag_size( const char *short_name, const char *long_name, const char *desc, size_t *var);
void flag_float(const char *short_name, const char *long_name, const char *desc, double *var);
void flag_bool( const char *short_name, const char *long_name, const char *desc, bool   *var);

void args_print_flags(FILE *file);
void args_print_usage(FILE *file, const char *app_name, const char *usage);

/*
 * void flag_cstr(const char *short_name, const char *long_name, const char *desc, char **var)
 *     Register a c string flag with short name 'short_name', long name 'long_name' and description
 *     'desc'. 'var' is a pointer to the variable that holds the flag value. The value of the
 *     variable at the time of running this function will be used as the default flag value. If
 *     the flag has no short name or no long name, they can be set to NULL.
 *
 * flag_char, flag_int, flag_size, flag_float and flag_bool all work the same as flag_cstr, but
 * their flag types are char, int, size_t, double and bool respectively.
 *
 * void args_print_flags(FILE *file)
 *     Prints all registered flags into 'file'. The format is
 *     '  -<SHORT_NAME>, --<LONG_NAME>   <DESCRIPTION>', where the spaces before '<DESCRIPTION>' are
 *     extended to align the res of the flags output.
 *
 * void args_print_usage(FILE *file, const char *app_name, const char *usage)
 *     Prints the usage to 'file' in the following format:
 *     'Usage: <app_name> <usage>
 *      Options:
 *      <args_print_flags()>'
 */

#ifdef __cplusplus
}
#endif

#endif

#ifdef CARGS_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

args_t new_args(int argc, const char **argv) {
	return CONSTRUCT(args_t, argc, argv, (char**)argv);
}

bool arg_is_flag(const char *arg) {
	return arg[0] == '-' && !arg_is_flags_end(arg);
}

bool arg_is_flag_long(const char *arg) {
	if (strlen(arg) > 2)
		return arg[0] == '-' && arg[1] == '-';
	else
		return false;
}

bool arg_is_flags_end(const char *arg) {
	return strcmp(arg, "--") == 0;
}

const char *args_shift(args_t *a) {
	if (a->c <= 0)
		return NULL;

	const char *arg = *a->v;

	-- a->c;
	++ a->v;

	return arg;
}

typedef enum {
	FLAG_CSTR = 0,
	FLAG_CHAR,
	FLAG_INT,
	FLAG_SIZE,
	FLAG_FLOAT,
	FLAG_BOOL,
} flag_type_t;

typedef struct {
	flag_type_t  type;

	union {
		char  **cstr;
		char   *ch;
		int    *int_;
		size_t *size;
		double *float_;
		bool   *bool_;
	} as;

	union {
		char  *cstr;
		char   ch;
		int    int_;
		size_t size;
		double float_;
		bool   bool_;
	} def;

	const char *short_name, *long_name, *desc;
} flag_t;

#define FLAGS_CAPACITY 128

flag_t flags[FLAGS_CAPACITY];
size_t flags_count = 0;

static flag_t *get_flag_by_short_name(const char *short_name) {
	if (short_name == NULL)
		return NULL;

	for (size_t i = 0; i < flags_count; ++ i) {
		if (flags[i].short_name == NULL)
			continue;

		if (strcmp(flags[i].short_name, short_name) == 0)
			return &flags[i];
	}

	return NULL;
}

static flag_t *get_flag_by_long_name(const char *long_name) {
	if (long_name == NULL)
		return NULL;

	for (size_t i = 0; i < flags_count; ++ i) {
		if (flags[i].long_name == NULL)
			continue;

		if (strcmp(flags[i].long_name, long_name) == 0)
			return &flags[i];
	}

	return NULL;
}

void flag_cstr(const char *short_name, const char *long_name, const char *desc, char **var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_CSTR;
	flags[flags_count].as.cstr    = var;
	flags[flags_count].def.cstr   = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_char(const char *short_name, const char *long_name, const char *desc, char *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_CHAR;
	flags[flags_count].as.ch      = var;
	flags[flags_count].def.ch     = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_int(const char *short_name, const char *long_name, const char *desc, int *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_INT;
	flags[flags_count].as.int_    = var;
	flags[flags_count].def.int_   = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;

}

void flag_size(const char *short_name, const char *long_name, const char *desc, size_t *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_SIZE;
	flags[flags_count].as.size    = var;
	flags[flags_count].def.size   = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_float(const char *short_name, const char *long_name, const char *desc, double *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_FLOAT;
	flags[flags_count].as.float_  = var;
	flags[flags_count].def.float_ = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

void flag_bool(const char *short_name, const char *long_name, const char *desc, bool *var) {
	assert(flags_count < FLAGS_CAPACITY);

	flags[flags_count].type       = FLAG_BOOL;
	flags[flags_count].as.bool_   = var;
	flags[flags_count].def.bool_  = *var;
	flags[flags_count].short_name = short_name;
	flags[flags_count].long_name  = long_name;
	flags[flags_count].desc       = desc;
	++ flags_count;
}

static int flag_set(flag_t *f, const char *val) {
	switch (f->type) {
	case FLAG_CSTR: {
		*f->as.cstr = (char*)malloc(strlen(val) + 1);
		strcpy(*f->as.cstr, val);
	} break;

	case FLAG_CHAR:
		if (strlen(val) != 1)
			return ARG_EXPECTED_CHAR;

		*f->as.ch = val[0];
		break;

	case FLAG_INT: {
		char *ptr;
		int n = (int)strtol(val, &ptr, 10);
		if (*ptr != '\0')
			return ARG_EXPECTED_INT;

		*f->as.int_ = n;
	} break;

	case FLAG_SIZE: {
		char *ptr;
		size_t n = (size_t)strtoull(val, &ptr, 10);
		if (*ptr != '\0')
			return ARG_EXPECTED_SIZE;

		*f->as.size = n;
	} break;

	case FLAG_FLOAT: {
		char *ptr;
		double n = (double)strtod(val, &ptr);
		if (*ptr != '\0')
			return ARG_EXPECTED_FLOAT;

		*f->as.float_ = n;
	} break;

	case FLAG_BOOL:
		if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0)
			*f->as.bool_ = true;
		else if (strcmp(val, "false") == 0 || strcmp(val, "0") == 0)
			*f->as.bool_ = false;
		else
			return ARG_EXPECTED_BOOL;
	}

	return ARG_OK;
}

int args_parse_flags(args_t *a, int *where, args_t *stripped) {
	/* If stripped args are supposed to be returned, allocate memory for them
	   (allocate the same size as the original arguments so we dont have to do any reallocs) */
	if (stripped != NULL) {
		stripped->c    = 0;
		stripped->base = (char**)malloc(sizeof(*stripped) * a->c);
		if (stripped->base == NULL)
			return ARG_OUT_OF_MEM;

		stripped->v = (const char**)stripped->base;
	}

	bool flags_end = false;
	for (int i = 0; i < a->c; ++ i) {
		if (arg_is_flags_end(a->v[i]) && !flags_end) {
			/* If stripped args arent expected to be returned, we can just return from the
			   function after hitting the end of flag arguments */
			if (stripped == NULL)
				return ARG_OK;
			else {
				/* Otherwise we continue to save all the non-flag args */
				flags_end = true;
				continue;
			}
		} else if (!arg_is_flag(a->v[i]) || flags_end) {
			/* If stripped args are supposed to be returned, save each non-flag arg */
			if (stripped != NULL)
				stripped->v[stripped->c ++] = a->v[i];

			/* Dont parse non-flag arguments */
			continue;
		}

		/* Save the current arg index */
		if (where != NULL)
			*where = i;

		bool is_long = arg_is_flag_long(a->v[i]);
		const char *arg = a->v[i] + is_long + 1;

		/* Find the end of flag name */
		const char *tmp   = arg;
		size_t      count = 0;
		while (*arg != '\0' && *arg != '=') {
			++ count;
			++ arg;
		}

		/* Allocate memory for flag name and copy it there */
		char *name = (char*)malloc(count + 1);
		if (name == NULL)
			return ARG_OUT_OF_MEM;

		strncpy(name, tmp, count);
		name[count] = '\0';

		/* Get the flag pointer */
		flag_t *flag = is_long? get_flag_by_long_name(name) : get_flag_by_short_name(name);
		if (flag == NULL)
			return ARG_UNKNOWN;

		/* If the flag has '=', save the value */
		char *val = NULL;
		if (*arg == '=') {
			++ arg;

			/* Find value end */
			tmp   = arg;
			count = 0;
			while (*arg != '\0') {
				++ count;
				++ arg;
			}

			/* Allocate memory for value and copy it there */
			val = (char*)malloc(count + 1);
			if (val == NULL)
				return ARG_OUT_OF_MEM;

			strncpy(val, tmp, count);
			val[count] = '\0';
		}

		/* If there was no value in the flag: */
		if (val == NULL) {
			/* Set the flag to true if its a boolean flag */
			if (flag->type == FLAG_BOOL)
				*flag->as.bool_ = true;
			else {
				/* Else read the next argument for the value */
				++ i;
				if (i >= a->c)
					return ARG_MISSING_VALUE;

				val = (char*)a->v[i];

				int err = flag_set(flag, val);
				if (err != ARG_OK)
					return err;
			}
		} else {
			/* If there was a value, set the flag value to it */
			int err = flag_set(flag, val);
			if (err != ARG_OK)
				return err;

			free(val);
		}

		free(name);
	}

	return ARG_OK;
}

void args_print_flags(FILE *file) {
	/* Find the offset of the flag descriptions */
	int longest = 0;
	for (size_t i = 0; i < flags_count; ++ i) {
		flag_t *f = &flags[i];

		int len;
		if (f->short_name == NULL)
			len = snprintf(NULL, 0, "  --%s", f->long_name);
		else if (f->long_name == NULL)
			len = snprintf(NULL, 0, "  -%s", f->short_name);
		else
			len = snprintf(NULL, 0, "  -%s, --%s", f->short_name, f->long_name);

		if (len > longest)
			longest = len;
	}

	/* Print all flags */
	for (size_t i = 0; i < flags_count; ++ i) {
		flag_t *f = &flags[i];

		int len;
		if (f->short_name == NULL)
			len = fprintf(file, "  --%s", f->long_name);
		else if (f->long_name == NULL)
			len = fprintf(file, "  -%s", f->short_name);
		else
			len = fprintf(file, "  -%s, --%s", f->short_name, f->long_name);

		for (int i = len; i < longest; ++ i)
			fputc(' ', file);

		fprintf(file, "    %s", f->desc);

		/* If the default value is a false bool or a NULL string, dont print it */
		if ((f->type == FLAG_BOOL && !f->def.bool_) ||
		    (f->type == FLAG_CSTR && f->def.cstr == NULL)) {
			fprintf(file, "\n");
			continue;
		}

		fprintf(file, " (default '");
		switch (f->type) {
		case FLAG_CSTR:  fprintf(file, "%s",  f->def.cstr);   break;
		case FLAG_CHAR:  fprintf(file, "%c",  f->def.ch);     break;
		case FLAG_INT:   fprintf(file, "%i",  f->def.int_);   break;
		case FLAG_SIZE:  fprintf(file, "%zu", f->def.size);   break;
		case FLAG_FLOAT: fprintf(file, "%f",  f->def.float_); break;
		case FLAG_BOOL:  fprintf(file, "true");               break;
		}
		fprintf(file, "')\n");
	}
}

void args_print_usage(FILE *file, const char *app_name, const char *usage) {
	fprintf(file, "Usage: %s %s\nOptions:\n", app_name, usage);
	args_print_flags(file);
}

#ifdef __cplusplus
}
#endif

#endif
