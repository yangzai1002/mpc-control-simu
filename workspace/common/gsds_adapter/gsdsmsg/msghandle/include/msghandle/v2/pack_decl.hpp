//
// MessagePack for C++ serializing routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_PACK_DECL_HPP
#define MSGHANDLE_V2_PACK_DECL_HPP

#include "msghandle/v1/pack_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

using v1::packer;

using v1::pack;


#if MSGHANDLE_ENDIAN_LITTLE_BYTE

using v1::take8_8;

using v1::take8_16;

using v1::take8_32;

using v1::take8_64;

#elif MSGHANDLE_ENDIAN_BIG_BYTE

using v1::take8_8;

using v1::take8_16;

using v1::take8_32;

using v1::take8_64;

#else
#error msghandle-c supports only big endian and little endian
#endif

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V2_PACK_DECL_HPP
