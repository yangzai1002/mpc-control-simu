/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_HARDWARE_SIMD_PPC_H
#define MSGHANDLE_PREDEF_HARDWARE_SIMD_PPC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/hardware/simd/ppc/versions.h>

/*`
 [heading `MSGHANDLE_HW_SIMD_PPC`]

 The SIMD extension for PowerPC (*if detected*).
 Version number depends on the most recent detected extension.

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__VECTOR4DOUBLE__`] [__predef_detection__]]

     [[`__ALTIVEC__`] [__predef_detection__]]
     [[`__VEC__`] [__predef_detection__]]

     [[`__VSX__`] [__predef_detection__]]
     ]

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__VECTOR4DOUBLE__`] [MSGHANDLE_HW_SIMD_PPC_QPX_VERSION]]

     [[`__ALTIVEC__`] [MSGHANDLE_HW_SIMD_PPC_VMX_VERSION]]
     [[`__VEC__`] [MSGHANDLE_HW_SIMD_PPC_VMX_VERSION]]

     [[`__VSX__`] [MSGHANDLE_HW_SIMD_PPC_VSX_VERSION]]
     ]

 */

#define MSGHANDLE_HW_SIMD_PPC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#undef MSGHANDLE_HW_SIMD_PPC
#if !defined(MSGHANDLE_HW_SIMD_PPC) && defined(__VECTOR4DOUBLE__)
#   define MSGHANDLE_HW_SIMD_PPC MSGHANDLE_HW_SIMD_PPC_QPX_VERSION
#endif
#if !defined(MSGHANDLE_HW_SIMD_PPC) && defined(__VSX__)
#   define MSGHANDLE_HW_SIMD_PPC MSGHANDLE_HW_SIMD_PPC_VSX_VERSION
#endif
#if !defined(MSGHANDLE_HW_SIMD_PPC) && (defined(__ALTIVEC__) || defined(__VEC__))
#   define MSGHANDLE_HW_SIMD_PPC MSGHANDLE_HW_SIMD_PPC_VMX_VERSION
#endif

#if !defined(MSGHANDLE_HW_SIMD_PPC)
#   define MSGHANDLE_HW_SIMD_PPC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE
#else
#   define MSGHANDLE_HW_SIMD_PPC_AVAILABLE
#endif

#define MSGHANDLE_HW_SIMD_PPC_NAME "PPC SIMD"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_HW_SIMD_PPC, MSGHANDLE_HW_SIMD_PPC_NAME)
