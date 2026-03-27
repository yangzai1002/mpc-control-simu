//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_TYPE_CPP11_SHARED_PTR_HPP
#define MSGHANDLE_V1_TYPE_CPP11_SHARED_PTR_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <memory>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename T>
struct as<std::shared_ptr<T>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::shared_ptr<T> operator()(msghandle::object const& o) const {
        if(o.is_nil()) return MSGHANDLE_NULLPTR;
        return std::make_shared<T>(o.as<T>());
    }
};

template <typename T>
struct convert<std::shared_ptr<T>> {
    msghandle::object const& operator()(msghandle::object const& o, std::shared_ptr<T>& v) const {
        if(o.is_nil()) v.reset();
        else {
            v = std::make_shared<T>();
            msghandle::adaptor::convert<T>()(o, *v);
        }
        return o;
    }
};

template <typename T>
struct pack<std::shared_ptr<T>> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::shared_ptr<T>& v) const {
        if (v) o.pack(*v);
        else o.pack_nil();
        return o;
    }
};

template <typename T>
struct object<std::shared_ptr<T> > {
    void operator()(msghandle::object& o, const std::shared_ptr<T>& v) const {
        if (v) msghandle::adaptor::object<T>()(o, *v);
        else o.type = msghandle::type::NIL;
    }
};

template <typename T>
struct object_with_zone<std::shared_ptr<T>> {
    void operator()(msghandle::object::with_zone& o, const std::shared_ptr<T>& v) const {
        if (v) msghandle::adaptor::object_with_zone<T>()(o, *v);
        else o.type = msghandle::type::NIL;
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CPP11_SHARED_PTR_HPP
