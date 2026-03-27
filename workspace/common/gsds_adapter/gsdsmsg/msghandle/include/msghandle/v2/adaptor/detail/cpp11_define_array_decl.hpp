//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_CPP11_DEFINE_ARRAY_DECL_HPP
#define MSGHANDLE_V2_CPP11_DEFINE_ARRAY_DECL_HPP

#include "msghandle/v1/adaptor/detail/cpp11_define_array_decl.hpp"

namespace msghandle {
/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond
namespace type {

using v1::type::define_array_imp;
using v1::type::define_array;

using v1::type::make_define_array;

}  // namespace type
/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond
}  // namespace msghandle

#endif // MSGHANDLE_V2_CPP11_DEFINE_ARRAY_DECL_HPP
