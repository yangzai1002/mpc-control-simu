/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_DIAB_H
#define MSGHANDLE_PREDEF_COMPILER_DIAB_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_DIAB`]

[@http://www.windriver.com/products/development_suite/wind_river_compiler/ Diab C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DCC__`] [__predef_detection__]]

    [[`__VERSION_NUMBER__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_DIAB MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DCC__)
#   define MSGHANDLE_COMP_DIAB_DETECTION MSGHANDLE_PREDEF_MAKE_10_VRPP(__VERSION_NUMBER__)
#endif

#ifdef MSGHANDLE_COMP_DIAB_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_DIAB_EMULATED MSGHANDLE_COMP_DIAB_DETECTION
#   else
#       undef MSGHANDLE_COMP_DIAB
#       define MSGHANDLE_COMP_DIAB MSGHANDLE_COMP_DIAB_DETECTION
#   endif
#   define MSGHANDLE_COMP_DIAB_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_DIAB_NAME "Diab C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_DIAB,MSGHANDLE_COMP_DIAB_NAME)

#ifdef MSGHANDLE_COMP_DIAB_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_DIAB_EMULATED,MSGHANDLE_COMP_DIAB_NAME)
#endif
