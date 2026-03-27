//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_ADAPTOR_BASE_HPP
#define MSGHANDLE_V3_ADAPTOR_BASE_HPP

#include "msghandle/v3/adaptor/adaptor_base_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond


namespace adaptor {

// Adaptor functors

template <typename T, typename Enabler>
struct convert : v2::adaptor::convert<T, Enabler> {
};

template <typename T, typename Enabler>
struct pack : v2::adaptor::pack<T, Enabler> {
};

template <typename T, typename Enabler>
struct print : v2::adaptor::print<T, Enabler> {
};

template <typename T, typename Enabler>
struct object
    : v2::adaptor::object<
        T,
        typename msghandle::enable_if<
            !msghandle::is_same<T, std::string>::value &&
            !msghandle::is_array<T>::value,
            Enabler
        >::type
      > {
};

template <typename T, typename Enabler>
struct object_with_zone : v2::adaptor::object_with_zone<T, Enabler> {
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

} // namespace msghandle


#endif // MSGHANDLE_V3_ADAPTOR_BASE_HPP
