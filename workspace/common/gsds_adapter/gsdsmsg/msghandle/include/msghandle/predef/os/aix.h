/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_OS_AIX_H
#define MSGHANDLE_PREDEF_OS_AIX_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_OS_AIX`]

[@http://en.wikipedia.org/wiki/AIX_operating_system IBM AIX] operating system.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_AIX`] [__predef_detection__]]
    [[`__TOS_AIX__`] [__predef_detection__]]

    [[`_AIX43`] [4.3.0]]
    [[`_AIX41`] [4.1.0]]
    [[`_AIX32`] [3.2.0]]
    [[`_AIX3`] [3.0.0]]
    ]
 */

#define MSGHANDLE_OS_AIX MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(MSGHANDLE_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(_AIX) || defined(__TOS_AIX__) \
    )
#   undef MSGHANDLE_OS_AIX
#   if !defined(MSGHANDLE_OS_AIX) && defined(_AIX43)
#       define MSGHANDLE_OS_AIX MSGHANDLE_VERSION_NUMBER(4,3,0)
#   endif
#   if !defined(MSGHANDLE_OS_AIX) && defined(_AIX41)
#       define MSGHANDLE_OS_AIX MSGHANDLE_VERSION_NUMBER(4,1,0)
#   endif
#   if !defined(MSGHANDLE_OS_AIX) && defined(_AIX32)
#       define MSGHANDLE_OS_AIX MSGHANDLE_VERSION_NUMBER(3,2,0)
#   endif
#   if !defined(MSGHANDLE_OS_AIX) && defined(_AIX3)
#       define MSGHANDLE_OS_AIX MSGHANDLE_VERSION_NUMBER(3,0,0)
#   endif
#   if !defined(MSGHANDLE_OS_AIX)
#       define MSGHANDLE_OS_AIX MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_OS_AIX
#   define MSGHANDLE_OS_AIX_AVAILABLE
#   include <msghandle/predef/detail/os_detected.h>
#endif

#define MSGHANDLE_OS_AIX_NAME "IBM AIX"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_OS_AIX,MSGHANDLE_OS_AIX_NAME)
