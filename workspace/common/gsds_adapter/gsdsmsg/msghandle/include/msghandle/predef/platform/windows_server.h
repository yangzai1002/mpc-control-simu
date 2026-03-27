/*
Copyright James E. King III, 2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_WINDOWS_SERVER_H
#define MSGHANDLE_PREDEF_PLAT_WINDOWS_SERVER_H

#include <msghandle/predef/make.h>
#include <msghandle/predef/os/windows.h>
#include <msghandle/predef/platform/windows_uwp.h>
#include <msghandle/predef/version_number.h>

/*`
[heading `MSGHANDLE_PLAT_WINDOWS_SERVER`]

[@https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide UWP]
for Windows Server development.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`WINAPI_FAMILY == WINAPI_FAMILY_SERVER`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_PLAT_WINDOWS_SERVER MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if MSGHANDLE_OS_WINDOWS && \
    defined(WINAPI_FAMILY_SERVER) && WINAPI_FAMILY == WINAPI_FAMILY_SERVER
#   undef MSGHANDLE_PLAT_WINDOWS_SERVER
#   define MSGHANDLE_PLAT_WINDOWS_SERVER MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif
 
#if MSGHANDLE_PLAT_WINDOWS_SERVER
#   define MSGHANDLE_PLAT_WINDOWS_SERVER_AVAILABLE
#   include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_WINDOWS_SERVER_NAME "Windows Server"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_WINDOWS_SERVER,MSGHANDLE_PLAT_WINDOWS_SERVER_NAME)
