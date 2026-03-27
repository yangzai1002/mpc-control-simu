/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_WATCOM_H
#define MSGHANDLE_PREDEF_COMPILER_WATCOM_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_WATCOM`]

[@http://en.wikipedia.org/wiki/Watcom Watcom C++] compiler.
Version number available as major, and minor.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__WATCOMC__`] [__predef_detection__]]

    [[`__WATCOMC__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_WATCOM MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__WATCOMC__)
#   define MSGHANDLE_COMP_WATCOM_DETECTION MSGHANDLE_PREDEF_MAKE_10_VVRR(__WATCOMC__)
#endif

#ifdef MSGHANDLE_COMP_WATCOM_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_WATCOM_EMULATED MSGHANDLE_COMP_WATCOM_DETECTION
#   else
#       undef MSGHANDLE_COMP_WATCOM
#       define MSGHANDLE_COMP_WATCOM MSGHANDLE_COMP_WATCOM_DETECTION
#   endif
#   define MSGHANDLE_COMP_WATCOM_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_WATCOM_NAME "Watcom C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_WATCOM,MSGHANDLE_COMP_WATCOM_NAME)

#ifdef MSGHANDLE_COMP_WATCOM_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_WATCOM_EMULATED,MSGHANDLE_COMP_WATCOM_NAME)
#endif
