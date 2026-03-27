/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_LLVM_H
#define MSGHANDLE_PREDEF_COMPILER_LLVM_H

/* Other compilers that emulate this one need to be detected first. */

#include <msghandle/predef/compiler/clang.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_LLVM`]

[@http://en.wikipedia.org/wiki/LLVM LLVM] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__llvm__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_COMP_LLVM MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__llvm__)
#   define MSGHANDLE_COMP_LLVM_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#ifdef MSGHANDLE_COMP_LLVM_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_LLVM_EMULATED MSGHANDLE_COMP_LLVM_DETECTION
#   else
#       undef MSGHANDLE_COMP_LLVM
#       define MSGHANDLE_COMP_LLVM MSGHANDLE_COMP_LLVM_DETECTION
#   endif
#   define MSGHANDLE_COMP_LLVM_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_LLVM_NAME "LLVM"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_LLVM,MSGHANDLE_COMP_LLVM_NAME)

#ifdef MSGHANDLE_COMP_LLVM_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_LLVM_EMULATED,MSGHANDLE_COMP_LLVM_NAME)
#endif
