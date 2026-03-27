/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_SYS370_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_SYS370_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_SYS370`]

[@http://en.wikipedia.org/wiki/System/370 System/370] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__370__`] [__predef_detection__]]
    [[`__THW_370__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_ARCH_SYS370 MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__370__) || defined(__THW_370__)
#   undef MSGHANDLE_ARCH_SYS370
#   define MSGHANDLE_ARCH_SYS370 MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_SYS370
#   define MSGHANDLE_ARCH_SYS370_AVAILABLE
#endif

#define MSGHANDLE_ARCH_SYS370_NAME "System/370"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_SYS370,MSGHANDLE_ARCH_SYS370_NAME)
