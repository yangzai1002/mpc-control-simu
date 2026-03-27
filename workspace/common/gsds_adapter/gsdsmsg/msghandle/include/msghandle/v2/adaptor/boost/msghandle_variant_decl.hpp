//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_TYPE_BOOST_MSGHANDLE_VARIANT_DECL_HPP
#define MSGHANDLE_V2_TYPE_BOOST_MSGHANDLE_VARIANT_DECL_HPP

#include "msghandle/v1/adaptor/boost/msghandle_variant_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace type {

using v1::type::basic_variant;
using v1::type::variant;
using v1::type::variant_ref;

using v1::type::operator<;

using v1::type::operator==;

} // namespace type

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V2_TYPE_BOOST_MSGHANDLE_VARIANT_DECL_HPP
