/*
Copyright Rene Rivera 2008-2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_INTEL_H
#define MSGHANDLE_PREDEF_COMPILER_INTEL_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_INTEL`]

[@http://en.wikipedia.org/wiki/Intel_C%2B%2B Intel C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__INTEL_COMPILER`] [__predef_detection__]]
    [[`__ICL`] [__predef_detection__]]
    [[`__ICC`] [__predef_detection__]]
    [[`__ECC`] [__predef_detection__]]

    [[`__INTEL_COMPILER`] [V.R]]
    [[`__INTEL_COMPILER` and `__INTEL_COMPILER_UPDATE`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_INTEL MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || \
    defined(__ECC)
/*`
[note Because of an Intel mistake in the release version numbering when
`__INTEL_COMPILER` is `9999` it is detected as version 12.1.0.]
 */
#   if !defined(MSGHANDLE_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 9999)
#       define MSGHANDLE_COMP_INTEL_DETECTION MSGHANDLE_VERSION_NUMBER(12,1,0)
#   endif
#   if !defined(MSGHANDLE_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE)
#       define MSGHANDLE_COMP_INTEL_DETECTION MSGHANDLE_VERSION_NUMBER( \
            MSGHANDLE_VERSION_NUMBER_MAJOR(MSGHANDLE_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)), \
            MSGHANDLE_VERSION_NUMBER_MINOR(MSGHANDLE_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)), \
            __INTEL_COMPILER_UPDATE)
#   endif
#   if !defined(MSGHANDLE_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER)
#       define MSGHANDLE_COMP_INTEL_DETECTION MSGHANDLE_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)
#   endif
#   if !defined(MSGHANDLE_COMP_INTEL_DETECTION)
#       define MSGHANDLE_COMP_INTEL_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_INTEL_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_INTEL_EMULATED MSGHANDLE_COMP_INTEL_DETECTION
#   else
#       undef MSGHANDLE_COMP_INTEL
#       define MSGHANDLE_COMP_INTEL MSGHANDLE_COMP_INTEL_DETECTION
#   endif
#   define MSGHANDLE_COMP_INTEL_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_INTEL_NAME "Intel C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_INTEL,MSGHANDLE_COMP_INTEL_NAME)

#ifdef MSGHANDLE_COMP_INTEL_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_INTEL_EMULATED,MSGHANDLE_COMP_INTEL_NAME)
#endif
