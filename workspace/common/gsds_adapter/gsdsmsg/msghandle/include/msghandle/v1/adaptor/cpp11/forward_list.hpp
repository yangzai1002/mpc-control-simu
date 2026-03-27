//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014 KONDO-2015 Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_TYPE_CPP11_FORWARD_LIST_HPP
#define MSGHANDLE_V1_TYPE_CPP11_FORWARD_LIST_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <forward_list>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename T, typename Alloc>
    struct as<std::forward_list<T, Alloc>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::forward_list<T, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        std::forward_list<T, Alloc> v;
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pend = o.via.array.ptr;
        while (p != pend) {
            --p;
            v.push_front(p->as<T>());
        }
        return v;
    }
};

template <typename T, typename Alloc>
struct convert<std::forward_list<T, Alloc>> {
    msghandle::object const& operator()(msghandle::object const& o, std::forward_list<T, Alloc>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        v.resize(o.via.array.size);
        msghandle::object* p = o.via.array.ptr;
        for (auto &e : v) {
            p->convert(e);
            ++p;
        }
        return o;
    }
};

template <typename T, typename Alloc>
struct pack<std::forward_list<T, Alloc>> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::forward_list<T, Alloc>& v) const {
        uint32_t size = checked_get_container_size(std::distance(v.begin(), v.end()));
        o.pack_array(size);
        for(auto const& e : v) o.pack(e);
        return o;
    }
};

template <typename T, typename Alloc>
struct object_with_zone<std::forward_list<T, Alloc>> {
    void operator()(msghandle::object::with_zone& o, const std::forward_list<T, Alloc>& v) const {
        o.type = msghandle::type::ARRAY;
        if(v.empty()) {
            o.via.array.ptr = MSGHANDLE_NULLPTR;
            o.via.array.size = 0;
        } else {
            uint32_t size = checked_get_container_size(std::distance(v.begin(), v.end()));
            o.via.array.size = size;
            msghandle::object* p = static_cast<msghandle::object*>(
                o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            o.via.array.ptr = p;
            for(auto const& e : v) *p++ = msghandle::object(e, o.zone);
        }
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CPP11_FORWARD_LIST_HPP
