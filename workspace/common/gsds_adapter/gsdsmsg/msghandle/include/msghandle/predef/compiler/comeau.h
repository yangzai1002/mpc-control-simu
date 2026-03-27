/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_COMEAU_H
#define MSGHANDLE_PREDEF_COMPILER_COMEAU_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

#define MSGHANDLE_COMP_COMO MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

/*`
[heading `MSGHANDLE_COMP_COMO`]

[@http://en.wikipedia.org/wiki/Comeau_C/C%2B%2B Comeau C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__COMO__`] [__predef_detection__]]

    [[`__COMO_VERSION__`] [V.R.P]]
    ]
 */

#if defined(__COMO__)
#   if !defined(MSGHANDLE_COMP_COMO_DETECTION) && defined(__COMO_VERSION__)
#       define MSGHANDLE_COMP_COMO_DETECTION MSGHANDLE_PREDEF_MAKE_0X_VRP(__COMO_VERSION__)
#   endif
#   if !defined(MSGHANDLE_COMP_COMO_DETECTION)
#       define MSGHANDLE_COMP_COMO_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_COMO_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_COMO_EMULATED MSGHANDLE_COMP_COMO_DETECTION
#   else
#       undef MSGHANDLE_COMP_COMO
#       define MSGHANDLE_COMP_COMO MSGHANDLE_COMP_COMO_DETECTION
#   endif
#   define MSGHANDLE_COMP_COMO_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_COMO_NAME "Comeau C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_COMO,MSGHANDLE_COMP_COMO_NAME)

#ifdef MSGHANDLE_COMP_COMO_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_COMO_EMULATED,MSGHANDLE_COMP_COMO_NAME)
#endif
