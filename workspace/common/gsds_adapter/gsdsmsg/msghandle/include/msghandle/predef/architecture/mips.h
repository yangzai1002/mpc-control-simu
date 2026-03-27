/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_MIPS_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_MIPS_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_MIPS`]

[@http://en.wikipedia.org/wiki/MIPS_architecture MIPS] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__mips__`] [__predef_detection__]]
    [[`__mips`] [__predef_detection__]]
    [[`__MIPS__`] [__predef_detection__]]

    [[`__mips`] [V.0.0]]
    [[`_MIPS_ISA_MIPS1`] [1.0.0]]
    [[`_R3000`] [1.0.0]]
    [[`_MIPS_ISA_MIPS2`] [2.0.0]]
    [[`__MIPS_ISA2__`] [2.0.0]]
    [[`_R4000`] [2.0.0]]
    [[`_MIPS_ISA_MIPS3`] [3.0.0]]
    [[`__MIPS_ISA3__`] [3.0.0]]
    [[`_MIPS_ISA_MIPS4`] [4.0.0]]
    [[`__MIPS_ISA4__`] [4.0.0]]
    ]
 */

#define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__mips__) || defined(__mips) || \
    defined(__MIPS__)
#   undef MSGHANDLE_ARCH_MIPS
#   if !defined(MSGHANDLE_ARCH_MIPS) && (defined(__mips))
#       define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER(__mips,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_MIPS) && (defined(_MIPS_ISA_MIPS1) || defined(_R3000))
#       define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER(1,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_MIPS) && (defined(_MIPS_ISA_MIPS2) || defined(__MIPS_ISA2__) || defined(_R4000))
#       define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER(2,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_MIPS) && (defined(_MIPS_ISA_MIPS3) || defined(__MIPS_ISA3__))
#       define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER(3,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_MIPS) && (defined(_MIPS_ISA_MIPS4) || defined(__MIPS_ISA4__))
#       define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER(4,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_MIPS)
#       define MSGHANDLE_ARCH_MIPS MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_ARCH_MIPS
#   define MSGHANDLE_ARCH_MIPS_AVAILABLE
#endif

#define MSGHANDLE_ARCH_MIPS_NAME "MIPS"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_MIPS,MSGHANDLE_ARCH_MIPS_NAME)
