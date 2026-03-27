/*
Copyright (c) Microsoft Corporation 2014
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_WINDOWS_DESKTOP_H
#define MSGHANDLE_PREDEF_PLAT_WINDOWS_DESKTOP_H

#include <msghandle/predef/make.h>
#include <msghandle/predef/os/windows.h>
#include <msghandle/predef/platform/windows_uwp.h>
#include <msghandle/predef/version_number.h>

/*`
[heading `MSGHANDLE_PLAT_WINDOWS_DESKTOP`]

[@https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide UWP]
for Windows Desktop development.  Also available if the Platform SDK is too
old to support UWP.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP`] [__predef_detection__]]
    [[`!MSGHANDLE_PLAT_WINDOWS_UWP`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_PLAT_WINDOWS_DESKTOP MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if MSGHANDLE_OS_WINDOWS && \
    ((defined(WINAPI_FAMILY_DESKTOP_APP) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) || \
     !MSGHANDLE_PLAT_WINDOWS_UWP)
#   undef MSGHANDLE_PLAT_WINDOWS_DESKTOP
#   define MSGHANDLE_PLAT_WINDOWS_DESKTOP MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif
 
#if MSGHANDLE_PLAT_WINDOWS_DESKTOP
#   define MSGHANDLE_PLAT_WINDOWS_DESKTOP_AVAILABLE
#   include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_WINDOWS_DESKTOP_NAME "Windows Desktop"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_WINDOWS_DESKTOP,MSGHANDLE_PLAT_WINDOWS_DESKTOP_NAME)
