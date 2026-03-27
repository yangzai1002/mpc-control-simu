/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_RS6K_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_RS6K_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_RS6000`]

[@http://en.wikipedia.org/wiki/RS/6000 RS/6000] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__THW_RS6000`] [__predef_detection__]]
    [[`_IBMR2`] [__predef_detection__]]
    [[`_POWER`] [__predef_detection__]]
    [[`_ARCH_PWR`] [__predef_detection__]]
    [[`_ARCH_PWR2`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_ARCH_RS6000 MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__THW_RS6000) || defined(_IBMR2) || \
    defined(_POWER) || defined(_ARCH_PWR) || \
    defined(_ARCH_PWR2)
#   undef MSGHANDLE_ARCH_RS6000
#   define MSGHANDLE_ARCH_RS6000 MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_RS6000
#   define MSGHANDLE_ARCH_RS6000_AVAILABLE
#endif

#define MSGHANDLE_ARCH_RS6000_NAME "RS/6000"

#define MSGHANDLE_ARCH_PWR MSGHANDLE_ARCH_RS6000

#if MSGHANDLE_ARCH_PWR
#   define MSGHANDLE_ARCH_PWR_AVAILABLE
#endif

#define MSGHANDLE_ARCH_PWR_NAME MSGHANDLE_ARCH_RS6000_NAME

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_RS6000,MSGHANDLE_ARCH_RS6000_NAME)
