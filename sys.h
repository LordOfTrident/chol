/*
 * This header is a part of the C header-only libraries collection
 * (https://github.com/LordOfTrident/chol)
 *
 * This header defines some version macros to make specific functions available, and includes
 * the header 'unistd.h' on Unix/Linux and 'windows.h' on Windows.
 *
 * Projects using any of the libraries here should include this header instead of 'windows.h' and
 * 'unistd.h', otherwise the libraries might give errors.
 */

#ifndef CHOL_SYS_H_HEADER_GUARD
#define CHOL_SYS_H_HEADER_GUARD

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#	if defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#		define WIN32
#	endif
#endif

#ifdef WIN32
/* WINVER and _WIN32_WINNT have to be greater or equal to 0x0600 for
   CreateSymbolicLinkA to work on MinGW */
#	ifdef WINVER
#		if WINVER < 0x0600
#			undef  WINVER
#			define WINVER 0x0600
#		endif
#	endif

#	ifdef _WIN32_WINNT
#		if _WIN32_WINNT < 0x0600
#			undef  _WIN32_WINNT
#			define _WIN32_WINNT 0x0600
#		endif
#	endif

#	include <windows.h>
#else
#	ifndef __USE_XOPEN_EXTENDED
#		define __USE_XOPEN_EXTENDED
#	endif

#	ifdef _POSIX_C_SOURCE
#		if _POSIX_C_SOURCE < 200112L
#			undef  _POSIX_C_SOURCE
#			define _POSIX_C_SOURCE 200112L
#		endif
#	endif

#	ifdef _XOPEN_SOURCE
#		if _XOPEN_SOURCE < 500
#			undef  _XOPEN_SOURCE
#			define _XOPEN_SOURCE 500
#		endif
#	endif

#	include <unistd.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
