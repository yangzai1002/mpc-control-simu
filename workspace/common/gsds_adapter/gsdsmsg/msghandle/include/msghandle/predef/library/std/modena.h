/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_MODENA_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_MODENA_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_MSIPL`]

[@http://modena.us/ Modena Software Lib++] Standard C++ Library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`MSIPL_COMPILE_H`] [__predef_detection__]]
    [[`__MSIPL_COMPILE_H`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_LIB_STD_MSIPL MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(MSIPL_COMPILE_H) || defined(__MSIPL_COMPILE_H)
#   undef MSGHANDLE_LIB_STD_MSIPL
#   define MSGHANDLE_LIB_STD_MSIPL MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_LIB_STD_MSIPL
#   define MSGHANDLE_LIB_STD_MSIPL_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_MSIPL_NAME "Modena Software Lib++"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_MSIPL,MSGHANDLE_LIB_STD_MSIPL_NAME)
