//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_OBJECT_DECL_HPP
#define MSGHANDLE_V3_OBJECT_DECL_HPP

#include "msghandle/v2/object_decl.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond

using v2::object_handle;

namespace detail {

using v2::detail::add_ext_type_size;

} // namespace detail

using v2::aligned_zone_size;

using v2::clone;

namespace detail {

using v2::detail::packer_serializer;
using v2::detail::packer_print;
using v2::detail::packer_checkcode;
using v2::detail::packer_reflectget;
} // namespace detail

using v2::object_parser;
using v2::object_pack_visitor;
using v2::object_stringize_visitor;

using v2::operator==;
using v2::operator!=;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V3_OBJECT_DECL_HPP
