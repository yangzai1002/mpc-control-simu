/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_SGI_MIPSPRO_H
#define MSGHANDLE_PREDEF_COMPILER_SGI_MIPSPRO_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_SGI`]

[@http://en.wikipedia.org/wiki/MIPSpro SGI MIPSpro] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sgi`] [__predef_detection__]]
    [[`sgi`] [__predef_detection__]]

    [[`_SGI_COMPILER_VERSION`] [V.R.P]]
    [[`_COMPILER_VERSION`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_SGI MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sgi) || defined(sgi)
#   if !defined(MSGHANDLE_COMP_SGI_DETECTION) && defined(_SGI_COMPILER_VERSION)
#       define MSGHANDLE_COMP_SGI_DETECTION MSGHANDLE_PREDEF_MAKE_10_VRP(_SGI_COMPILER_VERSION)
#   endif
#   if !defined(MSGHANDLE_COMP_SGI_DETECTION) && defined(_COMPILER_VERSION)
#       define MSGHANDLE_COMP_SGI_DETECTION MSGHANDLE_PREDEF_MAKE_10_VRP(_COMPILER_VERSION)
#   endif
#   if !defined(MSGHANDLE_COMP_SGI_DETECTION)
#       define MSGHANDLE_COMP_SGI_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_SGI_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_SGI_EMULATED MSGHANDLE_COMP_SGI_DETECTION
#   else
#       undef MSGHANDLE_COMP_SGI
#       define MSGHANDLE_COMP_SGI MSGHANDLE_COMP_SGI_DETECTION
#   endif
#   define MSGHANDLE_COMP_SGI_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_SGI_NAME "SGI MIPSpro"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_SGI,MSGHANDLE_COMP_SGI_NAME)

#ifdef MSGHANDLE_COMP_SGI_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_SGI_EMULATED,MSGHANDLE_COMP_SGI_NAME)
#endif
