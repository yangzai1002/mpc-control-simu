/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_DIGITALMARS_H
#define MSGHANDLE_PREDEF_COMPILER_DIGITALMARS_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_DMC`]

[@http://en.wikipedia.org/wiki/Digital_Mars Digital Mars] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DMC__`] [__predef_detection__]]

    [[`__DMC__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_DMC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DMC__)
#   define MSGHANDLE_COMP_DMC_DETECTION MSGHANDLE_PREDEF_MAKE_0X_VRP(__DMC__)
#endif

#ifdef MSGHANDLE_COMP_DMC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_DMC_EMULATED MSGHANDLE_COMP_DMC_DETECTION
#   else
#       undef MSGHANDLE_COMP_DMC
#       define MSGHANDLE_COMP_DMC MSGHANDLE_COMP_DMC_DETECTION
#   endif
#   define MSGHANDLE_COMP_DMC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_DMC_NAME "Digital Mars"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_DMC,MSGHANDLE_COMP_DMC_NAME)

#ifdef MSGHANDLE_COMP_DMC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_DMC_EMULATED,MSGHANDLE_COMP_DMC_NAME)
#endif
