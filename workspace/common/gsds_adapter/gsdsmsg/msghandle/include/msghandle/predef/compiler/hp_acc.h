/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_HP_ACC_H
#define MSGHANDLE_PREDEF_COMPILER_HP_ACC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_HPACC`]

HP aC++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__HP_aCC`] [__predef_detection__]]

    [[`__HP_aCC`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_HPACC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__HP_aCC)
#   if !defined(MSGHANDLE_COMP_HPACC_DETECTION) && (__HP_aCC > 1)
#       define MSGHANDLE_COMP_HPACC_DETECTION MSGHANDLE_PREDEF_MAKE_10_VVRRPP(__HP_aCC)
#   endif
#   if !defined(MSGHANDLE_COMP_HPACC_DETECTION)
#       define MSGHANDLE_COMP_HPACC_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_HPACC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_HPACC_EMULATED MSGHANDLE_COMP_HPACC_DETECTION
#   else
#       undef MSGHANDLE_COMP_HPACC
#       define MSGHANDLE_COMP_HPACC MSGHANDLE_COMP_HPACC_DETECTION
#   endif
#   define MSGHANDLE_COMP_HPACC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_HPACC_NAME "HP aC++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_HPACC,MSGHANDLE_COMP_HPACC_NAME)

#ifdef MSGHANDLE_COMP_HPACC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_HPACC_EMULATED,MSGHANDLE_COMP_HPACC_NAME)
#endif
