/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_C_UC_H
#define MSGHANDLE_PREDEF_LIBRARY_C_UC_H

#include <msghandle/predef/library/c/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_C_UC`]

[@http://en.wikipedia.org/wiki/Uclibc uClibc] Standard C library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__UCLIBC__`] [__predef_detection__]]

    [[`__UCLIBC_MAJOR__`, `__UCLIBC_MINOR__`, `__UCLIBC_SUBLEVEL__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LIB_C_UC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__UCLIBC__)
#   undef MSGHANDLE_LIB_C_UC
#   define MSGHANDLE_LIB_C_UC MSGHANDLE_VERSION_NUMBER(\
        __UCLIBC_MAJOR__,__UCLIBC_MINOR__,__UCLIBC_SUBLEVEL__)
#endif

#if MSGHANDLE_LIB_C_UC
#   define MSGHANDLE_LIB_C_UC_AVAILABLE
#endif

#define MSGHANDLE_LIB_C_UC_NAME "uClibc"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_C_UC,MSGHANDLE_LIB_C_UC_NAME)
