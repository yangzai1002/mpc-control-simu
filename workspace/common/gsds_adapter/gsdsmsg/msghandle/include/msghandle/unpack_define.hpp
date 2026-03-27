/*
 * MessagePack unpacking routine template
 *
 * Copyright (C) 2008-2010 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef MSGHANDLE_UNPACK_DEFINE_HPP
#define MSGHANDLE_UNPACK_DEFINE_HPP

#include "msghandle/sysdep.hpp"

#ifndef MSGHANDLE_EMBED_STACK_SIZE
#define MSGHANDLE_EMBED_STACK_SIZE 32
#endif


typedef enum {
    MSGHANDLE_CS_HEADER            = 0x00,  // nil

    //MSGHANDLE_CS_                = 0x01,
    //MSGHANDLE_CS_                = 0x02,  // false
    //MSGHANDLE_CS_                = 0x03,  // true

    MSGHANDLE_CS_BIN_8             = 0x04,
    MSGHANDLE_CS_BIN_16            = 0x05,
    MSGHANDLE_CS_BIN_32            = 0x06,

    MSGHANDLE_CS_EXT_8             = 0x07,
    MSGHANDLE_CS_EXT_16            = 0x08,
    MSGHANDLE_CS_EXT_32            = 0x09,

    MSGHANDLE_CS_FLOAT             = 0x0a,
    MSGHANDLE_CS_DOUBLE            = 0x0b,
    MSGHANDLE_CS_UINT_8            = 0x0c,
    MSGHANDLE_CS_UINT_16           = 0x0d,
    MSGHANDLE_CS_UINT_32           = 0x0e,
    MSGHANDLE_CS_UINT_64           = 0x0f,
    MSGHANDLE_CS_INT_8             = 0x10,
    MSGHANDLE_CS_INT_16            = 0x11,
    MSGHANDLE_CS_INT_32            = 0x12,
    MSGHANDLE_CS_INT_64            = 0x13,

    MSGHANDLE_CS_FIXEXT_1          = 0x14,
    MSGHANDLE_CS_FIXEXT_2          = 0x15,
    MSGHANDLE_CS_FIXEXT_4          = 0x16,
    MSGHANDLE_CS_FIXEXT_8          = 0x17,
    MSGHANDLE_CS_FIXEXT_16         = 0x18,

    MSGHANDLE_CS_STR_8             = 0x19, // str8
    MSGHANDLE_CS_STR_16            = 0x1a, // str16
    MSGHANDLE_CS_STR_32            = 0x1b, // str32
    MSGHANDLE_CS_ARRAY_16          = 0x1c,
    MSGHANDLE_CS_ARRAY_32          = 0x1d,
    MSGHANDLE_CS_MAP_16            = 0x1e,
    MSGHANDLE_CS_MAP_32            = 0x1f,

    //MSGHANDLE_ACS_BIG_INT_VALUE,
    //MSGHANDLE_ACS_BIG_FLOAT_VALUE,
    MSGHANDLE_ACS_STR_VALUE,
    MSGHANDLE_ACS_BIN_VALUE,
    MSGHANDLE_ACS_EXT_VALUE
} msghandle_unpack_state;


typedef enum {
    MSGHANDLE_CT_ARRAY_ITEM,
    MSGHANDLE_CT_MAP_KEY,
    MSGHANDLE_CT_MAP_VALUE
} msghandle_container_type;


#endif /* msghandle/unpack_define.hpp */
