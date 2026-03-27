//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_BOOST_OPTIONAL_HPP
#define MSGHANDLE_V1_TYPE_BOOST_OPTIONAL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif // defined(__GNUC__)

// To suppress warning on Boost.1.58.0
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) || defined(__clang__)

#include <boost/optional.hpp>

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) || defined(__clang__)
#pragma GCC diagnostic pop
#endif // defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) || defined(__clang__)

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif // defined(__GNUC__)

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined (MSGHANDLE_USE_CPP03)

template <typename T>
struct as<boost::optional<T>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    boost::optional<T> operator()(msghandle::object const& o) const {
        if(o.is_nil()) return boost::none;
        return o.as<T>();
    }
};

#endif // !defined (MSGHANDLE_USE_CPP03)

template <typename T>
struct convert<boost::optional<T> > {
    msghandle::object const& operator()(msghandle::object const& o, boost::optional<T>& v) const {
        if(o.is_nil()) v = boost::none;
        else {
            T t;
            msghandle::adaptor::convert<T>()(o, t);
            v = t;
        }
        return o;
    }
};

template <typename T>
struct pack<boost::optional<T> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const boost::optional<T>& v) const {
        if (v) o.pack(*v);
        else o.pack_nil();
        return o;
    }
};

template <typename T>
struct object<boost::optional<T> > {
    void operator()(msghandle::object& o, const boost::optional<T>& v) const {
        if (v) msghandle::adaptor::object<T>()(o, *v);
        else o.type = msghandle::type::NIL;
    }
};

template <typename T>
struct object_with_zone<boost::optional<T> > {
    void operator()(msghandle::object::with_zone& o, const boost::optional<T>& v) const {
        if (v) msghandle::adaptor::object_with_zone<T>()(o, *v);
        else o.type = msghandle::type::NIL;
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_BOOST_OPTIONAL_HPP
