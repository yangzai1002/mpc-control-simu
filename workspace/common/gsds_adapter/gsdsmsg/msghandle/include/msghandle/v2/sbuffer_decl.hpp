//
// MessagePack for C++ simple buffer implementation
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_SBUFFER_DECL_HPP
#define MSGHANDLE_V2_SBUFFER_DECL_HPP

#include "msghandle/versioning.hpp"

#ifndef MSGHANDLE_SBUFFER_INIT_SIZE
#define MSGHANDLE_SBUFFER_INIT_SIZE 8192
#endif

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

using v1::sbuffer;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V2_SBUFFER_DECL_HPP
