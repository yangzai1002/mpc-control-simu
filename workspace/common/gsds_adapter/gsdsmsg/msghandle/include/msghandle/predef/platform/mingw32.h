/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_MINGW32_H
#define MSGHANDLE_PREDEF_PLAT_MINGW32_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_PLAT_MINGW32`]

[@http://www.mingw.org/ MinGW] platform.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MINGW32__`] [__predef_detection__]]

    [[`__MINGW32_VERSION_MAJOR`, `__MINGW32_VERSION_MINOR`] [V.R.0]]
    ]
 */

#define MSGHANDLE_PLAT_MINGW32 MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MINGW32__)
#   include <_mingw.h>
#   if !defined(MSGHANDLE_PLAT_MINGW32_DETECTION) && (defined(__MINGW32_VERSION_MAJOR) && defined(__MINGW32_VERSION_MINOR))
#       define MSGHANDLE_PLAT_MINGW32_DETECTION \
            MSGHANDLE_VERSION_NUMBER(__MINGW32_VERSION_MAJOR,__MINGW32_VERSION_MINOR,0)
#   endif
#   if !defined(MSGHANDLE_PLAT_MINGW32_DETECTION)
#       define MSGHANDLE_PLAT_MINGW32_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_PLAT_MINGW32_DETECTION
#   define MSGHANDLE_PLAT_MINGW32_AVAILABLE
#   if defined(MSGHANDLE_PREDEF_DETAIL_PLAT_DETECTED)
#       define MSGHANDLE_PLAT_MINGW32_EMULATED MSGHANDLE_PLAT_MINGW32_DETECTION
#   else
#       undef MSGHANDLE_PLAT_MINGW32
#       define MSGHANDLE_PLAT_MINGW32 MSGHANDLE_PLAT_MINGW32_DETECTION
#   endif
#   include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_MINGW32_NAME "MinGW"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_MINGW32,MSGHANDLE_PLAT_MINGW32_NAME)

#ifdef MSGHANDLE_PLAT_MINGW32_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_MINGW32_EMULATED,MSGHANDLE_PLAT_MINGW32_NAME)
#endif
