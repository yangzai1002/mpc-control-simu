/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_MSL_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_MSL_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_MSL`]

[@http://www.freescale.com/ Metrowerks] Standard C++ Library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MSL_CPP__`] [__predef_detection__]]
    [[`__MSL__`] [__predef_detection__]]

    [[`__MSL_CPP__`] [V.R.P]]
    [[`__MSL__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LIB_STD_MSL MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MSL_CPP__) || defined(__MSL__)
#   undef MSGHANDLE_LIB_STD_MSL
#   if defined(__MSL_CPP__)
#       define MSGHANDLE_LIB_STD_MSL MSGHANDLE_PREDEF_MAKE_0X_VRPP(__MSL_CPP__)
#   else
#       define MSGHANDLE_LIB_STD_MSL MSGHANDLE_PREDEF_MAKE_0X_VRPP(__MSL__)
#   endif
#endif

#if MSGHANDLE_LIB_STD_MSL
#   define MSGHANDLE_LIB_STD_MSL_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_MSL_NAME "Metrowerks"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_MSL,MSGHANDLE_LIB_STD_MSL_NAME)
