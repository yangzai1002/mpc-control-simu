//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_CREATE_OBJECT_VISITOR_DECL_HPP
#define MSGHANDLE_V3_CREATE_OBJECT_VISITOR_DECL_HPP

#include "msghandle/v2/create_object_visitor_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond

namespace detail {

using v2::detail::create_object_visitor;

} // detail

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V3_CREATE_OBJECT_VISITOR_DECL_HPP
