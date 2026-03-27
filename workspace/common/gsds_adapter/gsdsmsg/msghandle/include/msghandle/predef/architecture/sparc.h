/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_SPARC_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_SPARC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_SPARC`]

[@http://en.wikipedia.org/wiki/SPARC SPARC] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sparc__`] [__predef_detection__]]
    [[`__sparc`] [__predef_detection__]]

    [[`__sparcv9`] [9.0.0]]
    [[`__sparcv8`] [8.0.0]]
    ]
 */

#define MSGHANDLE_ARCH_SPARC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sparc__) || defined(__sparc)
#   undef MSGHANDLE_ARCH_SPARC
#   if !defined(MSGHANDLE_ARCH_SPARC) && defined(__sparcv9)
#       define MSGHANDLE_ARCH_SPARC MSGHANDLE_VERSION_NUMBER(9,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_SPARC) && defined(__sparcv8)
#       define MSGHANDLE_ARCH_SPARC MSGHANDLE_VERSION_NUMBER(8,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_SPARC)
#       define MSGHANDLE_ARCH_SPARC MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_ARCH_SPARC
#   define MSGHANDLE_ARCH_SPARC_AVAILABLE
#endif

#define MSGHANDLE_ARCH_SPARC_NAME "SPARC"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_SPARC,MSGHANDLE_ARCH_SPARC_NAME)
