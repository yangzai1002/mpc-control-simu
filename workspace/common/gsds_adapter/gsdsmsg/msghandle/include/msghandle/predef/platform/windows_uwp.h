/*
Copyright James E. King III, 2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_WINDOWS_UWP_H
#define MSGHANDLE_PREDEF_PLAT_WINDOWS_UWP_H

#include <msghandle/predef/make.h>
#include <msghandle/predef/os/windows.h>
#include <msghandle/predef/version_number.h>

/*`
[heading `MSGHANDLE_PLAT_WINDOWS_UWP`]

[@http://docs.microsoft.com/windows/uwp/ Universal Windows Platform]
is available if the current development environment is capable of targeting 
UWP development.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MINGW64_VERSION_MAJOR` from `_mingw.h`] [`>= 3`]]
    [[`VER_PRODUCTBUILD` from `ntverp.h`] [`>= 9200`]]
]
*/

#define MSGHANDLE_PLAT_WINDOWS_UWP MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE
#define MSGHANDLE_PLAT_WINDOWS_SDK_VERSION MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if MSGHANDLE_OS_WINDOWS
//  MinGW (32-bit) has no ntverp.h header
#if !defined(__MINGW32__)
#   include <ntverp.h>
#   undef MSGHANDLE_PLAT_WINDOWS_SDK_VERSION
#   define MSGHANDLE_PLAT_WINDOWS_SDK_VERSION MSGHANDLE_VERSION_NUMBER(0, 0, VER_PRODUCTBUILD)
#endif

// 9200 is Windows SDK 8.0 from ntverp.h which introduced family support
#if ((MSGHANDLE_PLAT_WINDOWS_SDK_VERSION >= MSGHANDLE_VERSION_NUMBER(0, 0, 9200)) || \
     (defined(__MINGW64__) && __MINGW64_VERSION_MAJOR >= 3))
#   undef MSGHANDLE_PLAT_WINDOWS_UWP
#   define MSGHANDLE_PLAT_WINDOWS_UWP MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif
#endif

#if MSGHANDLE_PLAT_WINDOWS_UWP
#   define MSGHANDLE_PLAT_WINDOWS_UWP_AVAILABLE
#   include <msghandle/predef/detail/platform_detected.h>
#   include <winapifamily.h>    // Windows SDK
#endif

#define MSGHANDLE_PLAT_WINDOWS_UWP_NAME "Universal Windows Platform"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_WINDOWS_UWP, MSGHANDLE_PLAT_WINDOWS_UWP_NAME)
