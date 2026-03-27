/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_CXX_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_CXX_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_CXX`]

[@http://libcxx.llvm.org/ libc++] C++ Standard Library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_LIBCPP_VERSION`] [__predef_detection__]]

    [[`_LIBCPP_VERSION`] [V.0.P]]
    ]
 */

#define MSGHANDLE_LIB_STD_CXX MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_LIBCPP_VERSION)
#   undef MSGHANDLE_LIB_STD_CXX
#   define MSGHANDLE_LIB_STD_CXX MSGHANDLE_PREDEF_MAKE_10_VPPP(_LIBCPP_VERSION)
#endif

#if MSGHANDLE_LIB_STD_CXX
#   define MSGHANDLE_LIB_STD_CXX_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_CXX_NAME "libc++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_CXX,MSGHANDLE_LIB_STD_CXX_NAME)
