/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_VACPP_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_VACPP_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_IBM`]

[@http://www.ibm.com/software/awdtools/xlcpp/ IBM VACPP Standard C++] library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IBMCPP__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_LIB_STD_IBM MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IBMCPP__)
#   undef MSGHANDLE_LIB_STD_IBM
#   define MSGHANDLE_LIB_STD_IBM MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_LIB_STD_IBM
#   define MSGHANDLE_LIB_STD_IBM_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_IBM_NAME "IBM VACPP"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_IBM,MSGHANDLE_LIB_STD_IBM_NAME)
