/*
Copyright (c) Microsoft Corporation 2014
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_WINDOWS_PHONE_H
#define MSGHANDLE_PREDEF_PLAT_WINDOWS_PHONE_H

#include <msghandle/predef/make.h>
#include <msghandle/predef/os/windows.h>
#include <msghandle/predef/platform/windows_uwp.h>
#include <msghandle/predef/version_number.h>

/*`
[heading `MSGHANDLE_PLAT_WINDOWS_PHONE`]

[@https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide UWP]
for Windows Phone development.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_PLAT_WINDOWS_PHONE MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if MSGHANDLE_OS_WINDOWS && \
    defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#   undef MSGHANDLE_PLAT_WINDOWS_PHONE
#   define MSGHANDLE_PLAT_WINDOWS_PHONE MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif
 
#if MSGHANDLE_PLAT_WINDOWS_PHONE
#   define MSGHANDLE_PLAT_WINDOWS_PHONE_AVAILABLE
#   include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_WINDOWS_PHONE_NAME "Windows Phone"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_WINDOWS_PHONE,MSGHANDLE_PLAT_WINDOWS_PHONE_NAME)
