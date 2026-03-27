/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_PGI_H
#define MSGHANDLE_PREDEF_COMPILER_PGI_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_PGI`]

[@http://en.wikipedia.org/wiki/The_Portland_Group Portland Group C/C++] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__PGI`] [__predef_detection__]]

    [[`__PGIC__`, `__PGIC_MINOR__`, `__PGIC_PATCHLEVEL__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_PGI MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__PGI)
#   if !defined(MSGHANDLE_COMP_PGI_DETECTION) && (defined(__PGIC__) && defined(__PGIC_MINOR__) && defined(__PGIC_PATCHLEVEL__))
#       define MSGHANDLE_COMP_PGI_DETECTION MSGHANDLE_VERSION_NUMBER(__PGIC__,__PGIC_MINOR__,__PGIC_PATCHLEVEL__)
#   endif
#   if !defined(MSGHANDLE_COMP_PGI_DETECTION)
#       define MSGHANDLE_COMP_PGI_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_PGI_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_PGI_EMULATED MSGHANDLE_COMP_PGI_DETECTION
#   else
#       undef MSGHANDLE_COMP_PGI
#       define MSGHANDLE_COMP_PGI MSGHANDLE_COMP_PGI_DETECTION
#   endif
#   define MSGHANDLE_COMP_PGI_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_PGI_NAME "Portland Group C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_PGI,MSGHANDLE_COMP_PGI_NAME)

#ifdef MSGHANDLE_COMP_PGI_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_PGI_EMULATED,MSGHANDLE_COMP_PGI_NAME)
#endif
