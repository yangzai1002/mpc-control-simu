/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_ROGUEWAVE_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_ROGUEWAVE_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_RW`]

[@http://stdcxx.apache.org/ Roguewave] Standard C++ library.
If available version number as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__STD_RWCOMPILER_H__`] [__predef_detection__]]
    [[`_RWSTD_VER`] [__predef_detection__]]

    [[`_RWSTD_VER`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LIB_STD_RW MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__STD_RWCOMPILER_H__) || defined(_RWSTD_VER)
#   undef MSGHANDLE_LIB_STD_RW
#   if defined(_RWSTD_VER)
#       if _RWSTD_VER < 0x010000
#           define MSGHANDLE_LIB_STD_RW MSGHANDLE_PREDEF_MAKE_0X_VVRRP(_RWSTD_VER)
#       else
#           define MSGHANDLE_LIB_STD_RW MSGHANDLE_PREDEF_MAKE_0X_VVRRPP(_RWSTD_VER)
#       endif
#   else
#       define MSGHANDLE_LIB_STD_RW MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_LIB_STD_RW
#   define MSGHANDLE_LIB_STD_RW_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_RW_NAME "Roguewave"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_RW,MSGHANDLE_LIB_STD_RW_NAME)
