/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LANGUAGE_OBJC_H
#define MSGHANDLE_PREDEF_LANGUAGE_OBJC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LANG_OBJC`]

[@http://en.wikipedia.org/wiki/Objective-C Objective-C] language.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__OBJC__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_LANG_OBJC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__OBJC__)
#   undef MSGHANDLE_LANG_OBJC
#   define MSGHANDLE_LANG_OBJC MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_LANG_OBJC
#   define MSGHANDLE_LANG_OBJC_AVAILABLE
#endif

#define MSGHANDLE_LANG_OBJC_NAME "Objective-C"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LANG_OBJC,MSGHANDLE_LANG_OBJC_NAME)
