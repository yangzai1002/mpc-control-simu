/*
Copyright (c) Microsoft Corporation 2014
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_WINDOWS_RUNTIME_H
#define MSGHANDLE_PREDEF_PLAT_WINDOWS_RUNTIME_H

#include <msghandle/predef/make.h>
#include <msghandle/predef/os/windows.h>
#include <msghandle/predef/platform/windows_phone.h>
#include <msghandle/predef/platform/windows_store.h>
#include <msghandle/predef/version_number.h>

/*`
[heading `MSGHANDLE_PLAT_WINDOWS_RUNTIME`]

Deprecated.

[@https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide UWP]
for Windows Phone or Store development.  This does not align to the existing development model for
UWP and is deprecated.  Use one of the other `MSGHANDLE_PLAT_WINDOWS_*`definitions instead.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`MSGHANDLE_PLAT_WINDOWS_PHONE`] [__predef_detection__]]
    [[`MSGHANDLE_PLAT_WINDOWS_STORE`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_PLAT_WINDOWS_RUNTIME MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if MSGHANDLE_OS_WINDOWS && \
    (MSGHANDLE_PLAT_WINDOWS_STORE || MSGHANDLE_PLAT_WINDOWS_PHONE)
#   undef MSGHANDLE_PLAT_WINDOWS_RUNTIME
#   define MSGHANDLE_PLAT_WINDOWS_RUNTIME MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif
 
#if MSGHANDLE_PLAT_WINDOWS_RUNTIME
#   define MSGHANDLE_PLAT_WINDOWS_RUNTIME_AVAILABLE
#   include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_WINDOWS_RUNTIME_NAME "Windows Runtime"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_WINDOWS_RUNTIME,MSGHANDLE_PLAT_WINDOWS_RUNTIME_NAME)
