//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_X3_UNPACK_DECL_HPP
#define MSGHANDLE_V2_X3_UNPACK_DECL_HPP

#if defined(MSGHANDLE_USE_X3_PARSE)

#include "msghandle/versioning.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace detail {

template <typename Iterator>
void
unpack_imp(Iterator&& begin, Iterator&& end,
           msghandle::zone& result_zone, msghandle::object& result, bool& referenced,
           unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR,
           unpack_limit const& limit = unpack_limit());

} // namespace detail

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
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle


#endif // defined(MSGHANDLE_USE_X3_PARSE)

#endif // MSGHANDLE_V2_X3_UNPACK_DECL_HPP
