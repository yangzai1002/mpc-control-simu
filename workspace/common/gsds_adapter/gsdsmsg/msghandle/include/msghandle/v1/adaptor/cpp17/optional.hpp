//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2017 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_OPTIONAL_HPP
#define MSGHANDLE_V1_TYPE_OPTIONAL_HPP

#include "msghandle/cpp_version.hpp"

#if MSGHANDLE_CPP_VERSION >= 201703

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <optional>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined (MSGHANDLE_USE_CPP03)

template <typename T>
struct as<std::optional<T>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::optional<T> operator()(msghandle::object const& o) const {
        if(o.is_nil()) return std::nullopt;
        return o.as<T>();
    }
};

#endif // !defined (MSGHANDLE_USE_CPP03)

template <typename T>
struct convert<std::optional<T> > {
    msghandle::object const& operator()(msghandle::object const& o, std::optional<T>& v) const {
        if(o.is_nil()) v = std::nullopt;
        else {
            T t;
            msghandle::adaptor::convert<T>()(o, t);
            v = t;
        }
        return o;
    }
};

template <typename T>
struct pack<std::optional<T> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::optional<T>& v) const {
        if (v) o.pack(*v);
        else o.pack_nil();
        return o;
    }
};

template <typename T>
struct object<std::optional<T> > {
    void operator()(msghandle::object& o, const std::optional<T>& v) const {
        if (v) msghandle::adaptor::object<T>()(o, *v);
        else o.type = msghandle::type::NIL;
    }
};

template <typename T>
struct object_with_zone<std::optional<T> > {
    void operator()(msghandle::object::with_zone& o, const std::optional<T>& v) const {
        if (v) msghandle::adaptor::object_with_zone<T>()(o, *v);
        else o.type = msghandle::type::NIL;
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_CPP_VERSION >= 201703

#endif // MSGHANDLE_V1_TYPE_OPTIONAL_HPP
