/*
Copyright Jessica Hamilton 2014
Copyright Rene Rivera 2014-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_OS_HAIKU_H
#define MSGHANDLE_PREDEF_OS_HAIKU_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_OS_HAIKU`]

[@http://en.wikipedia.org/wiki/Haiku_(operating_system) Haiku] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__HAIKU__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_OS_HAIKU MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(MSGHANDLE_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__HAIKU__) \
    )
#   undef MSGHANDLE_OS_HAIKU
#   define MSGHANDLE_OS_HAIKU MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_OS_HAIKU
#   define MSGHANDLE_OS_HAIKU_AVAILABLE
#   include <msghandle/predef/detail/os_detected.h>
#endif

#define MSGHANDLE_OS_HAIKU_NAME "Haiku"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_OS_HAIKU,MSGHANDLE_OS_HAIKU_NAME)
