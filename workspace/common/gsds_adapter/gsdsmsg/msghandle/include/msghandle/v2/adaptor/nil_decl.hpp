//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_TYPE_NIL_DECL_HPP
#define MSGHANDLE_V2_TYPE_NIL_DECL_HPP

#include "msghandle/v1/adaptor/nil_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace type {

using v1::type::nil_t;

#if defined(MSGHANDLE_USE_LEGACY_NIL)

typedef nil_t nil;

#endif // defined(MSGHANDLE_USE_LEGACY_NIL)

using v1::type::operator<;
using v1::type::operator==;

}  // namespace type

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V2_TYPE_NIL_DECL_HPP
