/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_EDG_H
#define MSGHANDLE_PREDEF_COMPILER_EDG_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_EDG`]

[@http://en.wikipedia.org/wiki/Edison_Design_Group EDG C++ Frontend] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__EDG__`] [__predef_detection__]]

    [[`__EDG_VERSION__`] [V.R.0]]
    ]
 */

#define MSGHANDLE_COMP_EDG MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__EDG__)
#   define MSGHANDLE_COMP_EDG_DETECTION MSGHANDLE_PREDEF_MAKE_10_VRR(__EDG_VERSION__)
#endif

#ifdef MSGHANDLE_COMP_EDG_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_EDG_EMULATED MSGHANDLE_COMP_EDG_DETECTION
#   else
#       undef MSGHANDLE_COMP_EDG
#       define MSGHANDLE_COMP_EDG MSGHANDLE_COMP_EDG_DETECTION
#   endif
#   define MSGHANDLE_COMP_EDG_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_EDG_NAME "EDG C++ Frontend"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_EDG,MSGHANDLE_COMP_EDG_NAME)

#ifdef MSGHANDLE_COMP_EDG_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_EDG_EMULATED,MSGHANDLE_COMP_EDG_NAME)
#endif
