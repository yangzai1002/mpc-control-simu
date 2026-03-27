/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_OS_QNXNTO_H
#define MSGHANDLE_PREDEF_OS_QNXNTO_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_OS_QNX`]

[@http://en.wikipedia.org/wiki/QNX QNX] operating system.
Version number available as major, and minor if possible. And
version 4 is specifically detected.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__QNX__`] [__predef_detection__]]
    [[`__QNXNTO__`] [__predef_detection__]]

    [[`_NTO_VERSION`] [V.R.0]]
    [[`__QNX__`] [4.0.0]]
    ]
 */

#define MSGHANDLE_OS_QNX MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(MSGHANDLE_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__QNX__) || defined(__QNXNTO__) \
    )
#   undef MSGHANDLE_OS_QNX
#   if !defined(MSGHANDLE_OS_QNX) && defined(_NTO_VERSION)
#       define MSGHANDLE_OS_QNX MSGHANDLE_PREDEF_MAKE_10_VVRR(_NTO_VERSION)
#   endif
#   if !defined(MSGHANDLE_OS_QNX) && defined(__QNX__)
#       define MSGHANDLE_OS_QNX MSGHANDLE_VERSION_NUMBER(4,0,0)
#   endif
#   if !defined(MSGHANDLE_OS_QNX)
#       define MSGHANDLE_OS_QNX MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_OS_QNX
#   define MSGHANDLE_OS_QNX_AVAILABLE
#   include <msghandle/predef/detail/os_detected.h>
#endif

#define MSGHANDLE_OS_QNX_NAME "QNX"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_OS_QNX,MSGHANDLE_OS_QNX_NAME)
