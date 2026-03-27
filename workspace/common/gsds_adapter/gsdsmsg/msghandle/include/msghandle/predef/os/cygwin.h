/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_OS_CYGWIN_H
#define MSGHANDLE_PREDEF_OS_CYGWIN_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_OS_CYGWIN`]

[@http://en.wikipedia.org/wiki/Cygwin Cygwin] evironment.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CYGWIN__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_OS_CYGWIN MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(MSGHANDLE_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__CYGWIN__) \
    )
#   undef MSGHANDLE_OS_CYGWIN
#   define MSGHANDLE_OS_CYGWIN MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_OS_CYGWIN
#   define MSGHANDLE_OS_CYGWIN_AVAILABLE
#   include <msghandle/predef/detail/os_detected.h>
#endif

#define MSGHANDLE_OS_CYGWIN_NAME "Cygwin"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_OS_CYGWIN,MSGHANDLE_OS_CYGWIN_NAME)
