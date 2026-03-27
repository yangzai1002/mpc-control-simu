//
// MessagePack for C++ memory pool
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_CPP11_ZONE_DECL_HPP
#define MSGHANDLE_V1_CPP11_ZONE_DECL_HPP

#include "msghandle/versioning.hpp"

#include <cstdlib>
#include <memory>
#include <vector>

#include "msghandle/cpp_config.hpp"

#ifndef MSGHANDLE_ZONE_CHUNK_SIZE
#define MSGHANDLE_ZONE_CHUNK_SIZE 8192
#endif

#ifndef MSGHANDLE_ZONE_ALIGN
#define MSGHANDLE_ZONE_ALIGN sizeof(void*)
#endif

#if defined(_MSC_VER)
#define MSGHANDLE_ZONE_ALIGNOF(type) __alignof(type)
#else
#define MSGHANDLE_ZONE_ALIGNOF(type) __alignof__(type)
#endif
// For a compiler that doesn't support __alignof__:
// #define MSGHANDLE_ZONE_ALIGNOF(type) MSGHANDLE_ZONE_ALIGN

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

class zone;

std::size_t aligned_size(
    std::size_t size,
    std::size_t align = MSGHANDLE_ZONE_ALIGN);

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_CPP11_ZONE_DECL_HPP
