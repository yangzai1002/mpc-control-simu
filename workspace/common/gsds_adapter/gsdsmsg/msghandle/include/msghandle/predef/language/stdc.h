/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_LANGUAGE_STDC_H
#define MSGHANDLE_PREDEF_LANGUAGE_STDC_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_LANG_STDC`]

[@http://en.wikipedia.org/wiki/C_(programming_language) Standard C] language.
If available, the year of the standard is detected as YYYY.MM.1 from the Epoc date.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__STDC__`] [__predef_detection__]]

    [[`__STDC_VERSION__`] [V.R.P]]
    ]
 */

#define MSGHANDLE_LANG_STDC MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__STDC__)
#   undef MSGHANDLE_LANG_STDC
#   if defined(__STDC_VERSION__)
#       if (__STDC_VERSION__ > 100)
#           define MSGHANDLE_LANG_STDC MSGHANDLE_PREDEF_MAKE_YYYYMM(__STDC_VERSION__)
#       else
#           define MSGHANDLE_LANG_STDC MSGHANDLE_VERSION_NUMBER_AVAILABLE
#       endif
#   else
#       define MSGHANDLE_LANG_STDC MSGHANDLE_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if MSGHANDLE_LANG_STDC
#   define MSGHANDLE_LANG_STDC_AVAILABLE
#endif

#define MSGHANDLE_LANG_STDC_NAME "Standard C"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_LANG_STDC,MSGHANDLE_LANG_STDC_NAME)
