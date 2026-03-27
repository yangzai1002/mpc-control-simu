/*
  Copyright 2017 James E. King, III
  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_CLOUDABI_H
#define MSGHANDLE_PREDEF_PLAT_CLOUDABI_H

#include <msghandle/predef/version_number.h>
#include <msghandle/predef/make.h>

/*`
[heading `MSGHANDLE_PLAT_CLOUDABI`]

[@https://github.com/NuxiNL/cloudabi CloudABI] platform.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CloudABI__`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_PLAT_CLOUDABI MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__CloudABI__)
#   undef MSGHANDLE_PLAT_CLOUDABI
#   define MSGHANDLE_PLAT_CLOUDABI MSGHANDLE_VERSION_NUMBER_AVAILABLE
#endif

#if MSGHANDLE_PLAT_CLOUDABI
#   define MSGHANDLE_PLAT_CLOUDABI_AVAILABLE
#   include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_CLOUDABI_NAME "CloudABI"

#endif

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_CLOUDABI,MSGHANDLE_PLAT_CLOUDABI_NAME)
