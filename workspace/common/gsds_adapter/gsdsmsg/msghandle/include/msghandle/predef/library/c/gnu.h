/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_C_GNU_H
#define MSGHANDLE_PREDEF_LIBRARY_C_GNU_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

#include <msghandle/predef/library/c/_prefix.h>

#if defined(__STDC__)
#include <stddef.h>
#elif defined(__cplusplus)
#include <cstddef>
#endif

/*`
[heading `MSGHANDLE_LIB_C_GNU`]

[@http://en.wikipedia.org/wiki/Glibc GNU glibc] Standard C library.
Version number available as major, and minor.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GLIBC__`] [__predef_detection__]]
    [[`__GNU_LIBRARY__`] [__predef_detection__]]

    [[`__GLIBC__`, `__GLIBC_MINOR__`] [V.R.0]]
    [[`__GNU_LIBRARY__`, `__GNU_LIBRARY_MINOR__`] [V.R.0]]
    ]
 */

#define MSGHANDLE_LIB_C_GNU MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GLIBC__) || defined(__GNU_LIBRARY__)
#   undef MSGHANDLE_LIB_C_GNU
#   if defined(__GLIBC__)
#       define MSGHANDLE_LIB_C_GNU \
            MSGHANDLE_VERSION_NUMBER(__GLIBC__,__GLIBC_MINOR__,0)
#   else
#       define MSGHANDLE_LIB_C_GNU \
            MSGHANDLE_VERSION_NUMBER(__GNU_LIBRARY__,__GNU_LIBRARY_MINOR__,0)
#   endif
#endif

#if MSGHANDLE_LIB_C_GNU
#   define MSGHANDLE_LIB_C_GNU_AVAILABLE
#endif

#define MSGHANDLE_LIB_C_GNU_NAME "GNU"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_C_GNU,MSGHANDLE_LIB_C_GNU_NAME)
