//
// MessagePack for C++ deflate buffer implementation
//
// Copyright (C) 2010-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_ZBUFFER_DECL_HPP
#define MSGHANDLE_V1_ZBUFFER_DECL_HPP

#include "msghandle/versioning.hpp"

#ifndef MSGHANDLE_ZBUFFER_RESERVE_SIZE
#define MSGHANDLE_ZBUFFER_RESERVE_SIZE 512
#endif

#ifndef MSGHANDLE_ZBUFFER_INIT_SIZE
#define MSGHANDLE_ZBUFFER_INIT_SIZE 8192
#endif

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

class zbuffer;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_ZBUFFER_DECL_HPP
