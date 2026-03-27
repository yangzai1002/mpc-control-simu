/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_PARISC_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_PARISC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_PARISC`]

[@http://en.wikipedia.org/wiki/PA-RISC_family HP/PA RISC] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__hppa__`] [__predef_detection__]]
    [[`__hppa`] [__predef_detection__]]
    [[`__HPPA__`] [__predef_detection__]]

    [[`_PA_RISC1_0`] [1.0.0]]
    [[`_PA_RISC1_1`] [1.1.0]]
    [[`__HPPA11__`] [1.1.0]]
    [[`__PA7100__`] [1.1.0]]
    [[`_PA_RISC2_0`] [2.0.0]]
    [[`__RISC2_0__`] [2.0.0]]
    [[`__HPPA20__`] [2.0.0]]
    [[`__PA8000__`] [2.0.0]]
    ]
 */

#define MSGHANDLE_ARCH_PARISC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__hppa__) || defined(__hppa) || defined(__HPPA__)
#   undef MSGHANDLE_ARCH_PARISC
#   if !defined(MSGHANDLE_ARCH_PARISC) && (defined(_PA_RISC1_0))
#       define MSGHANDLE_ARCH_PARISC MSGHANDLE_VERSION_NUMBER(1,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_PARISC) && (defined(_PA_RISC1_1) || defined(__HPPA11__) || defined(__PA7100__))
#       define MSGHANDLE_ARCH_PARISC MSGHANDLE_VERSION_NUMBER(1,1,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_PARISC) && (defined(_PA_RISC2_0) || defined(__RISC2_0__) || defined(__HPPA20__) || defined(__PA8000__))
#       define MSGHANDLE_ARCH_PARISC MSGHANDLE_VERSION_NUMBER(2,0,0)
#   endif
#   if !defined(MSGHANDLE_ARCH_PARISC)
#       define MSGHANDLE_ARCH_PARISC MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_ARCH_PARISC
#   define MSGHANDLE_ARCH_PARISC_AVAILABLE
#endif

#define MSGHANDLE_ARCH_PARISC_NAME "HP/PA RISC"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_PARISC,MSGHANDLE_ARCH_PARISC_NAME)
