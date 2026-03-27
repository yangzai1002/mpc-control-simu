/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_OS_BSD_BSDI_H
#define MSGHANDLE_PREDEF_OS_BSD_BSDI_H

#include <msghandle/predef/os/bsd.h>

/*`
[heading `MSGHANDLE_OS_BSD_BSDI`]

[@http://en.wikipedia.org/wiki/BSD/OS BSDi BSD/OS] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__bsdi__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_OS_BSD_BSDI MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(MSGHANDLE_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__bsdi__) \
    )
#   ifndef MSGHANDLE_OS_BSD_AVAILABLE
#       define MSGHANDLE_OS_BSD MSGHANDLE_VERSION_NUMBER_AVAILABLE
#       define MSGHANDLE_OS_BSD_AVAILABLE
#   endif
#   undef MSGHANDLE_OS_BSD_BSDI
#   define MSGHANDLE_OS_BSD_BSDI MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_OS_BSD_BSDI
#   define MSGHANDLE_OS_BSD_BSDI_AVAILABLE
#   include <msghandle/predef/detail/os_detected.h>
#endif

#define MSGHANDLE_OS_BSD_BSDI_NAME "BSDi BSD/OS"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_OS_BSD_BSDI,MSGHANDLE_OS_BSD_BSDI_NAME)
