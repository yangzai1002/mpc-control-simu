//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_SIZE_EQUAL_ONLY_DECL_HPP
#define MSGHANDLE_V1_TYPE_SIZE_EQUAL_ONLY_DECL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/msghandle_tuple.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

template <typename T>
struct size_equal_only;

template <typename T>
size_equal_only<T> make_size_equal_only(T& t);

template <typename T>
std::size_t size(T const& t);

template <typename T, std::size_t N>
std::size_t size(const T(&)[N]);

#if !defined(MSGHANDLE_USE_CPP03)

template <typename... T>
std::size_t size(std::tuple<T...> const&);

#endif // !defined(MSGHANDLE_USE_CPP03)

} // namespace type

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_SIZE_EQUAL_ONLY_DECL_HPP
