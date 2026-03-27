/*
Copyright Rene Rivera 2013-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_BLACKFIN_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_BLACKFIN_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_BLACKFIN`]

Blackfin Processors from Analog Devices.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__bfin__`] [__predef_detection__]]
    [[`__BFIN__`] [__predef_detection__]]
    [[`bfin`] [__predef_detection__]]
    [[`BFIN`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_ARCH_BLACKFIN MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__bfin__) || defined(__BFIN__) || \
    defined(bfin) || defined(BFIN)
#   undef MSGHANDLE_ARCH_BLACKFIN
#   define MSGHANDLE_ARCH_BLACKFIN MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_BLACKFIN
#   define MSGHANDLE_ARCH_BLACKFIN_AVAILABLE
#endif

#define MSGHANDLE_ARCH_BLACKFIN_NAME "Blackfin"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_BLACKFIN,MSGHANDLE_ARCH_BLACKFIN_NAME)
