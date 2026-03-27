//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2014 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_OBJECT_DECL_HPP
#define MSGHANDLE_V1_OBJECT_DECL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/pack.hpp"
#include "msghandle/zone.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"

#include <cstring>
#include <stdexcept>
#include <typeinfo>
#include <limits>
#include <ostream>
#include <typeinfo>
#include <iomanip>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

/// The class holds object and zone
class object_handle;

namespace detail {

template <std::size_t N>
std::size_t add_ext_type_size(std::size_t size);

template <>
std::size_t add_ext_type_size<4>(std::size_t size);

} // namespace detail

std::size_t aligned_zone_size(msghandle::object const& obj);

/// clone object
/**
 * Clone (deep copy) object.
 * The copied object is located on newly allocated zone.
 * @param obj copy source object
 *
 * @return object_handle that holds deep copied object and zone.
 */
object_handle clone(msghandle::object const& obj);

namespace detail {

template <typename Stream, typename T>
struct packer_serializer;

template <typename Stream, typename T>
struct packer_print;

template <typename Stream, typename T>
struct packer_checkcode;

template <typename Stream, typename T>
struct packer_reflectget;
} // namespace detail

// obsolete
template <typename Type>
class define;

bool operator==(const msghandle::object& x, const msghandle::object& y);

template <typename T>
bool operator==(const msghandle::object& x, const T& y);

bool operator!=(const msghandle::object& x, const msghandle::object& y);

template <typename T>
bool operator==(const T& y, const msghandle::object& x);

template <typename T>
bool operator!=(const msghandle::object& x, const T& y);

template <typename T>
bool operator!=(const T& y, const msghandle::object& x);

class object_parser;

template <typename Stream>
struct object_pack_visitor;

struct object_stringize_visitor;

// obsolete
template <typename T>
MSGHANDLE_DEPRECATED("please use member function version of object::convert(T&)")
void convert(T& v, msghandle::object const& o);

// obsolete
template <typename Stream, typename T>
MSGHANDLE_DEPRECATED("please use member function version of packer::pack(const T&)")
void pack(msghandle::packer<Stream>& o, const T& v,SMsgHandleInfo& msghandle_info);

// obsolete
template <typename Stream, typename T>
MSGHANDLE_DEPRECATED("please use member function version of packer::print(const T&)")
void print(msghandle::packer<Stream>& o, const T& v,const std::string& k);

// obsolete
template <typename Stream, typename T>
MSGHANDLE_DEPRECATED("please use member function version of packer::checkcode(const T&)")
void checkcode(msghandle::packer<Stream>& o, const T& v,const std::string& k);

// obsolete
template <typename Stream, typename T>
MSGHANDLE_DEPRECATED("please use member function version of packer::pack(const T&)")
void pack_copy(msghandle::packer<Stream>& o, T v);

template <typename Stream>
msghandle::packer<Stream>& operator<< (msghandle::packer<Stream>& o, const msghandle::object& v);

template <typename Stream>
msghandle::packer<Stream>& operator<< (msghandle::packer<Stream>& o, const msghandle::object::with_zone& v);

std::ostream& operator<< (std::ostream& s, const msghandle::object& v);

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_OBJECT_DECL_HPP
