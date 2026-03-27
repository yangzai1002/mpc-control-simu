/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_COMPILER_COMPAQ_H
#define MSGHANDLE_PREDEF_COMPILER_COMPAQ_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_COMP_DEC`]

[@http://www.openvms.compaq.com/openvms/brochures/deccplus/ Compaq C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DECCXX`] [__predef_detection__]]
    [[`__DECC`] [__predef_detection__]]

    [[`__DECCXX_VER`] [V.R.P]]
    [[`__DECC_VER`] [V.R.P]]
    ]
 */

#define MSGHANDLE_COMP_DEC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DECC) || defined(__DECCXX)
#   if !defined(MSGHANDLE_COMP_DEC_DETECTION) && defined(__DECCXX_VER)
#       define MSGHANDLE_COMP_DEC_DETECTION MSGHANDLE_PREDEF_MAKE_10_VVRR0PP00(__DECCXX_VER)
#   endif
#   if !defined(MSGHANDLE_COMP_DEC_DETECTION) && defined(__DECC_VER)
#       define MSGHANDLE_COMP_DEC_DETECTION MSGHANDLE_PREDEF_MAKE_10_VVRR0PP00(__DECC_VER)
#   endif
#   if !defined(MSGHANDLE_COMP_DEC_DETECTION)
#       define MSGHANDLE_COM_DEC_DETECTION MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef MSGHANDLE_COMP_DEC_DETECTION
#   if defined(MSGHANDLE_PREDEF_DETAIL_COMP_DETECTED)
#       define MSGHANDLE_COMP_DEC_EMULATED MSGHANDLE_COMP_DEC_DETECTION
#   else
#       undef MSGHANDLE_COMP_DEC
#       define MSGHANDLE_COMP_DEC MSGHANDLE_COMP_DEC_DETECTION
#   endif
#   define MSGHANDLE_COMP_DEC_AVAILABLE
#   include <msghandle/predef/detail/comp_detected.h>
#endif

#define MSGHANDLE_COMP_DEC_NAME "Compaq C/C++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_DEC,MSGHANDLE_COMP_DEC_NAME)

#ifdef MSGHANDLE_COMP_DEC_EMULATED
#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_COMP_DEC_EMULATED,MSGHANDLE_COMP_DEC_NAME)
#endif
