//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_OBJECT_DECL_HPP
#define MSGHANDLE_V2_OBJECT_DECL_HPP

#include "msghandle/v1/object_decl.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

using v1::object_handle;

namespace detail {

using v1::detail::add_ext_type_size;

} // namespace detail

using v1::aligned_zone_size;

using v1::clone;

namespace detail {

using v1::detail::packer_serializer;
using v1::detail::packer_print;
using v1::detail::packer_checkcode;
using v1::detail::packer_reflectget;
} // namespace detail

using v1::object_parser;
using v1::object_pack_visitor;
using v1::object_stringize_visitor;

using v1::operator==;
using v1::operator!=;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V2_OBJECT_DECL_HPP
