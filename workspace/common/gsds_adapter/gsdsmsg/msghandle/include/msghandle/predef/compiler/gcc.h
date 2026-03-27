/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_GCC_H
#define MSGHANDLE_PREDEF_COMPILER_GCC_H

/* Other compilers that emulate this one need to be detected first. */

#include <msghandle/predef/compiler/clang.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_GNUC`]

[@http://en.wikipedia.org/wiki/GNU_Compiler_Collection Gnu GCC C/C++] compiler.
Version number available as major, minor, and patch (if available).

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GNUC__`] [__predef_detection__]]

    [[`__GNUC__`, `__GNUC_MINOR__`, `__GNUC_PATCHLEVEL__`] [V.R.P]]
    [[`__GNUC__`, `__GNUC_MINOR__`] [V.R.0]]
    ]
 */

#define MSGHANDLE_COMP_GNUC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GNUC__)
#   if !defined(MSGHANDLE_COMP_GNUC_DETECTION) && defined(__GNUC_PATCHLEVEL__)
#       define MSGHANDLE_COMP_GNUC_DETECTION \
            MSGHANDLE_VERSION_NUMBER(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#   endif
#   if !defined(MSGHANDLE_COMP_GNUC_DETECTION)
#       define MSGHANDLE_COMP_GNUC_DETECTION \
            MSGHANDLE_VERSION_NUMBER(__GNUC__,__GNUC_MINOR__,0)
#   endif
#endif

#ifdef MSGHANDLE_COMP_GNUC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_GNUC_EMULATED MSGHANDLE_COMP_GNUC_DETECTION
#   else
#       undef MSGHANDLE_COMP_GNUC
#       define MSGHANDLE_COMP_GNUC MSGHANDLE_COMP_GNUC_DETECTION
#   endif
#   define MSGHANDLE_COMP_GNUC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_GNUC_NAME "Gnu GCC C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_GNUC,MSGHANDLE_COMP_GNUC_NAME)

#ifdef MSGHANDLE_COMP_GNUC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_GNUC_EMULATED,MSGHANDLE_COMP_GNUC_NAME)
#endif
