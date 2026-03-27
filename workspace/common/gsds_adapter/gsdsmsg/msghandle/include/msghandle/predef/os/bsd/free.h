/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_OS_BSD_FREE_H
#define MSGHANDLE_PREDEF_OS_BSD_FREE_H

#include <msghandle/predef/os/bsd.h>

/*`
[heading `MSGHANDLE_OS_BSD_FREE`]

[@http://en.wikipedia.org/wiki/Freebsd FreeBSD] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__FreeBSD__`] [__predef_detection__]]

    [[`__FreeBSD_version`] [V.R.P]]
    ]
 */

#define MSGHANDLE_OS_BSD_FREE MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(MSGHANDLE_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__FreeBSD__) \
    )
#   ifndef MSGHANDLE_OS_BSD_AVAILABLE
#       define MSGHANDLE_OS_BSD MSGHANDLE_VERSION_NUMBER_AVAILABLE
#       define MSGHANDLE_OS_BSD_AVAILABLE
#   endif
#   undef MSGHANDLE_OS_BSD_FREE
#   include <sys/param.h>
#   if defined(__FreeBSD_version)
#       if __FreeBSD_version == 491000
#           define MSGHANDLE_OS_BSD_FREE \
                MSGHANDLE_VERSION_NUMBER(4, 10, 0)
#       elif __FreeBSD_version == 492000
#           define MSGHANDLE_OS_BSD_FREE \
                MSGHANDLE_VERSION_NUMBER(4, 11, 0)
#       elif __FreeBSD_version < 500000
#           define MSGHANDLE_OS_BSD_FREE \
                MSGHANDLE_PREDEF_MAKE_10_VRPPPP(__FreeBSD_version)
#       else
#           define MSGHANDLE_OS_BSD_FREE \
                MSGHANDLE_PREDEF_MAKE_10_VVRRPPP(__FreeBSD_version)
#       endif
#   else
#       define MSGHANDLE_OS_BSD_FREE MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_OS_BSD_FREE
#   define MSGHANDLE_OS_BSD_FREE_AVAILABLE
#   include <msghandle/predef/detail/os_detected.h>
#endif

#define MSGHANDLE_OS_BSD_FREE_NAME "Free BSD"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_OS_BSD_FREE,MSGHANDLE_OS_BSD_FREE_NAME)
