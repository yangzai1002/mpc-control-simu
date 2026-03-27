/*
Copyright Benjamin Worpitz 2018
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_NVCC_H
#define MSGHANDLE_PREDEF_COMPILER_NVCC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_NVCC`]

[@https://en.wikipedia.org/wiki/NVIDIA_CUDA_Compiler NVCC] compiler.
Version number available as major, minor, and patch beginning with version 7.5.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__NVCC__`] [__predef_detection__]]

    [[`__CUDACC_VER_MAJOR__`, `__CUDACC_VER_MINOR__`, `__CUDACC_VER_BUILD__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_NVCC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__NVCC__)
#   if !defined(__CUDACC_VER_MAJOR__) || !defined(__CUDACC_VER_MINOR__) || !defined(__CUDACC_VER_BUILD__)
#       define MSGHANDLE_COMP_NVCC_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   else
#       define MSGHANDLE_COMP_NVCC_DETECTION MSGHANDLE_VERSION_NUMBER(__CUDACC_VER_MAJOR__, __CUDACC_VER_MINOR__, __CUDACC_VER_BUILD__)
#   endif
#endif

#ifdef MSGHANDLE_COMP_NVCC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_NVCC_EMULATED MSGHANDLE_COMP_NVCC_DETECTION
#   else
#       undef MSGHANDLE_COMP_NVCC
#       define MSGHANDLE_COMP_NVCC MSGHANDLE_COMP_NVCC_DETECTION
#   endif
#   define MSGHANDLE_COMP_NVCC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_NVCC_NAME "NVCC"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_NVCC,MSGHANDLE_COMP_NVCC_NAME)

#ifdef MSGHANDLE_COMP_NVCC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_NVCC_EMULATED,MSGHANDLE_COMP_NVCC_NAME)
#endif
