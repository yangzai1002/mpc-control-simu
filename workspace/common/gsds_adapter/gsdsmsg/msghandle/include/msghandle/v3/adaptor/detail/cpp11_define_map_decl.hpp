//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_CPP11_DEFINE_MAP_DECL_HPP
#define MSGHANDLE_V3_CPP11_DEFINE_MAP_DECL_HPP

#include "msghandle/v2/adaptor/detail/cpp11_define_map_decl.hpp"

namespace msghandle {
/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond
namespace type {

using v2::type::define_map_imp;
using v2::type::define_map;
using v2::type::make_define_map;

}  // namespace type
/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond
}  // namespace msghandle

#endif // MSGHANDLE_V3_CPP11_DEFINE_MAP_DECL_HPP
