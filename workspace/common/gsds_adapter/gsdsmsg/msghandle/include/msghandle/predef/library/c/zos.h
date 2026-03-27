/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_C_ZOS_H
#define MSGHANDLE_PREDEF_LIBRARY_C_ZOS_H

#include <msghandle/predef/library/c/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_C_ZOS`]

z/OS libc Standard C library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__LIBREL__`] [__predef_detection__]]

    [[`__LIBREL__`] [V.R.P]]
    [[`__TARGET_LIB__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LIB_C_ZOS MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__LIBREL__)
#   undef MSGHANDLE_LIB_C_ZOS
#   if !defined(MSGHANDLE_LIB_C_ZOS) && defined(__LIBREL__)
#       define MSGHANDLE_LIB_C_ZOS MSGHANDLE_PREDEF_MAKE_0X_VRRPPPP(__LIBREL__)
#   endif
#   if !defined(MSGHANDLE_LIB_C_ZOS) && defined(__TARGET_LIB__)
#       define MSGHANDLE_LIB_C_ZOS MSGHANDLE_PREDEF_MAKE_0X_VRRPPPP(__TARGET_LIB__)
#   endif
#   if !defined(MSGHANDLE_LIB_C_ZOS)
#       define MSGHANDLE_LIB_C_ZOS MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_LIB_C_ZOS
#   define MSGHANDLE_LIB_C_ZOS_AVAILABLE
#endif

#define MSGHANDLE_LIB_C_ZOS_NAME "z/OS"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_C_ZOS,MSGHANDLE_LIB_C_ZOS_NAME)
