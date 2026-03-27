/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_KAI_H
#define MSGHANDLE_PREDEF_COMPILER_KAI_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_KCC`]

Kai C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__KCC`] [__predef_detection__]]

    [[`__KCC_VERSION`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_KCC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__KCC)
#   define MSGHANDLE_COMP_KCC_DETECTION MSGHANDLE_PREDEF_MAKE_0X_VRPP(__KCC_VERSION)
#endif

#ifdef MSGHANDLE_COMP_KCC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_KCC_EMULATED MSGHANDLE_COMP_KCC_DETECTION
#   else
#       undef MSGHANDLE_COMP_KCC
#       define MSGHANDLE_COMP_KCC MSGHANDLE_COMP_KCC_DETECTION
#   endif
#   define MSGHANDLE_COMP_KCC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_KCC_NAME "Kai C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_KCC,MSGHANDLE_COMP_KCC_NAME)

#ifdef MSGHANDLE_COMP_KCC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_KCC_EMULATED,MSGHANDLE_COMP_KCC_NAME)
#endif
