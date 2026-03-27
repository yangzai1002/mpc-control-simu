//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2017 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_X3_UNPACK_DECL_HPP
#define MSGHANDLE_V3_X3_UNPACK_DECL_HPP

#if defined(MSGHANDLE_USE_X3_PARSE)

#include "msghandle/v2/x3_unpack_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond

namespace detail {

using v2::detail::unpack_imp;

} // detail

template <typename Iterator>
msghandle::object_handle unpack(
    Iterator&& begin, Iterator&& end,
    bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR,
    unpack_limit const& limit = unpack_limit());

template <typename Iterator>
msghandle::object_handle unpack(
    Iterator&& begin, Iterator&& end,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR,
    unpack_limit const& limit = unpack_limit());

template <typename Iterator>
msghandle::object unpack(
    msghandle::zone& z,
    Iterator&& begin, Iterator&& end,
    bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR,
    unpack_limit const& limit = unpack_limit());

template <typename Iterator>
msghandle::object unpack(
    msghandle::zone& z,
    Iterator&& begin, Iterator&& end,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR,
    unpack_limit const& limit = unpack_limit());

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msghandle

#endif // defined(MSGHANDLE_USE_X3_PARSE)

#endif // MSGHANDLE_V3_X3_UNPACK_DECL_HPP
