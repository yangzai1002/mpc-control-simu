/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_BORLAND_H
#define MSGHANDLE_PREDEF_COMPILER_BORLAND_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_BORLAND`]

[@http://en.wikipedia.org/wiki/C_plus_plus_builder Borland C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__BORLANDC__`] [__predef_detection__]]
    [[`__CODEGEARC__`] [__predef_detection__]]

    [[`__BORLANDC__`] [V.R.P]]
    [[`__CODEGEARC__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_BORLAND MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__BORLANDC__) || defined(__CODEGEARC__)
#   if !defined(MSGHANDLE_COMP_BORLAND_DETECTION) && (defined(__CODEGEARC__))
#       define MSGHANDLE_COMP_BORLAND_DETECTION MSGHANDLE_PREDEF_MAKE_0X_VVRP(__CODEGEARC__)
#   endif
#   if !defined(MSGHANDLE_COMP_BORLAND_DETECTION)
#       define MSGHANDLE_COMP_BORLAND_DETECTION MSGHANDLE_PREDEF_MAKE_0X_VVRP(__BORLANDC__)
#   endif
#endif

#ifdef MSGHANDLE_COMP_BORLAND_DETECTION
#   define MSGHANDLE_COMP_BORLAND_AVAILABLE
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_BORLAND_EMULATED MSGHANDLE_COMP_BORLAND_DETECTION
#   else
#       undef MSGHANDLE_COMP_BORLAND
#       define MSGHANDLE_COMP_BORLAND MSGHANDLE_COMP_BORLAND_DETECTION
#   endif
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_BORLAND_NAME "Borland C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_BORLAND,MSGHANDLE_COMP_BORLAND_NAME)

#ifdef MSGHANDLE_COMP_BORLAND_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_BORLAND_EMULATED,MSGHANDLE_COMP_BORLAND_NAME)
#endif
