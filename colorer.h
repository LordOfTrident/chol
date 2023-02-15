/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define COLORER_IMPLEMENTATION
 *
 * This library provides simple cross-platform (Windows, Unix/Linux) functions to set the outpit
 * color.
 */

/* Simple example of the library:
#include <stdio.h>

#define COLORER_IMPLEMENTATION
#include <colorer.h>

int main(void) {
	color_init();

	color_fg(stdout, COLOR_RED);
	printf("Hello, red!");

	color_reset(stdout);
	printf("\n");
	return 0;
}
*/

#ifndef COLORER_H_HEADER_GUARD
#define COLORER_H_HEADER_GUARD

#ifdef __cplusplus
extern "C" {
#endif

#define COLORER_VERSION_MAJOR 1
#define COLORER_VERSION_MINOR 2
#define COLORER_VERSION_PATCH 0

/*
 * 1.0.0: 16 color macros, resetting color, changing foreground and background
 * 1.1.0: Support C++
 * 1.2.0: Add color_bold() (does not do anything on Windows)
 */

#ifndef WIN32
#	if defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#		define WIN32
#	endif
#endif

#ifdef WIN32
#	include <windows.h>
#else
#	include <stdio.h> /* stdout, stderr, fputs */
#endif

/* Enum values for all colors */
enum {
	COLOR_BLACK = 0,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE,

	COLOR_GREY,
	COLOR_BRED,
	COLOR_BGREEN,
	COLOR_BYELLOW,
	COLOR_BBLUE,
	COLOR_BMAGENTA,
	COLOR_BCYAN,
	COLOR_BWHITE,
};

void color_init(void);
void color_reset(FILE *file);
void color_bold( FILE *file); /* Does not do anything on Windows */

void color_fg(FILE *file, int color);
void color_bg(FILE *file, int color);

/*
 * void color_init(void)
 *     Initialize the colors (needed for Windows).
 *
 * void color_reset(FILE *file)
 *     Set the output color of 'file' to the default.
 *
 * void color_bold( FILE *file)
 *     Make the output of 'file' bold (Unix/Linux only).
 *
 * void color_fg(FILE *file, int color)
 *     Set the foreground color of 'file' to 'color'.

 * void color_bg(FILE *file, int color)
 *     Set the background color of 'file' to 'color'.
 */

#ifdef __cplusplus
}
#endif

#endif

#ifdef COLORER_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
static WORD _fg_colors[] = {
	[COLOR_BLACK]   = 0,
	[COLOR_RED]     = FOREGROUND_RED,
	[COLOR_GREEN]   = FOREGROUND_GREEN,
	[COLOR_YELLOW]  = FOREGROUND_RED | FOREGROUND_GREEN,
	[COLOR_BLUE]    = FOREGROUND_BLUE,
	[COLOR_MAGENTA] = FOREGROUND_RED | FOREGROUND_BLUE,
	[COLOR_CYAN]    = FOREGROUND_GREEN | FOREGROUND_BLUE,
	[COLOR_WHITE]   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

	[COLOR_GREY]     = FOREGROUND_INTENSITY,
	[COLOR_BRED]     = FOREGROUND_RED | FOREGROUND_INTENSITY,
	[COLOR_BGREEN]   = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	[COLOR_BYELLOW]  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	[COLOR_BBLUE]    = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	[COLOR_BMAGENTA] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	[COLOR_BCYAN]    = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	[COLOR_BWHITE]   = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};

static WORD _bg_colors[] = {
	[COLOR_BLACK]   = 0,
	[COLOR_RED]     = BACKGROUND_RED,
	[COLOR_GREEN]   = BACKGROUND_GREEN,
	[COLOR_YELLOW]  = BACKGROUND_RED | BACKGROUND_GREEN,
	[COLOR_BLUE]    = BACKGROUND_BLUE,
	[COLOR_MAGENTA] = BACKGROUND_RED | BACKGROUND_BLUE,
	[COLOR_CYAN]    = BACKGROUND_GREEN | BACKGROUND_BLUE,
	[COLOR_WHITE]   = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,

	[COLOR_GREY]     = BACKGROUND_INTENSITY,
	[COLOR_BRED]     = BACKGROUND_RED | BACKGROUND_INTENSITY,
	[COLOR_BGREEN]   = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
	[COLOR_BYELLOW]  = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
	[COLOR_BBLUE]    = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	[COLOR_BMAGENTA] = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	[COLOR_BCYAN]    = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	[COLOR_BWHITE]   = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
};

static WORD _bg_color = 0;
static WORD _fg_color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

static HANDLE _stdout_handle;
static HANDLE _stderr_handle;

static CONSOLE_SCREEN_BUFFER_INFO _csbi;
#else
static const char *_fg_colors[] = {
	[COLOR_BLACK]   = "\x1b[30m",
	[COLOR_RED]     = "\x1b[31m",
	[COLOR_GREEN]   = "\x1b[32m",
	[COLOR_YELLOW]  = "\x1b[33m",
	[COLOR_BLUE]    = "\x1b[34m",
	[COLOR_MAGENTA] = "\x1b[35m",
	[COLOR_CYAN]    = "\x1b[36m",
	[COLOR_WHITE]   = "\x1b[37m",

	[COLOR_GREY]     = "\x1b[90m",
	[COLOR_BRED]     = "\x1b[91m",
	[COLOR_BGREEN]   = "\x1b[92m",
	[COLOR_BYELLOW]  = "\x1b[93m",
	[COLOR_BBLUE]    = "\x1b[94m",
	[COLOR_BMAGENTA] = "\x1b[95m",
	[COLOR_BCYAN]    = "\x1b[96m",
	[COLOR_BWHITE]   = "\x1b[97m",
};

static const char *_bg_colors[] = {
	[COLOR_BLACK]   = "\x1b[40m",
	[COLOR_RED]     = "\x1b[41m",
	[COLOR_GREEN]   = "\x1b[42m",
	[COLOR_YELLOW]  = "\x1b[43m",
	[COLOR_BLUE]    = "\x1b[44m",
	[COLOR_MAGENTA] = "\x1b[45m",
	[COLOR_CYAN]    = "\x1b[46m",
	[COLOR_WHITE]   = "\x1b[47m",

	[COLOR_GREY]     = "\x1b[100m",
	[COLOR_BRED]     = "\x1b[101m",
	[COLOR_BGREEN]   = "\x1b[102m",
	[COLOR_BYELLOW]  = "\x1b[103m",
	[COLOR_BBLUE]    = "\x1b[104m",
	[COLOR_BMAGENTA] = "\x1b[105m",
	[COLOR_BCYAN]    = "\x1b[106m",
	[COLOR_BWHITE]   = "\x1b[107m",
};
#endif

void color_init(void) {
#ifdef WIN32
	_stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	_stderr_handle = GetStdHandle(STD_ERROR_HANDLE);

	GetConsoleScreenBufferInfo(_stdout_handle, &_csbi);
#endif
}

void color_reset(FILE *file) {
#ifdef WIN32
	HANDLE handle;
	if (file == stderr)
		handle = _stderr_handle;
	else if (file == stdout)
		handle = _stdout_handle;
	else
		return;

	SetConsoleTextAttribute(handle, _csbi.wAttributes);
#else
	if (file != stderr && file != stdout)
		return;

	fputs("\x1b[0m", file);
#endif
}

void color_bold(FILE *file) {
#ifdef WIN32
	(void)file;
#else
	if (file != stderr && file != stdout)
		return;

	fputs("\x1b[1m", file);
#endif
}

void color_fg(FILE *file, int color) {
#ifdef WIN32
	HANDLE handle;
	if (file == stderr)
		handle = GetStdHandle(STD_ERROR_HANDLE);
	else if (file == stdout)
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
	else
		return;

	_fg_color = _fg_colors[color];
	SetConsoleTextAttribute(handle, _fg_color | _bg_color);
#else
	if (file != stderr && file != stdout)
		return;

	fputs(_fg_colors[color], file);
#endif
}

void color_bg(FILE *file, int color) {
#ifdef WIN32
	HANDLE handle;
	if (file == stderr)
		handle = GetStdHandle(STD_ERROR_HANDLE);
	else if (file == stdout)
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
	else
		return;

	_bg_color = _bg_colors[color];
	SetConsoleTextAttribute(handle, _fg_color | _bg_color);
#else
	if (file != stderr && file != stdout)
		return;

	fputs(_bg_colors[color], file);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
