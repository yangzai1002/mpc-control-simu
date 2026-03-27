/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_CLANG_H
#define MSGHANDLE_PREDEF_COMPILER_CLANG_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_CLANG`]

[@http://en.wikipedia.org/wiki/Clang Clang] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__clang__`] [__predef_detection__]]

    [[`__clang_major__`, `__clang_minor__`, `__clang_patchlevel__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_CLANG MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__clang__)
#   define MSGHANDLE_COMP_CLANG_DETECTION MSGHANDLE_VERSION_NUMBER(__clang_major__,__clang_minor__,__clang_patchlevel__)
#endif

#ifdef MSGHANDLE_COMP_CLANG_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_CLANG_EMULATED MSGHANDLE_COMP_CLANG_DETECTION
#   else
#       undef MSGHANDLE_COMP_CLANG
#       define MSGHANDLE_COMP_CLANG MSGHANDLE_COMP_CLANG_DETECTION
#   endif
#   define MSGHANDLE_COMP_CLANG_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_CLANG_NAME "Clang"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_CLANG,MSGHANDLE_COMP_CLANG_NAME)

#ifdef MSGHANDLE_COMP_CLANG_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_CLANG_EMULATED,MSGHANDLE_COMP_CLANG_NAME)
#endif
