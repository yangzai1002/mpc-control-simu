/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_HARDWARE_SIMD_ARM_H
#define MSGHANDLE_PREDEF_HARDWARE_SIMD_ARM_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/hardware/simd/arm/versions.h>

/*`
 [heading `MSGHANDLE_HW_SIMD_ARM`]

 The SIMD extension for ARM (*if detected*).
 Version number depends on the most recent detected extension.

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__ARM_NEON__`] [__predef_detection__]]
     [[`__aarch64__`] [__predef_detection__]]
     [[`_M_ARM`] [__predef_detection__]]
     [[`_M_ARM64`] [__predef_detection__]]
     ]

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__ARM_NEON__`] [MSGHANDLE_HW_SIMD_ARM_NEON_VERSION]]
     [[`__aarch64__`] [MSGHANDLE_HW_SIMD_ARM_NEON_VERSION]]
     [[`_M_ARM`] [MSGHANDLE_HW_SIMD_ARM_NEON_VERSION]]
     [[`_M_ARM64`] [MSGHANDLE_HW_SIMD_ARM_NEON_VERSION]]
     ]

 */

#define MSGHANDLE_HW_SIMD_ARM MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#undef MSGHANDLE_HW_SIMD_ARM
#if !defined(MSGHANDLE_HW_SIMD_ARM) && (defined(__ARM_NEON__) || defined(__aarch64__) || defined (_M_ARM) || defined (_M_ARM64))
#   define MSGHANDLE_HW_SIMD_ARM MSGHANDLE_HW_SIMD_ARM_NEON_VERSION
#endif

#if !defined(MSGHANDLE_HW_SIMD_ARM)
#   define MSGHANDLE_HW_SIMD_ARM MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE
#else
#   define MSGHANDLE_HW_SIMD_ARM_AVAILABLE
#endif

#define MSGHANDLE_HW_SIMD_ARM_NAME "ARM SIMD"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_HW_SIMD_ARM, MSGHANDLE_HW_SIMD_ARM_NAME)
