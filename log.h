/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define CHOL_LOG_IMPLEMENTATION
 *
 * This library provides simple cross-platform (Windows, Unix/Linux) logging macros/functions.
 */

/* Simple example of the library:
#define CHOL_LOG_IMPLEMENTATION
#include <chol/log.h>

int main(void) {
	LOG_INFO("Hello, world!");
	LOG_WARN("I am a warning");
	LOG_ERROR("Something bad just happened!");
	LOG_FATAL("Something very bad happened, exiting...");

	return 0;
}
*/

#ifndef CHOL_LOG_H_HEADER_GUARD
#define CHOL_LOG_H_HEADER_GUARD
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>   /* FILE, fprintf, stderr */
#include <stdlib.h>  /* exit, EXIT_FAILURE */
#include <time.h>    /* time_t, time, localtime */
#include <stdarg.h>  /* va_list, va_start, va_end, vsnprintf */
#include <stdbool.h> /* bool, true, false */

#define CHOL_LOG_VERSION_MAJOR 1
#define CHOL_LOG_VERSION_MINOR 2
#define CHOL_LOG_VERSION_PATCH 1

/*
 * 1.0.0: Basic logging functions LOG_INFO, LOG_WARN, LOG_ERROR and LOG_FATAL
 * 1.1.0: Add LOG_CUSTOM for logging with custom titles
 * 1.2.0: Support C++
 * 1.2.1: Make the time strictly print 2 digits for second, minute and hour
 */

#ifndef WIN32
#	if defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#		define WIN32
#	endif
#endif

#ifdef WIN32
#	include "chol_sys.h"
#endif

enum {
	LOG_NONE = 0,
	LOG_TIME = 1 << 0,
	LOG_LOC  = 1 << 1,
};

void log_into(FILE *file);
void log_set_flags(int flags);

/*
 * LOG_NONE
 *     No log flags
 *
 * LOG_TIME
 *     Flag to include the time in logging output
 *
 * LOG_LOC
 *     Flag to include the source code location in loggign output
 *
 * void log_into(FILE *file)
 *     Output the logs into 'file'. If 'file' is stdout or stderr, the logs are colored.
 *
 * void log_set_flags(int flags)
 *     Set logging flags to 'flags'.
 */

#define LOG_INFO( ...) log_info( __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN( ...) log_warn( __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_error(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_fatal(__FILE__, __LINE__, __VA_ARGS__)

#define LOG_CUSTOM(TITLE, ...) log_custom(TITLE, __FILE__, __LINE__, __VA_ARGS__)

/*
 * LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL and LOG_CUSTOM call log_info, log_warn, log_error,
 * log_fatal and log_custom functions respectively.
 *
 * LOG_CUSTOM(TITLE, ...)
 *     Outputs a custom log with cyan title 'TITLE'. '...' is the formatted output.
 *
 * LOG_INFO(...)
 *     Outputs an info log (cyan). '...' is the formatted output.
 *
 * The other functions are the same as LOG_INFO, except that LOG_WARN outputs a warning (yellow),
 * LOG_ERROR outputs an error (red) and log_fatal outputs a fatal error (magenta) and fail exits
 * the program.
 *
 */

void log_info( const char *path, size_t line, const char *fmt, ...);
void log_warn( const char *path, size_t line, const char *fmt, ...);
void log_error(const char *path, size_t line, const char *fmt, ...);
void log_fatal(const char *path, size_t line, const char *fmt, ...);

void log_custom(const char *title, const char *path, size_t line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif

#ifdef CHOL_LOG_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

enum {
	CHOL_LOG_COLOR_INFO = 0,
	CHOL_LOG_COLOR_WARN,
	CHOL_LOG_COLOR_ERROR,
	CHOL_LOG_COLOR_FATAL,
};

#ifdef WIN32
#	define CHOL_LOG_RESET_COLOR      _log_color_default
#	define CHOL_LOG_TIME_COLOR       FOREGROUND_INTENSITY
#	define CHOL_LOG_HIGHLIGHT_COLOR (CHOL_LOG_RESET_COLOR | FOREGROUND_INTENSITY)
#	define CHOL_LOG_MSG_COLOR       (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

static WORD _log_color_default = CHOL_LOG_MSG_COLOR;

static WORD _log_colors[] = {
	/* CHOL_LOG_COLOR_INFO  */ FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	/* CHOL_LOG_COLOR_WARN  */ FOREGROUND_GREEN | FOREGROUND_RED  | FOREGROUND_INTENSITY,
	/* CHOL_LOG_COLOR_ERROR */ FOREGROUND_RED   | FOREGROUND_INTENSITY,
	/* CHOL_LOG_COLOR_FATAL */ FOREGROUND_RED   | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};
#else
#	define CHOL_LOG_RESET_COLOR     "\x1b[0m"
#	define CHOL_LOG_TIME_COLOR      "\x1b[1;90m"
#	define CHOL_LOG_HIGHLIGHT_COLOR "\x1b[1;97m"
#	define CHOL_LOG_MSG_COLOR       "\x1b[0m"

static const char *_log_colors[] = {
	/* CHOL_LOG_COLOR_INFO  */ "\x1b[1;96m",
	/* CHOL_LOG_COLOR_WARN  */ "\x1b[1;93m",
	/* CHOL_LOG_COLOR_ERROR */ "\x1b[1;91m",
	/* CHOL_LOG_COLOR_FATAL */ "\x1b[1;95m",
};
#endif

static FILE *_log_file  = NULL;
static int   _log_flags = LOG_NONE;

void log_set_flags(int flags) {
	_log_flags = flags;
}

void log_into(FILE *file) {
	_log_file = file;
}

static void log_reset_color(void) {
	if (_log_file != stderr && _log_file != stdout)
		return;

#ifdef WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CHOL_LOG_RESET_COLOR);
#else
	fprintf(_log_file, "%s", CHOL_LOG_RESET_COLOR);
#endif
}

static void log_print_title(int color, const char *title) {
	log_reset_color();

	if (_log_file == stderr || _log_file == stdout)
#ifdef WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _log_colors[color]);
#else
		fprintf(_log_file, "%s", _log_colors[color]);
#endif

	fprintf(_log_file, "[%s]", title);
	log_reset_color();
}

static void log_print_time(void) {
	time_t raw;
	time(&raw);
	struct tm *info = localtime(&raw);

	if (_log_file == stderr || _log_file == stdout)
#ifdef WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CHOL_LOG_TIME_COLOR);
#else
		fprintf(_log_file, "%s", CHOL_LOG_TIME_COLOR);
#endif

	fprintf(_log_file, "%02d:%02d:%02d", info->tm_hour, info->tm_min, info->tm_sec);
	log_reset_color();
}

static void log_print_loc(const char *path, size_t line) {
#ifdef WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CHOL_LOG_HIGHLIGHT_COLOR);
#else
	fprintf(_log_file, "%s", CHOL_LOG_HIGHLIGHT_COLOR);
#endif
	fprintf(_log_file, " %s:%zu:", path, line);
	log_reset_color();
}

static void log_print_msg(const char *msg) {
#ifdef WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CHOL_LOG_MSG_COLOR);
#else
	fprintf(_log_file, "%s", CHOL_LOG_MSG_COLOR);
#endif

	fprintf(_log_file, " %s\n", msg);
	log_reset_color();
}

static void log_template(int color, const char *title, const char *msg,
                         const char *path, size_t line) {
	if (_log_file == NULL)
		_log_file = stderr;

#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	_log_color_default = csbi.wAttributes;
#endif

	if (_log_flags & LOG_TIME) {
		log_print_time();
		fprintf(_log_file, " ");
	}
	log_print_title(color, title);

	if (_log_flags & LOG_LOC)
		log_print_loc(path, line);

	log_print_msg(msg);
}

void log_info(const char *path, size_t line, const char *fmt, ...) {
	char    buf[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	log_template(CHOL_LOG_COLOR_INFO, "INFO", buf, path, line);
}

void log_warn(const char *path, size_t line, const char *fmt, ...) {
	char    buf[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	log_template(CHOL_LOG_COLOR_WARN, "WARN", buf, path, line);
}

void log_error(const char *path, size_t line, const char *fmt, ...) {
	char    buf[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	log_template(CHOL_LOG_COLOR_ERROR, "ERROR", buf, path, line);
}

void log_fatal(const char *path, size_t line, const char *fmt, ...) {
	char    buf[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	log_template(CHOL_LOG_COLOR_FATAL, "FATAL", buf, path, line);
	exit(EXIT_FAILURE);
}

void log_custom(const char *title, const char *path, size_t line, const char *fmt, ...) {
	char    buf[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	log_template(CHOL_LOG_COLOR_INFO, title, buf, path, line);
}

#ifdef __cplusplus
}
#endif
#endif
