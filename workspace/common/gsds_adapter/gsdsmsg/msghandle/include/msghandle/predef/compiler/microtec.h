/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_MICROTEC_H
#define MSGHANDLE_PREDEF_COMPILER_MICROTEC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_MRI`]

[@http://www.mentor.com/microtec/ Microtec C/C++] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_MRI`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_COMP_MRI MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_MRI)
#   define MSGHANDLE_COMP_MRI_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#ifdef MSGHANDLE_COMP_MRI_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_MRI_EMULATED MSGHANDLE_COMP_MRI_DETECTION
#   else
#       undef MSGHANDLE_COMP_MRI
#       define MSGHANDLE_COMP_MRI MSGHANDLE_COMP_MRI_DETECTION
#   endif
#   define MSGHANDLE_COMP_MRI_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_MRI_NAME "Microtec C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_MRI,MSGHANDLE_COMP_MRI_NAME)

#ifdef MSGHANDLE_COMP_MRI_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_MRI_EMULATED,MSGHANDLE_COMP_MRI_NAME)
#endif
