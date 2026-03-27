/*
Copyright Benjamin Worpitz 2018
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_ARCHITECTURE_PTX_H
#define MSGHANDLE_PREDEF_ARCHITECTURE_PTX_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_ARCH_PTX`]

[@https://en.wikipedia.org/wiki/Parallel_Thread_Execution PTX] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CUDA_ARCH__`] [__predef_detection__]]

    [[`__CUDA_ARCH__`] [V.R.0]]
    ]
 */

#define MSGHANDLE_ARCH_PTX MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__CUDA_ARCH__)
#   undef MSGHANDLE_ARCH_PTX
#   define MSGHANDLE_ARCH_PTX MSGHANDLE_PREDEF_MAKE_10_VR0(__CUDA_ARCH__)
#endif

#if MSGHANDLE_ARCH_PTX
#   define MSGHANDLE_ARCH_PTX_AVAILABLE
#endif

#define MSGHANDLE_ARCH_PTX_NAME "PTX"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_ARCH_PTX,MSGHANDLE_ARCH_PTX_NAME)
