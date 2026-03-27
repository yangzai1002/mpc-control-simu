//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_ADAPTOR_BASE_DECL_HPP
#define MSGHANDLE_V1_ADAPTOR_BASE_DECL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/object_fwd.hpp"
#include "msghandle/pack.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

template <typename Stream>
class packer;

namespace adaptor {

// Adaptor functors

template <typename T, typename Enabler = void>
struct convert;

template <typename T, typename Enabler = void>
struct pack;

template <typename T, typename Enabler = void>
struct print;

template <typename T, typename Enabler = void>
struct checkcode;

template <typename T, typename Enabler = void>
struct reflectget;

template <typename T, typename Enabler = void>
struct object;

template <typename T, typename Enabler = void>
struct object_with_zone;

} // namespace adaptor

// operators

template <typename T>
typename msghandle::enable_if<
    !is_array<T>::value,
    msghandle::object const&
>::type
operator>> (msghandle::object const& o, T& v);
template <typename T, std::size_t N>
msghandle::object const& operator>> (msghandle::object const& o, T(&v)[N]);

template <typename Stream, typename T>
typename msghandle::enable_if<
    !is_array<T>::value,
    msghandle::packer<Stream>&
    >::type
operator<< (msghandle::packer<Stream>& o, T const& v);
//template <typename Stream, typename T>
//typename msghandle::enable_if<
//    !is_array<T>::value,
//    msghandle::packer<Stream>&
//    >::type
//operator<< (msghandle::packer<Stream>& o, T const& v,const std::string& k);
template <typename Stream, typename T, std::size_t N>
msghandle::packer<Stream>& operator<< (msghandle::packer<Stream>& o, const T(&v)[N]);

template <typename Stream, typename T>
msghandle::packer<Stream>& packfunc(msghandle::packer<Stream>& o, T const& v,const std::string& k,SMsgHandleInfo& msghandle_info);

template <typename Stream, typename T>
msghandle::packer<Stream>& operatorfunc(msghandle::packer<Stream>& o, T const& v,const std::string& k);

template <typename Stream, typename T>
msghandle::packer<Stream>& operatorcheckcode(msghandle::packer<Stream>& o, T const& v,const std::string& k);

template <typename Stream, typename T>
msghandle::packer<Stream>& operatorreflectget(msghandle::packer<Stream>& o, T const& v,const std::string& k, const std::string filter);

template <typename T>
typename msghandle::enable_if<
    !is_array<T>::value
>::type
operator<< (msghandle::object& o, T const& v);
template <typename T, std::size_t N>
void operator<< (msghandle::object& o, const T(&v)[N]);

template <typename T>
typename msghandle::enable_if<
    !is_array<T>::value
>::type
operator<< (msghandle::object::with_zone& o, T const& v);
template <typename T, std::size_t N>
void operator<< (msghandle::object::with_zone& o, const T(&v)[N]);

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_ADAPTOR_BASE_DECL_HPP
