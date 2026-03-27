/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_GREENHILLS_H
#define MSGHANDLE_PREDEF_COMPILER_GREENHILLS_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_GHS`]

[@http://en.wikipedia.org/wiki/Green_Hills_Software Green Hills C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__ghs`] [__predef_detection__]]
    [[`__ghs__`] [__predef_detection__]]

    [[`__GHS_VERSION_NUMBER__`] [V.R.P]]
    [[`__ghs`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_GHS MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__ghs) || defined(__ghs__)
#   if !defined(MSGHANDLE_COMP_GHS_DETECTION) && defined(__GHS_VERSION_NUMBER__)
#       define MSGHANDLE_COMP_GHS_DETECTION MSGHANDLE_PREDEF_MAKE_10_VRP(__GHS_VERSION_NUMBER__)
#   endif
#   if !defined(MSGHANDLE_COMP_GHS_DETECTION) && defined(__ghs)
#       define MSGHANDLE_COMP_GHS_DETECTION MSGHANDLE_PREDEF_MAKE_10_VRP(__ghs)
#   endif
#   if !defined(MSGHANDLE_COMP_GHS_DETECTION)
#       define MSGHANDLE_COMP_GHS_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_GHS_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_GHS_EMULATED MSGHANDLE_COMP_GHS_DETECTION
#   else
#       undef MSGHANDLE_COMP_GHS
#       define MSGHANDLE_COMP_GHS MSGHANDLE_COMP_GHS_DETECTION
#   endif
#   define MSGHANDLE_COMP_GHS_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_GHS_NAME "Green Hills C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_GHS,MSGHANDLE_COMP_GHS_NAME)

#ifdef MSGHANDLE_COMP_GHS_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_GHS_EMULATED,MSGHANDLE_COMP_GHS_NAME)
#endif
