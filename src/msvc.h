#pragma once

#ifdef _WIN32

#include <winsdkver.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN

//#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#define _STDLIB_H		/* imitate glibc (?) include guard */
#include <io.h>			/* read() for the lexers, instead of unistd.h */

/* POSIX name is deprecated, use ISO name instead, yadda yadda yadda */
#define fileno(x) _fileno(x)
#define read(fd, buffer, count) _read(fd, buffer, count)

/* Same where the same function has a different name */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf _snprintf

/* Silence a const-lost warning */
#define YY_USE_CONST

#endif /* _WIN32 */
