/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_SYS390_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_SYS390_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_SYS390`]

[@http://en.wikipedia.org/wiki/System/390 System/390] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__s390__`] [__predef_detection__]]
    [[`__s390x__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_ARCH_SYS390 MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__s390__) || defined(__s390x__)
#   undef MSGHANDLE_ARCH_SYS390
#   define MSGHANDLE_ARCH_SYS390 MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_SYS390
#   define MSGHANDLE_ARCH_SYS390_AVAILABLE
#endif

#define MSGHANDLE_ARCH_SYS390_NAME "System/390"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_SYS390,MSGHANDLE_ARCH_SYS390_NAME)
