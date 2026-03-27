/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LIBRARY_STD_STLPORT_H
#define MSGHANDLE_PREDEF_LIBRARY_STD_STLPORT_H

#include <msghandle/predef/library/std/_prefix.h>

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LIB_STD_STLPORT`]

[@http://sourceforge.net/projects/stlport/ STLport Standard C++] library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SGI_STL_PORT`] [__predef_detection__]]
    [[`_STLPORT_VERSION`] [__predef_detection__]]

    [[`_STLPORT_MAJOR`, `_STLPORT_MINOR`, `_STLPORT_PATCHLEVEL`] [V.R.P]]
    [[`_STLPORT_VERSION`] [V.R.P]]
    [[`__SGI_STL_PORT`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LIB_STD_STLPORT MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#   undef MSGHANDLE_LIB_STD_STLPORT
#   if !defined(MSGHANDLE_LIB_STD_STLPORT) && defined(_STLPORT_MAJOR)
#       define MSGHANDLE_LIB_STD_STLPORT \
            MSGHANDLE_VERSION_NUMBER(_STLPORT_MAJOR,_STLPORT_MINOR,_STLPORT_PATCHLEVEL)
#   endif
#   if !defined(MSGHANDLE_LIB_STD_STLPORT) && defined(_STLPORT_VERSION)
#       define MSGHANDLE_LIB_STD_STLPORT MSGHANDLE_PREDEF_MAKE_0X_VRP(_STLPORT_VERSION)
#   endif
#   if !defined(MSGHANDLE_LIB_STD_STLPORT)
#       define MSGHANDLE_LIB_STD_STLPORT MSGHANDLE_PREDEF_MAKE_0X_VRP(__SGI_STL_PORT)
#   endif
#endif

#if MSGHANDLE_LIB_STD_STLPORT
#   define MSGHANDLE_LIB_STD_STLPORT_AVAILABLE
#endif

#define MSGHANDLE_LIB_STD_STLPORT_NAME "STLport"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LIB_STD_STLPORT,MSGHANDLE_LIB_STD_STLPORT_NAME)
