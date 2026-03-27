//
// MessagePack for C++ zero-copy buffer implementation
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_VREFBUFFER_DECL_HPP
#define MSGHANDLE_V1_VREFBUFFER_DECL_HPP

#include "msghandle/versioning.hpp"

#include <cstdlib>

#ifndef MSGHANDLE_VREFBUFFER_REF_SIZE
#define MSGHANDLE_VREFBUFFER_REF_SIZE 32
#endif

#ifndef MSGHANDLE_VREFBUFFER_CHUNK_SIZE
#define MSGHANDLE_VREFBUFFER_CHUNK_SIZE 8192
#endif

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

class vrefbuffer;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_VREFBUFFER_DECL_HPP
