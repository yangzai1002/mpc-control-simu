/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_Z_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_Z_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_Z`]

[@http://en.wikipedia.org/wiki/Z/Architecture z/Architecture] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SYSC_ZARCH__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_ARCH_Z MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SYSC_ZARCH__)
#   undef MSGHANDLE_ARCH_Z
#   define MSGHANDLE_ARCH_Z MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_Z
#   define MSGHANDLE_ARCH_Z_AVAILABLE
#endif

#define MSGHANDLE_ARCH_Z_NAME "z/Architecture"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_Z,MSGHANDLE_ARCH_Z_NAME)
