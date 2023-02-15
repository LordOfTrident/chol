/*
 * This library is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This is an STB-style library, so to include the library implementation, you need to define
 * the implementation macro:
 *     #define CSV_IMPLEMENTATION
 *
 * This library provides C++ std::string_view-like structure and functions to operate on it.
 * (https://en.cppreference.com/w/cpp/string/basic_string_view)
 */

/* Simple example of the library:
#include <stdio.h>

#define CSV_IMPLEMENTATION
#include <csv.h>

int main(void) {
	sv_t sv = sv_cstr("aabaacbaHelloabacb");
	sv = sv_trim(sv, "abc");

	printf(SV_FMT", world!\n", SV_ARG(sv));
	return 0;
}
*/

#ifndef CSV_HEADER_GUARD
#define CSV_HEADER_GUARD

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>  /* strlen */
#include <stdbool.h> /* bool, true, false */

#define CSV_VERSION_MAJOR 1
#define CSV_VERSION_MINOR 1
#define CSV_VERSION_PATCH 1

/*
 * 1.0.0: string view structure, basic functions for trimming, finding, substring...
 * 1.1.0: Support C++
 * 1.1.1: Fix has_prefix, has_suffix
 */

typedef struct {
	size_t      len;
	const char *cstr;
} sv_t;

#define SV_FMT     "%.*s"
#define SV_ARG(SV) (int)(SV).len, (SV).cstr

#define SV_NULL (sv_t){.cstr = NULL}
#define SV_NPOS (size_t)-1

#define SV_WHITESPACES " \f\n\r\t\v"

/*
 * sv_t
 *     The string view structure
 *
 *     size_t len
 *         Length of the string
 *     const char *cstr
 *         The string pointer (Possibly not properly NULL terminated)
 *
 * SV_FMT
 *     The sv_t format for formatting functions (like printf).
 *
 * SV_ARG(SV)
 *     Macro for passing a string view 'SV' into formatting functions (like printf) as arguments.
 *     Example:
 *         | sv_t sv = sv_cstr("Hello");
 *         | printf(SV_FMT"!\n", SV_ARG(sv));
 *
 * SV_NULL
 *     NULL string view.
 *
 * SV_NPOS
 *     std::string::npos-like value.
 *
 * SV_WHITESPACES
 *     String of whitespace characters.
 */

sv_t sv_new( const char *cstr, size_t len) {return (sv_t){.len = len, .cstr = cstr};}
sv_t sv_cstr(const char *cstr)             {return sv_new(cstr, strlen(cstr));}

char sv_at(     sv_t sv, size_t idx) {return sv.cstr[idx];}
bool sv_is_null(sv_t sv)             {return sv.cstr == NULL;}

bool sv_is_equal(sv_t sv, sv_t to);

/*
 * sv_t sv_new(const char *cstr, size_t len)
 *     Construct a new sv_t with string pointer 'cstr' and length 'len'.
 *
 * sv_t sv_cstr(const char *cstr)
 *     Construct a new sv_t from C string (NULL terminated) 'cstr'.
 *
 * char sv_at(sv_t sv, size_t idx)
 *     Returns the character of string 'sv' at index 'idx'.
 *
 * bool sv_is_null(sv_t sv)
 *     Returns true if 'sv' is SV_NULL.
 *
 * bool sv_is_equal(sv_t sv, sv_t to)
 *     Returns true if 'sv' is equal to 'to'.
 */

bool sv_has_prefix(sv_t sv, sv_t prefix);
bool sv_has_suffix(sv_t sv, sv_t suffix);

sv_t sv_substr(    sv_t sv, size_t start, size_t len);
sv_t sv_trim_front(sv_t sv, const char *chs);
sv_t sv_trim_back( sv_t sv, const char *chs);
sv_t sv_trim(      sv_t sv, const char *chs);

/*
 * bool sv_has_prefix(sv_t sv, sv_t prefix)
 *     Return true if 'sv' begins with 'prefix'.
 *
 * bool sv_has_suffix(sv_t sv, sv_t suffix)
 *     Return true if 'sv' ends with 'suffix'.
 *
 * sv_t sv_substr(sv_t sv, size_t start, size_t len)
 *     Return the substring of 'sv' that starts at index 'start' and has length of 'len'.
 *
 * sv_t sv_trim_front(sv_t sv, const char *chs)
 *     Trim all characters from the front of 'sv' that are in 'chs'.
 *
 * sv_t sv_trim_back(sv_t sv, const char *chs)
 *     Trim all characters from the back of 'sv' that are in 'chs'.
 *
 * sv_t sv_trim(sv_t sv, const char *chs)
 *     Trim all characters from the front and back of 'sv' that are in 'chs'.
 */

bool   sv_contains(      sv_t sv, char ch);
size_t sv_find_first(    sv_t sv, char ch);
size_t sv_find_last(     sv_t sv, char ch);
size_t sv_find_first_not(sv_t sv, char ch);
size_t sv_find_last_not( sv_t sv, char ch);

bool   sv_contains_substr(sv_t sv, sv_t substr);
size_t sv_find_substr(    sv_t sv, sv_t substr);

/*
 * bool sv_contains(sv_t sv, char ch)
 *     Return true if 'sv' contains at least one char 'ch'.
 *
 * size_t sv_find_first(sv_t sv, char ch)
 *     Return the index of the first char 'ch' in 'sv'. On failure returns SV_NPOS.
 *
 * size_t sv_find_last(sv_t sv, char ch)
 *     Return the index of the last char 'ch' in 'sv'. On failure returns SV_NPOS.
 *
 * size_t sv_find_first_not(sv_t sv, char ch)
 *     Return the index of the first char that is not 'ch' in 'sv'. On failure returns SV_NPOS.
 *
 * size_t sv_find_last_not(sv_t sv, char ch)
 *     Return the index of the last char that is not 'ch' in 'sv'. On failure returns SV_NPOS.
 *
 * bool sv_contains_substr(sv_t sv, sv_t substr)
 *     Return true if 'sv' contains the substring 'substr'
 *
 * size_t sv_find_substr(sv_t sv, sv_t substr)
 *     Return the index of 'substr' substring in 'sv'. On failure returns SV_NPOS.
 */

#ifdef __cplusplus
}
#endif

#endif

#ifdef CSV_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

bool sv_is_equal(sv_t sv, sv_t to) {
	size_t len = sv.len > to.len? sv.len : to.len;
	for (size_t i = 0; i < len; ++ i) {
		if (sv_at(sv, i) != sv_at(to, i))
			return false;
	}

	return true;
}

bool sv_has_prefix(sv_t sv, sv_t prefix) {
	if (sv.len < prefix.len)
		return false;

	for (size_t i = 0; i < prefix.len; ++ i) {
		if (sv_at(sv, i) != sv_at(prefix, i))
			return false;
	}

	return true;
}

bool sv_has_suffix(sv_t sv, sv_t suffix) {
	if (sv.len < suffix.len)
		return false;

	for (size_t i = sv.len - suffix.len; i < sv.len; ++ i) {
		if (sv_at(sv, i) != sv_at(suffix, i))
			return false;
	}

	return true;
}

sv_t sv_substr(sv_t sv, size_t start, size_t len) {
	if (len != SV_NPOS) {
		if (start + len > sv.len)
			return SV_NULL;
	} else if (start > sv.len)
		return SV_NULL;

	sv_t substr  = sv;
	substr.cstr += start;
	substr.len   = len == SV_NPOS? sv.len - start : len;

	return substr;
}

sv_t sv_trim_front(sv_t sv, const char *chs) {
	sv_t matches = sv_cstr(chs);
	sv_t trimmed = sv;
	for (size_t i = 0; i < sv.len; ++ i) {
		if (!sv_contains(matches, sv_at(sv, i)))
			break;

		++ trimmed.cstr;
		-- trimmed.len;
	}

	return trimmed;
}

sv_t sv_trim_back(sv_t sv, const char *chs) {
	sv_t matches = sv_cstr(chs);
	sv_t trimmed = sv;
	for (size_t i = sv.len - 1; i != SV_NPOS; -- i) {
		if (!sv_contains(matches, sv_at(sv, i)))
			break;

		-- trimmed.len;
	}

	return trimmed;
}

sv_t sv_trim(sv_t sv, const char *chs) {
	sv =   sv_trim_front(sv, chs);
	return sv_trim_back( sv, chs);
}

bool sv_contains(sv_t sv, char ch) {
	for (size_t i = 0; i < sv.len; ++ i) {
		if (sv_at(sv, i) == ch)
			return true;
	}

	return false;
}

size_t sv_find_first(sv_t sv, char ch) {
	for (size_t i = 0; i < sv.len; ++ i) {
		if (sv_at(sv, i) == ch)
			return i;
	}

	return SV_NPOS;
}

size_t sv_find_last(sv_t sv, char ch) {
	for (size_t i = sv.len - 1; i != SV_NPOS; -- i) {
		if (sv_at(sv, i) == ch)
			return i;
	}

	return SV_NPOS;
}

size_t sv_find_first_not(sv_t sv, char ch) {
	for (size_t i = 0; i < sv.len; ++ i) {
		if (sv_at(sv, i) != ch)
			return i;
	}

	return SV_NPOS;
}

size_t sv_find_last_not(sv_t sv, char ch) {
	for (size_t i = sv.len - 1; i != SV_NPOS; -- i) {
		if (sv_at(sv, i) != ch)
			return i;
	}

	return SV_NPOS;
}

bool sv_contains_substr(sv_t sv, sv_t substr) {
	if (substr.len > sv.len)
		return false;

	for (size_t i = 0; i < sv.len - (substr.len - 1); ++ i) {
		bool matched = true;
		for (size_t j = 0; j < substr.len; ++ j) {
			if (sv_at(sv, i + j) != sv_at(substr, j)) {
				matched = false;
				break;
			}
		}

		if (matched)
			return true;
	}

	return false;
}


size_t sv_find_substr(sv_t sv, sv_t substr) {
	if (substr.len > sv.len)
		return SV_NPOS;

	for (size_t i = 0; i < sv.len - (substr.len - 1); ++ i) {
		bool matched = true;
		for (size_t j = 0; j < substr.len; ++ j) {
			if (sv_at(sv, i + j) != sv_at(substr, j)) {
				matched = false;
				break;
			}
		}

		if (matched)
			return i;
	}

	return SV_NPOS;
}

#ifdef __cplusplus
}
#endif

#endif
