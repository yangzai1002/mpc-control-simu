/*
Copyright Rene Rivera 2013
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_DETAIL_ENDIAN_COMPAT_H
#define MSGHANDLE_PREDEF_DETAIL_ENDIAN_COMPAT_H

#include <msghandle/predef/other/endian.h>

#if MSGHANDLE_ENDIAN_BIG_BYTE
#   define MSGHANDLE_BIG_ENDIAN
#   define MSGHANDLE_BYTE_ORDER 4321
#endif
#if MSGHANDLE_ENDIAN_LITTLE_BYTE
#   define MSGHANDLE_LITTLE_ENDIAN
#   define MSGHANDLE_BYTE_ORDER 1234
#endif
#if MSGHANDLE_ENDIAN_LITTLE_WORD
#   define MSGHANDLE_PDP_ENDIAN
#   define MSGHANDLE_BYTE_ORDER 2134
#endif

#endif
