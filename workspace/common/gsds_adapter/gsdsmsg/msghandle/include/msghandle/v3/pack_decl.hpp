//
// MessagePack for C++ serializing routine
//
// Copyright (C) 2008-2018 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_PACK_DECL_HPP
#define MSGHANDLE_V3_PACK_DECL_HPP

#include "msghandle/v2/pack_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond

using v2::packer;

using v2::pack;

#if MSGHANDLE_ENDIAN_LITTLE_BYTE

using v2::take8_8;

using v2::take8_16;

using v2::take8_32;

using v2::take8_64;

#elif MSGHANDLE_ENDIAN_BIG_BYTE

using v2::take8_8;

using v2::take8_16;

using v2::take8_32;

using v2::take8_64;

#else
#error msghandle-c supports only big endian and little endian
#endif

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V3_PACK_DECL_HPP
