/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define CHOL_BUILDER_IMPLEMENTATION
 *
 * This library provides functions to write cross-platform (Windows, Unix/Linux) build files for
 * C/C++ projects. Heavily inspired by https://github.com/tsoding/nobuild
 *
 * Depends on cfs.h, clog.h, ccommon.h and cargs.h
 */

/* Simple example of the library:
#define CHOL_BUILDER_IMPLEMENTATION
#include <chol/builder.h>

char *cc = CC;

int main(int argc, const char **argv) {
	args_t a = build_init(argc, argv);

	flag_cstr(NULL, "CC", "The C compiler path", &cc);
	build_parse_args(&a, NULL);

	CMD(cc, "main.c", "-o", "main");

	return 0;
}
*/

#ifndef CHOL_BUILDER_H_HEADER_GUARD
#define CHOL_BUILDER_H_HEADER_GUARD
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>  /* FILE, stderr, fprintf, fopen, fclose, fgetc, EOF */
#include <stdarg.h> /* va_list, va_start, va_end, vsnprintf */
#include <assert.h> /* assert */
#include <stdlib.h> /* exit, EXIT_FAILURE, EXIT_SUCCESS, malloc, realloc, free, atoll */
#include <string.h> /* strcmp, memcpy */

#include "sys.h"

#include "fs.h"
#include "log.h"
#include "args.h"
#include "common.h"

#define CHOL_BUILDER_VERSION_MAJOR 1
#define CHOL_BUILDER_VERSION_MINOR 8
#define CHOL_BUILDER_VERSION_PATCH 4

/*
 * 1.0.0: Running commands (CMD and COMPILE macros), platform detection, embedding files
 * 1.0.1: Make CMD call LOG_FATAL on non-zero exitcode
 * 1.1.1: Add LOG_FAIL
 * 1.2.1: Add build cache
 * 1.3.1: Add build_clean and build to shorten build.c code
 * 1.3.2: Fix build_clean and build
 * 1.4.2: Remove LOG_FAIL, replace with FATAL_FUNC_FAIL from ccommon.h,
 *        rename build to build_multi_src_app
 * 1.5.2: Add build_cache_update
 * 1.5.3: Fix errors on windows
 * 1.5.4: Fix CMD signature
 * 1.6.4: Add Windows support for cmd
 * 1.7.4: Rename build_multi_src_app to build_app, add build_app_config_t
 * 1.8.4: Added rebuild_all field to build_app_config_t, add build_cache_t optional parameter to
 *        build_app
 */

#if defined(WIN32)
#	define BUILD_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#	define BUILD_PLATFORM_APPLE
#elif defined(__linux__) || defined(__gnu_linux__) || defined(linux)
#	define BUILD_PLATFORM_LINUX
#elif defined(__unix__) || defined(unix)
#	define BUILD_PLATFORM_UNIX
#else
#	define BUILD_PLATFORM_UNKNOWN
#endif

#ifdef BUILD_PLATFORM_WINDOWS
#	define CC  "gcc"
#	define CXX "g++"
#else
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/wait.h>

#	define CC  "cc"
#	define CXX "c++"
#endif

#define BUILD_APP_NAME   "./build"
#define BUILD_CACHE_PATH ".chol_builder_cache"

void build_set_usage(const char *usage);
void build_parse_args(args_t *a, args_t *stripped);

void build_arg_error(const char *fmt, ...);

/*
 * CC
 *     Name of the C compiler (mingw 'gcc' on Windows, 'cc' on Linux/Unix).
 *
 * CXX
 *     Name of the C++ compiler (mingw 'g++' on Windows, 'c++' on Linux/Unix).
 *
 * BUILD_APP_NAME
 *     The expected name of the executable generated from this.
 *
 * BUILD_CACHE_PATH
 *     The path of the build cache file.
 *
 * void build_set_usage(const char *usage)
 *     Set the usage string to 'usage' (used by build_parse_args).
 *
 * void build_parse_args(args_t *a, args_t *stripped)
 *     A wrapper for cargs flags parsing. Parses args 'a' and if 'stripped' is not NULL, it becomes
 *     the stripped arguments. Parsing errors are handled internally with build_arg_error.
 *
 * void build_arg_error(const char *fmt, ...)
 *     Print a command-line arguments related error with format 'fmt' and '...'
 */

typedef struct {
	char   *path;
	int64_t mtime;
} build_cache_item_t;

typedef struct {
	build_cache_item_t *buf;
	size_t              count, size;
} build_cache_t;

int  build_cache_delete(void);
int  build_cache_load(build_cache_t *c);
int  build_cache_save(build_cache_t *c);
void build_cache_free(build_cache_t *c);

bool    build_cache_update(build_cache_t *c, const char *path);
void    build_cache_set(   build_cache_t *c, const char *path, int64_t mtime);
int64_t build_cache_get(   build_cache_t *c, const char *path);

/*
 * build_cache_t
 *     The build cache structure
 *
 * int build_cache_delete(void)
 *     Delete the build cache file. Returns 0 on success.
 *
 * int build_cache_load(build_cache_t *c)
 *     Load the build cache file into 'c'. Returns 0 on success, even if the file does not exist.
 *
 * int build_cache_save(build_cache_t *c)
 *     Save the build cache 'c' into the build cache file. Returns 0 on success.
 *
 * void build_cache_free(build_cache_t *c)
 *     Free the build cache 'c'. Returns 0 on success.
 *
 * bool build_cache_update(build_cache_t *c, const char *path)
 *     Update file 'path' in build cache. Returns true if the file has been modified.
 *
 * void build_cache_set(build_cache_t *c, const char *path, int64_t mtime)
 *     Set the last modified time of item 'path' of build cache 'c' to 'mtime'.
 *
 * int64_t build_cache_get(build_cache_t *c, const char *path)
 *     Return the last modified time of item 'path' from build cache 'c'.
 */

#define CMD(...) \
	do { \
		const char *argv[] = {__VA_ARGS__, NULL}; \
		cmd(argv); \
	} while (0)

#define COMPILE(NAME, SRCS, SRCS_COUNT, ...) \
	do { \
		const char *args[] = {__VA_ARGS__}; \
		compile(NAME, (const char **)SRCS, SRCS_COUNT, args, sizeof(args) / sizeof(args[0])); \
	} while (0)

void cmd(const char **argv);
void compile(const char *compiler, const char **srcs, size_t srcs_count,
             const char **args, size_t args_count);

/*
 * LOG_FAIL(...)
 *     Log an internal failure
 *
 * CMD( ...)
 *     Run a command with arguments '...', where the first argument is the command name.
 *
 * COMPILE(NAME, SRCS, SRCS_COUNT, ...)
 *     Run the command 'NAME' and pass in an array of parameters 'SRCS' with size 'SRCS_COUNT'
 *     along with arguments '...'. This function is made for compiling multiple files with the
 *     C/C++ compiler when they are not constant (in an array)
 *
 * The cmd and compile functions are what CMD and COMPILE respectively run. The functions take
 * arrays for arguments, so the macros exist to make the arrays for you and pass them in.
 */

enum {
	STRING_ARRAY = 0,
	BYTE_ARRAY,
};

void embed(const char *path, const char *out, int type);

void build_clean(const char *path);

typedef struct {
	const char *src_ext, *header_ext;

	const char *bin, *out;

	const char **srcs;
	size_t       srcs_count;

	bool rebuild_all;
} build_app_config_t;

void build_app(const char *compiler, build_app_config_t *config);

/*
 * build_app_config_t
 *     Config structure for the build_app function
 *
 *     const char *src_ext
 *         Extension of a source file
 *     const char *header_ext
 *         Extension of a header file
 *     const char *bin
 *         The binary output directory
 *     const char *out
 *         The app output path
 *     const char **srcs
 *         Source files to compile
 *     size_t srcs_count
 *         Count of elements in 'srcs'
 *     bool rebuild_all
 *         Rebuild all source files
 *
 * STRING_ARRAY
 *     Embed file as a string array (const char*[])
 *
 * BYTE_ARRAY
 *     Embed file as a byte array (unsigned char[])
 *
 * void embed(const char *path, const char *out, int type)
 *     Embed file 'path' as 'type' into file 'out'. This file is then to be included from the C
 *     source code with a macro ('EMBED_NAME') to set the variables name. Example of including
 *     from C code:
 *         | #define EMBED_NAME my_embed
 *         | #include "my_embed.h"
 *
 * void build_clean(const char *path)
 *     Function for common cleaning functionality. Cleans all .o files from directory 'path'.
 *
 * void build_app(const char *compiler, build_app_config_t *config, build_cache_t *c)
 *     A wrapper to provide common build.c functionality in a single function call. Example:
 *         | const char *srcs[] = {"main.c", "test.c"};
 *         | build_app_config_t config = {
 *         |     .src_ext = "c", .header_ext = "h",
 *         |     .bin = BIN, .out = BIN"/"OUT,
 *         |     .srcs = srcs, .srcs_count = sizeof(srcs) / sizeof(srcs[0]),
 *         | };
 *         | built_app(cc, &config, NULL);
 *
 *     This function also takes "extra parameters" from the 'CARGS' and 'CLIBS' macros. 'CARGS' are
 *     the extra arguments to run on compilation, and 'CLIBS' are the library linking arguments.
 *     To use these "extra parameters", simply define the 'CARGS' and 'CLIBS' macros. If they
 *     arent defined, they will be defined as empty macros.
 */

#ifdef __cplusplus
}
#endif
#endif

#ifdef CHOL_BUILDER_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

#define CHOL_FS_IMPLEMENTATION
#include "fs.h"

#define CHOL_LOG_IMPLEMENTATION
#include "log.h"

#define CHOL_ARGS_IMPLEMENTATION
#include "args.h"

#define CHOL_COMMON_IMPLEMENTATION
#include "common.h"

static bool _build_help = false;
static bool _build_ver  = false;

static const char *_build_usage = "[OPTIONS]";

args_t build_init(int argc, const char **argv) {
	args_t a = new_args(argc, argv);
	args_shift(&a);

	flag_bool("h", "help",    "Show the usage",   &_build_help);
	flag_bool("v", "version", "Show the version", &_build_ver);

	log_set_flags(LOG_TIME);

	return a;
}

void build_arg_error(const char *fmt, ...) {
	char    msg[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	fprintf(stderr, "Error: %s\n", msg);
	fprintf(stderr, "Try '" BUILD_APP_NAME " -h'\n");
}

void build_set_usage(const char *usage) {
	_build_usage = usage;
}

void build_parse_args(args_t *a, args_t *stripped) {
	int where;
	int err = args_parse_flags(a, &where, stripped);
	if (err != ARG_OK) {
		switch (err) {
		case ARG_OUT_OF_MEM:    FATAL_FUNC_FAIL("malloc");
		case ARG_UNKNOWN:       build_arg_error("Unknown flag '%s'", a->v[where]);            break;
		case ARG_MISSING_VALUE: build_arg_error("Flag '%s' is a missing value", a->v[where]); break;

		default: build_arg_error("Incorrect type for flag '%s'", a->v[where]);
		}

		exit(EXIT_FAILURE);
	}

	if (_build_help) {
		args_print_usage(stdout, BUILD_APP_NAME, _build_usage);
		exit(EXIT_SUCCESS);
	} else if (_build_ver) {
		printf("chol/builder.h v%i.%i.%i\n",
		       CHOL_BUILDER_VERSION_MAJOR, CHOL_BUILDER_VERSION_MINOR, CHOL_BUILDER_VERSION_PATCH);
		exit(EXIT_SUCCESS);
	}
}

void cmd(const char **argv) {
	size_t count     = 0;
	char   buf[1024] = {0};

	bool first = true;
	for (const char **next = argv; *next != NULL; ++ next) {
		if (first)
			first = false;
		else
			strcat(buf, " ");

		strcat(buf, *next);
		++ count;
	}

	LOG_CUSTOM("CMD", "%s", buf);

#ifdef BUILD_PLATFORM_WINDOWS
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	size_t len      = strlen(buf) + count * 2;
	char  *cmd_line = (char*)malloc(len);
	if (cmd_line == NULL)
		FATAL_FUNC_FAIL("malloc");

	memset(cmd_line, 0, len);

	first = true;
	for (const char **next = argv; *next != NULL; ++ next) {
		if (first) {
			strcat(cmd_line, *next);
			first = false;
		} else {
			strcat(cmd_line, " ");
			strcat(cmd_line, "\"");
			strcat(cmd_line, *next);
			strcat(cmd_line, "\"");
		}
	}

	LOG_INFO("%s", cmd_line);

	if (!CreateProcessA(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		LOG_FATAL("Could not execute command '%s' %d", argv[0], GetLastError());

	WaitForSingleObject(pi.hProcess, INFINITE);
	free(cmd_line);

	DWORD status;
	GetExitCodeProcess(pi.hProcess, &status);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (status != 0)
		LOG_FATAL("Command '%s' exited with exitcode '%i'", argv[0], status);
#else
	pid_t pid_ = fork();
	if (pid_ == 0) {
		if (execvp(argv[0], (char**)argv) == -1)
			LOG_FATAL("Could not execute command '%s'", argv[0]);

		exit(EXIT_SUCCESS);
	} else if (pid_ == -1)
		FATAL_FUNC_FAIL("fork");
	else {
		int status;
		pid_ = wait(&status);
		if (status != 0)
			LOG_FATAL("Command '%s' exited with exitcode '%i'", argv[0], status);
	}
#endif
}

void compile(const char *compiler, const char **srcs, size_t srcs_count,
             const char **args, size_t args_count) {
	const char **argv = (const char**)malloc((srcs_count + args_count + 2) * sizeof(*argv));
	if (argv == NULL)
		FATAL_FUNC_FAIL("malloc");

	argv[0] = compiler;
	size_t pos = 1;

	for (size_t i = 0; i < srcs_count; ++ i, ++ pos)
		argv[pos] = srcs[i];

	for (size_t i = 0; i < args_count; ++ i, ++ pos)
		argv[pos] = args[i];

	argv[pos] = NULL;

	cmd(argv);
	free(argv);
}

static void embed_str_arr(FILE *f, FILE *o) {
	fprintf(o, "static const char *EMBED_NAME[] = {\n\t\"");

	int ch;
	while ((ch = fgetc(f)) != EOF) {
		switch (ch) {
		case '\t': fprintf(o, "\\t");  break;
		case '\r': fprintf(o, "\\r");  break;
		case '\v': fprintf(o, "\\v");  break;
		case '\f': fprintf(o, "\\f");  break;
		case '\b': fprintf(o, "\\b");  break;
		case '\0': fprintf(o, "\\0");  break;
		case '"':  fprintf(o, "\\\""); break;
		case '\\': fprintf(o, "\\\\"); break;

		case '\n': {
			int next = fgetc(f);
			if (next != EOF)
				fprintf(o, "\",\n\t\"");

			ungetc(next, f);
		} break;

		default:
			if (ch >= ' ' && ch <= '~')
				fprintf(o, "%c", (char)ch);
			else
				fprintf(o, "\\x%02X", ch);
		}
	}

	fprintf(o, "\",\n};\n#undef EMBED_NAME\n");
}

static void embed_bytes(FILE *f, FILE *o) {
	fprintf(o, "static unsigned char EMBED_NAME[] = {\n");

	int byte;
	for (size_t i = 0; (byte = fgetc(f)) != EOF; ++ i) {
		if (i % 10 == 0) {
			if (i > 0)
				fprintf(o, "\n");

			fprintf(o, "\t");
		}

		fprintf(o, "0x%02X, ", byte);
	}

	fprintf(o, "\n};\n#undef EMBED_NAME\n");
}

void embed(const char *path, const char *out, int type) {
	LOG_CUSTOM("EMBED", "'%s' into '%s'", path, out);

	FILE *f = fopen(path, "r");
	if (f == NULL) {
		LOG_ERROR("Failed to open '%s' for embedding", path);
		return;
	}

	FILE *o = fopen(out, "w");
	if (o == NULL) {
		LOG_ERROR("Failed to open '%s' to embed '%s' into it", out, path);
		return;
	}

	fprintf(o, "/* %s */\n", path);

	if (type == STRING_ARRAY)
		embed_str_arr(f, o);
	else
		embed_bytes(f, o);

	fclose(o);
	fclose(f);
}

static build_cache_item_t *build_cache_add(build_cache_t *c) {
	++ c->count;
	if (c->count >= c->size) {
		c->size *= 2;
		void *ptr = realloc(c->buf, c->size * sizeof(*c->buf));
		if (ptr == NULL) {
			free(c->buf);
			FATAL_FUNC_FAIL("realloc");
		} else
			c->buf = (build_cache_item_t*)ptr;
	}

	return &c->buf[c->count - 1];
}

int build_cache_delete(void) {
	return fs_remove_file(BUILD_CACHE_PATH);
}

int build_cache_load(build_cache_t *c) {
	c->count = 0;
	c->size  = 16;
	c->buf   = (build_cache_item_t*)malloc(c->size * sizeof(*c->buf));
	if (c->buf == NULL)
		FATAL_FUNC_FAIL("malloc");

	/* If the build cache file exists, read it into the build cache structure */
	FILE *f = fopen(BUILD_CACHE_PATH, "r");
	if (f != NULL) {
		char line[PATH_MAX] = {0};
		while (fgets(line, PATH_MAX, f) != NULL) {
			/* First get the file path string */
			size_t line_len = strlen(line);
			if (line[line_len - 1] == '\n')
				line[line_len - 1] = '\0';

			if (line[0] != '"')
				return -1;

			size_t len = 0;
			for (; line[len + 1] != '"'; ++ len) {
				if (line[len + 1] == '\0')
					return -1;
			}

			build_cache_item_t *item = build_cache_add(c);
			item->path = (char*)malloc(len + 1);
			if (item->path == NULL)
				FATAL_FUNC_FAIL("malloc");

			memcpy(item->path, line + 1, len);
			item->path[len] = '\0';

			/* Parse the last modified time */
			char *num   = line + len + 3;
			item->mtime = (int64_t)atoll(num);
		}

		fclose(f);
	}
	return 0;
}

int build_cache_save(build_cache_t *c) {
	FILE *f = fopen(BUILD_CACHE_PATH, "w");
	if (f == NULL)
		return -1;

	for (size_t i = 0; i < c->count; ++ i)
		fprintf(f, "\"%s\" %llu\n", c->buf[i].path, (unsigned long long)c->buf[i].mtime);

	fclose(f);
	return 0;
}

void build_cache_free(build_cache_t *c) {
	for (size_t i = 0; i < c->count; ++ i)
		free(c->buf[i].path);

	free(c->buf);
	c->buf   = NULL;
	c->count = 0;
	c->size  = 0;
}

bool build_cache_update(build_cache_t *c, const char *path) {
	int64_t m_now, m_cached = build_cache_get(c, path);
	fs_time(path, &m_now, NULL);
	if (m_cached != m_now) {
		build_cache_set(c, path, m_now);
		return true;
	} else
		return false;
}

void build_cache_set(build_cache_t *c, const char *path, int64_t mtime) {
	for (size_t i = 0; i < c->count; ++ i) {
		if (strcmp(c->buf[i].path, path) == 0) {
			c->buf[i].mtime = mtime;
			return;
		}
	}

	build_cache_item_t *item = build_cache_add(c);
	item->path = (char*)malloc(strlen(path) + 1);
	if (item->path == NULL)
		FATAL_FUNC_FAIL("malloc");

	strcpy(item->path, path);
	item->mtime = mtime;
}

int64_t build_cache_get(build_cache_t *c, const char *path) {
	for (size_t i = 0; i < c->count; ++ i) {
		if (strcmp(c->buf[i].path, path) == 0)
			return c->buf[i].mtime;
	}

	return (int64_t)-1;
}

void build_clean(const char *path) {
	bool found = false;
	int  status;
	FOREACH_IN_DIR(path, dir, ent, {
		if (strcmp(fs_ext(ent.name), "o") != 0)
			continue;

		if (!found)
			found = true;

		char *path = FS_JOIN_PATH(dir.path, ent.name);
		if (path == NULL)
			FATAL_FUNC_FAIL("malloc");

		fs_remove_file(path);
		free(path);
	}, status);

	if (status != 0)
		LOG_FATAL("Failed to open directory '%s'", path);

	build_cache_delete();

	if (!found)
		LOG_INFO("Nothing to clean");
	else
		LOG_INFO("Cleaned '%s'", path);
}

/* Macros that have to be defined for this function */
#ifndef CARGS
#	define CARGS
#endif

#ifndef CLIBS
#	define CLIBS
#endif

static char *build_file(const char *cc, build_cache_t *c, const char *out_dir, const char *src_dir,
                        const char *src_name, bool force_rebuild) {
	/* Get the object file and source file paths */
	char *out_name = fs_replace_ext(src_name, "o");
	if (out_name == NULL)
		FATAL_FUNC_FAIL("malloc");

	char *out = FS_JOIN_PATH(out_dir, out_name);
	char *src = FS_JOIN_PATH(src_dir, src_name);
	if (out_name == NULL || out == NULL || src == NULL)
		FATAL_FUNC_FAIL("malloc");

	free(out_name);

	/* Compile if the file needs to be compiled */
	int64_t m_cached = build_cache_get(c, src);
	int64_t m_now;
	if (fs_time(src, &m_now, NULL) != 0)
		LOG_FATAL("Could not get last modified time of '%s'", src);

	if (m_cached != m_now || force_rebuild) {
		build_cache_set(c, src, m_now);
		CMD(cc, "-c", src, "-o", out, CARGS);
	}

	free(src);
	return out;
}

void build_app(const char *compiler, build_app_config_t *config, build_cache_t *c) {
	if (!fs_exists(config->bin))
		fs_create_dir(config->bin);

	char  *o_files[128];
	size_t o_files_count = 0;

	build_cache_t c_;
	bool create_build_cache_struct = c == NULL;
	if (create_build_cache_struct) {
		if (build_cache_load(&c_) != 0)
			LOG_FATAL("Build cache is corrupted");
		c = &c_;
	}

	bool rebuild_all = config->rebuild_all;

	/* Compile files in all source directories */
	for (size_t i = 0; i < config->srcs_count; ++ i) {
		int status;

		/* If a header file was modified, everything needs to be rebuilt. Check for that here */
		FOREACH_IN_DIR(config->srcs[i], dir, ent, {
			if (strcmp(fs_ext(ent.name), config->header_ext) != 0)
				continue;

			char *src = FS_JOIN_PATH(config->srcs[i], ent.name);
			if (src == NULL)
				FATAL_FUNC_FAIL("malloc");

			int64_t m_cached = build_cache_get(c, src);
			int64_t m_now;
			if (fs_time(src, &m_now, NULL) != 0)
				LOG_FATAL("Could not get last modified time of '%s'", src);

			if (m_cached != m_now) {
				if (!rebuild_all)
					rebuild_all = true;

				build_cache_set(c, src, m_now);
			}

			free(src);
		}, status);

		if (status != 0)
			LOG_FATAL("Failed to open directory '%s'", config->srcs[i]);

		/* Rebuild source files */
		FOREACH_IN_DIR(config->srcs[i], dir, ent, {
			if (strcmp(fs_ext(ent.name), config->src_ext) != 0)
				continue;

			assert(o_files_count < sizeof(o_files) / sizeof(o_files[0]));

			char *out = build_file(compiler, c, config->bin, config->srcs[i],
			                       ent.name, rebuild_all);
			o_files[o_files_count ++] = out;
		}, status);

		if (status != 0)
			LOG_FATAL("Failed to open directory '%s'", config->srcs[i]);
	}

	if (o_files_count == 0)
		LOG_INFO("Nothing to rebuild");
	else {
		if (build_cache_save(c) != 0)
			LOG_FATAL("Failed to save build cache");

		COMPILE(compiler, o_files, o_files_count, "-o", config->out, CARGS, CLIBS);

		for (size_t i = 0; i < o_files_count; ++ i)
			free(o_files[i]);
	}

	if (create_build_cache_struct)
		build_cache_free(c);
}

#ifdef __cplusplus
}
#endif
#endif
