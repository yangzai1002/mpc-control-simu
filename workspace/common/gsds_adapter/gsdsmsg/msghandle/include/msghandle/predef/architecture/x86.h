/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#include <msghandle/predef/architecture/x86/32.h>
#include <msghandle/predef/architecture/x86/64.h>

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_X86_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_X86_H

/*`
[heading `MSGHANDLE_ARCH_X86`]

[@http://en.wikipedia.org/wiki/X86 Intel x86] architecture. This is
a category to indicate that either `MSGHANDLE_ARCH_X86_32` or
`MSGHANDLE_ARCH_X86_64` is detected.
 */

#define MSGHANDLE_ARCH_X86 MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if MSGHANDLE_ARCH_X86_32 || MSGHANDLE_ARCH_X86_64
#   undef MSGHANDLE_ARCH_X86
#   define MSGHANDLE_ARCH_X86 MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_ARCH_X86
#   define MSGHANDLE_ARCH_X86_AVAILABLE
#endif

#define MSGHANDLE_ARCH_X86_NAME "Intel x86"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_X86,MSGHANDLE_ARCH_X86_NAME)
