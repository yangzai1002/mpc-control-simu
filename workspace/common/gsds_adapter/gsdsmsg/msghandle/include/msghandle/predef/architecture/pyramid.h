/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_PYRAMID_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_PYRAMID_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_PYRAMID`]

Pyramid 9810 architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`pyr`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_ARCH_PYRAMID MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(pyr)
#   undef MSGHANDLE_ARCH_PYRAMID
#   define MSGHANDLE_ARCH_PYRAMID MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_PYRAMID
#   define MSGHANDLE_ARCH_PYRAMID_AVAILABLE
#endif

#define MSGHANDLE_ARCH_PYRAMID_NAME "Pyramid 9810"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_PYRAMID,MSGHANDLE_ARCH_PYRAMID_NAME)
