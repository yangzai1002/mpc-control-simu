//
// MessagePack for C++ serializing routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_PACK_DECL_HPP
#define MSGHANDLE_V1_PACK_DECL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/cpp_config.hpp"
#include "msghandle/sysdep.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

/// The class template that supports continuous packing.
/**
 * @tparam Stream  Any type that have a member function `Stream write(const char*, size_t s)`
 *
 */
template <typename Stream>
class packer;

/// Pack the value as MessagePack format into the stream
/**
 * This function template is left for compatibility.
 * Use `void pack(Stream& s, const T& v)` instead of the function template.
 *
 * @tparam Stream Any type that have a member function `Stream write(const char*, size_t s)`
 * @tparam T Any type that is adapted to MessagePack
 * @param s The pointer to packing destination stream
 * @param v Packing value
 */
template <typename Stream, typename T>
void pack(Stream* s, const T& v);

/// Pack the value as MessagePack format into the stream
/**
 * @tparam Stream Any type that have a member function `Stream write(const char*, size_t s)`
 * @tparam T Any type that is adapted to MessagePack
 * @param s Packing destination stream
 * @param v Packing value
 */
template <typename Stream, typename T>
void pack(Stream& s, const T& v,SMsgHandleInfo& msghandle_info);


template <typename Stream, typename T>
void print(Stream* s, const T& v);
template <typename Stream, typename T>
void print(Stream& s, const T& v);

template <typename Stream, typename T>
void checkcode(Stream& s, const T& v);

template <typename T>
void checkcode(std::string& s, const T& v);

#if MSGHANDLE_ENDIAN_LITTLE_BYTE
template <typename T>
char take8_8(T d);

template <typename T>
char take8_16(T d);

template <typename T>
char take8_32(T d);

template <typename T>
char take8_64(T d);

#elif MSGHANDLE_ENDIAN_BIG_BYTE

template <typename T>
char take8_8(T d);

template <typename T>
char take8_16(T d);

template <typename T>
char take8_32(T d);

template <typename T>
char take8_64(T d);

#else
#error msghandle-c supports only big endian and little endian
#endif

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_PACK_DECL_HPP
