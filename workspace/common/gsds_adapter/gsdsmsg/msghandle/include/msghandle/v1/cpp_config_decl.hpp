//
// MessagePack for C++ C++03/C++11 Adaptation
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_CPP_CONFIG_DECL_HPP
#define MSGHANDLE_V1_CPP_CONFIG_DECL_HPP

#include "msghandle/cpp_version.hpp"
#include "msghandle/versioning.hpp"

#if defined(MSGHANDLE_USE_CPP03)

#if defined(nullptr)
#  if defined (__cplusplus_cli)
#    define MSGHANDLE_NULLPTR __nullptr
#  else  // defined (__cplusplus_cli)
#    define MSGHANDLE_NULLPTR nullptr
#  endif // defined (__cplusplus_cli)
#else  // defined(nullptr)
#  define MSGHANDLE_NULLPTR (0)
#endif // defined(nullptr)

#include <memory>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

template <typename T>
struct unique_ptr;

template <typename T>
T& move(T& t);

template <typename T>
T const& move(T const& t);

template <bool P, typename T = void>
struct enable_if;

template<typename T, T val>
struct integral_constant;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

template<class T, class U>
struct is_same;

template<typename T>
struct underlying_type;

template<class T>
struct is_array;

template<class T>
struct remove_const;
template<class T>
struct remove_volatile;
template<class T>
struct remove_cv;

template<class T>
struct is_pointer;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle


#else  // MSGHANDLE_USE_CPP03

#if defined (__cplusplus_cli)
#  define MSGHANDLE_NULLPTR __nullptr
#else  // defined (__cplusplus_cli)
#  define MSGHANDLE_NULLPTR nullptr
#endif // defined (__cplusplus_cli)

#include <memory>
#include <tuple>

namespace msghandle {
/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

    // unique_ptr
    using std::unique_ptr;
    // using std::make_unique; // since C++14
    using std::hash;

    // utility
    using std::move;
    using std::swap;
    using std::enable_if;
    using std::is_same;
    using std::underlying_type;
    using std::is_array;
    using std::remove_const;
    using std::remove_volatile;
    using std::remove_cv;
    using std::is_pointer;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond
}  // namespace msghandle


#endif // MSGHANDLE_USE_CPP03

#if defined(__has_include)
#define MSGHANDLE_HAS_INCLUDE __has_include
#else  // defined(__has_include)
#define MSGHANDLE_HAS_INCLUDE(header) 0
#endif // defined(__has_include)

#endif // MSGHANDLE_V1_CPP_CONFIG_DECL_HPP
