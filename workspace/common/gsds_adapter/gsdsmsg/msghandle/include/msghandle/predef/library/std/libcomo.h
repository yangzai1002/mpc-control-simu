/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_LIBCOMO_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_LIBCOMO_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_COMO`]

[@http://www.comeaucomputing.com/libcomo/ Comeau Computing] Standard C++ Library.
Version number available as major.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__LIBCOMO__`] [__predef_detection__]]

    [[`__LIBCOMO_VERSION__`] [V.0.0]]
    ]
 */

#define MSGHANDLE_LIB_STD_COMO MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__LIBCOMO__)
#   undef MSGHANDLE_LIB_STD_COMO
#   define MSGHANDLE_LIB_STD_COMO MSGHANDLE_VERSION_NUMBER(__LIBCOMO_VERSION__,0,0)
#endif

#if MSGHANDLE_LIB_STD_COMO
#   define MSGHANDLE_LIB_STD_COMO_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_COMO_NAME "Comeau Computing"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_COMO,MSGHANDLE_LIB_STD_COMO_NAME)
