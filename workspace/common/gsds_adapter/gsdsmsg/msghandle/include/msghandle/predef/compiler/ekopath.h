/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_EKOPATH_H
#define MSGHANDLE_PREDEF_COMPILER_EKOPATH_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_PATH`]

[@http://en.wikipedia.org/wiki/PathScale EKOpath] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__PATHCC__`] [__predef_detection__]]

    [[`__PATHCC__`, `__PATHCC_MINOR__`, `__PATHCC_PATCHLEVEL__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_PATH MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__PATHCC__)
#   define MSGHANDLE_COMP_PATH_DETECTION \
        MSGHANDLE_VERSION_NUMBER(__PATHCC__,__PATHCC_MINOR__,__PATHCC_PATCHLEVEL__)
#endif

#ifdef MSGHANDLE_COMP_PATH_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_PATH_EMULATED MSGHANDLE_COMP_PATH_DETECTION
#   else
#       undef MSGHANDLE_COMP_PATH
#       define MSGHANDLE_COMP_PATH MSGHANDLE_COMP_PATH_DETECTION
#   endif
#   define MSGHANDLE_COMP_PATH_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_PATH_NAME "EKOpath"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_PATH,MSGHANDLE_COMP_PATH_NAME)

#ifdef MSGHANDLE_COMP_PATH_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_PATH_EMULATED,MSGHANDLE_COMP_PATH_NAME)
#endif
