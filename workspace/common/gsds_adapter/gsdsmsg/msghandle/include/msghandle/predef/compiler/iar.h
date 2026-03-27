/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_IAR_H
#define MSGHANDLE_PREDEF_COMPILER_IAR_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_IAR`]

IAR C/C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IAR_SYSTEMS_ICC__`] [__predef_detection__]]

    [[`__VER__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_IAR MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IAR_SYSTEMS_ICC__)
#   define MSGHANDLE_COMP_IAR_DETECTION MSGHANDLE_PREDEF_MAKE_10_VVRR(__VER__)
#endif

#ifdef MSGHANDLE_COMP_IAR_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_IAR_EMULATED MSGHANDLE_COMP_IAR_DETECTION
#   else
#       undef MSGHANDLE_COMP_IAR
#       define MSGHANDLE_COMP_IAR MSGHANDLE_COMP_IAR_DETECTION
#   endif
#   define MSGHANDLE_COMP_IAR_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_IAR_NAME "IAR C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_IAR,MSGHANDLE_COMP_IAR_NAME)

#ifdef MSGHANDLE_COMP_IAR_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_IAR_EMULATED,MSGHANDLE_COMP_IAR_NAME)
#endif
