//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_TYPE_CPP11_REFERENCE_WRAPPER_HPP
#define MSGHANDLE_V1_TYPE_CPP11_REFERENCE_WRAPPER_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <memory>
#include <type_traits>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename T>
struct convert<std::reference_wrapper<T>> {
    msghandle::object const& operator()(msghandle::object const& o, std::reference_wrapper<T>& v) const {
        msghandle::adaptor::convert<T>()(o, v.get());
        return o;
    }
};

template <typename T>
struct pack<std::reference_wrapper<T>> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::reference_wrapper<T>& v) const {
        o.pack(v.get());
        return o;
    }
};

template <typename T>
struct object<std::reference_wrapper<T> > {
    void operator()(msghandle::object& o, const std::reference_wrapper<T>& v) const {
        msghandle::adaptor::object<typename std::remove_const<T>::type>()(o, v.get());
    }
};

template <typename T>
struct object_with_zone<std::reference_wrapper<T>> {
    void operator()(msghandle::object::with_zone& o, const std::reference_wrapper<T>& v) const {
        msghandle::adaptor::object_with_zone<typename std::remove_const<T>::type>()(o, v.get());
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CPP11_REFERENCE_WRAPPER_HPP
