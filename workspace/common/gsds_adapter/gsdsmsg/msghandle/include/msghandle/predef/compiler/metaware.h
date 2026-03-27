/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_METAWARE_H
#define MSGHANDLE_PREDEF_COMPILER_METAWARE_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_HIGHC`]

MetaWare High C/C++ compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__HIGHC__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_COMP_HIGHC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__HIGHC__)
#   define MSGHANDLE_COMP_HIGHC_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#ifdef MSGHANDLE_COMP_HIGHC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_HIGHC_EMULATED MSGHANDLE_COMP_HIGHC_DETECTION
#   else
#       undef MSGHANDLE_COMP_HIGHC
#       define MSGHANDLE_COMP_HIGHC MSGHANDLE_COMP_HIGHC_DETECTION
#   endif
#   define MSGHANDLE_COMP_HIGHC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_HIGHC_NAME "MetaWare High C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_HIGHC,MSGHANDLE_COMP_HIGHC_NAME)

#ifdef MSGHANDLE_COMP_HIGHC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_HIGHC_EMULATED,MSGHANDLE_COMP_HIGHC_NAME)
#endif
