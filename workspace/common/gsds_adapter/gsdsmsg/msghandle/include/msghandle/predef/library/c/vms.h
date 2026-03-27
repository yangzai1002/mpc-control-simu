/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_C_VMS_H
#define MSGHANDLE_PREDEF_LIBRARY_C_VMS_H

#include <msghandle/predef/library/c/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_C_VMS`]

VMS libc Standard C library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CRTL_VER`] [__predef_detection__]]

    [[`__CRTL_VER`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LIB_C_VMS MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__CRTL_VER)
#   undef MSGHANDLE_LIB_C_VMS
#   define MSGHANDLE_LIB_C_VMS MSGHANDLE_PREDEF_MAKE_10_VVRR0PP00(__CRTL_VER)
#endif

#if MSGHANDLE_LIB_C_VMS
#   define MSGHANDLE_LIB_C_VMS_AVAILABLE
#endif

#define MSGHANDLE_LIB_C_VMS_NAME "VMS"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_C_VMS,MSGHANDLE_LIB_C_VMS_NAME)
