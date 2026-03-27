/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_CONVEX_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_CONVEX_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_CONVEX`]

[@http://en.wikipedia.org/wiki/Convex_Computer Convex Computer] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__convex__`] [__predef_detection__]]

    [[`__convex_c1__`] [1.0.0]]
    [[`__convex_c2__`] [2.0.0]]
    [[`__convex_c32__`] [3.2.0]]
    [[`__convex_c34__`] [3.4.0]]
    [[`__convex_c38__`] [3.8.0]]
    ]
 */

#define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__convex__)
#   undef MSGHANDLE_ARCH_CONVEX
#   if !defined(MSGHANDLE_ARCH_CONVEX) && defined(__convex_c1__)
#       define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER(1,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_CONVEX) && defined(__convex_c2__)
#       define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER(2,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_CONVEX) && defined(__convex_c32__)
#       define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER(3,2,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_CONVEX) && defined(__convex_c34__)
#       define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER(3,4,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_CONVEX) && defined(__convex_c38__)
#       define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER(3,8,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_CONVEX)
#       define MSGHANDLE_ARCH_CONVEX MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_ARCH_CONVEX
#   define MSGHANDLE_ARCH_CONVEX_AVAILABLE
#endif

#define MSGHANDLE_ARCH_CONVEX_NAME "Convex Computer"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_CONVEX,MSGHANDLE_ARCH_CONVEX_NAME)
