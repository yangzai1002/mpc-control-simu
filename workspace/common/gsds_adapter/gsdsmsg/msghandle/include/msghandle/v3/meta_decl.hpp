//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V3_META_DECL_HPP
#define MSGHANDLE_V3_META_DECL_HPP

#if !defined(MSGHANDLE_USE_CPP03)

#include "msghandle/v2/meta_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond

namespace detail {

using v2::detail::bool_pack;

using v2::detail::all_of_imp;

using v2::detail::any_of_imp;

} // namespace detail

using v2::all_of;

using v2::any_of;

using v2::seq;

using v2::gen_seq;

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

} // namespace msghandle

#endif // !defined(MSGHANDLE_USE_CPP03)

#endif // MSGHANDLE_V3_META_DECL_HPP
