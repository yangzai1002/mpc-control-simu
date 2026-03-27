/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#include <msghandle/predef/hardware/simd/x86.h>
#include <msghandle/predef/hardware/simd/x86_amd.h>
#include <msghandle/predef/hardware/simd/arm.h>
#include <msghandle/predef/hardware/simd/ppc.h>

#ifndef MSGHANDLE_PREDEF_HARDWARE_SIMD_H
#define MSGHANDLE_PREDEF_HARDWARE_SIMD_H

#include <msghandle/predef/version_number.h>

/*`
 [section Using the `MSGHANDLE_HW_SIMD_*` predefs]
 [include ../doc/hardware_simd.qbk]
 [endsect]

 [/ --------------------------- ]

 [section `MSGHANDLE_HW_SIMD_*`]

 [heading `MSGHANDLE_HW_SIMD`]

 The SIMD extension detected for a specific architectures.
 Version number depends on the detected extension.

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`MSGHANDLE_HW_SIMD_X86_AVAILABLE`] [__predef_detection__]]
     [[`MSGHANDLE_HW_SIMD_X86_AMD_AVAILABLE`] [__predef_detection__]]
     [[`MSGHANDLE_HW_SIMD_ARM_AVAILABLE`] [__predef_detection__]]
     [[`MSGHANDLE_HW_SIMD_PPC_AVAILABLE`] [__predef_detection__]]
     ]

 [include ../include/msghandle/predef/hardware/simd/x86.h]
 [include ../include/msghandle/predef/hardware/simd/x86_amd.h]
 [include ../include/msghandle/predef/hardware/simd/arm.h]
 [include ../include/msghandle/predef/hardware/simd/ppc.h]

 [endsect]

 [/ --------------------------- ]

 [section `MSGHANDLE_HW_SIMD_X86_*_VERSION`]
 [include ../include/msghandle/predef/hardware/simd/x86/versions.h]
 [endsect]

 [section `MSGHANDLE_HW_SIMD_X86_AMD_*_VERSION`]
 [include ../include/msghandle/predef/hardware/simd/x86_amd/versions.h]
 [endsect]

 [section `MSGHANDLE_HW_SIMD_ARM_*_VERSION`]
 [include ../include/msghandle/predef/hardware/simd/arm/versions.h]
 [endsect]

 [section `MSGHANDLE_HW_SIMD_PPC_*_VERSION`]
 [include ../include/msghandle/predef/hardware/simd/ppc/versions.h]
 [endsect]

 */

// We check if SIMD extension of multiples architectures have been detected,
// if yes, then this is an error!
//
// NOTE: _X86_AMD implies _X86, so there is no need to check for it here!
//
#if defined(MSGHANDLE_HW_SIMD_ARM_AVAILABLE) && defined(MSGHANDLE_HW_SIMD_PPC_AVAILABLE) ||\
    defined(MSGHANDLE_HW_SIMD_ARM_AVAILABLE) && defined(MSGHANDLE_HW_SIMD_X86_AVAILABLE) ||\
    defined(MSGHANDLE_HW_SIMD_PPC_AVAILABLE) && defined(MSGHANDLE_HW_SIMD_X86_AVAILABLE)
#   error "Multiple SIMD architectures detected, this cannot happen!"
#endif

#if defined(MSGHANDLE_HW_SIMD_X86_AVAILABLE) && defined(MSGHANDLE_HW_SIMD_X86_AMD_AVAILABLE)
    // If both standard _X86 and _X86_AMD are available,
    // then take the biggest version of the two!
#   if MSGHANDLE_HW_SIMD_X86 >= MSGHANDLE_HW_SIMD_X86_AMD
#      define MSGHANDLE_HW_SIMD MSGHANDLE_HW_SIMD_X86
#   else
#      define MSGHANDLE_HW_SIMD MSGHANDLE_HW_SIMD_X86_AMD
#   endif
#endif

#if !defined(MSGHANDLE_HW_SIMD)
    // At this point, only one of these two is defined
#   if defined(MSGHANDLE_HW_SIMD_X86_AVAILABLE)
#      define MSGHANDLE_HW_SIMD MSGHANDLE_HW_SIMD_X86
#   endif
#   if defined(MSGHANDLE_HW_SIMD_X86_AMD_AVAILABLE)
#      define MSGHANDLE_HW_SIMD MSGHANDLE_HW_SIMD_X86_AMD
#   endif
#endif

#if defined(MSGHANDLE_HW_SIMD_ARM_AVAILABLE)
#   define MSGHANDLE_HW_SIMD MSGHANDLE_HW_SIMD_ARM
#endif

#if defined(MSGHANDLE_HW_SIMD_PPC_AVAILABLE)
#   define MSGHANDLE_HW_SIMD MSGHANDLE_HW_SIMD_PPC
#endif

#if defined(MSGHANDLE_HW_SIMD)
#   define MSGHANDLE_HW_SIMD_AVAILABLE
#else
#   define MSGHANDLE_HW_SIMD MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE
#endif

#define MSGHANDLE_HW_SIMD_NAME "Hardware SIMD"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_HW_SIMD, MSGHANDLE_HW_SIMD_NAME)
