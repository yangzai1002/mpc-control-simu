//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014-2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_CPP11_UNORDERED_SET_HPP
#define MSGHANDLE_V1_TYPE_CPP11_UNORDERED_SET_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <unordered_set>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct as<std::unordered_set<Key, Hash, Compare, Alloc>, typename std::enable_if<msghandle::has_as<Key>::value>::type> {
    std::unordered_set<Key, Hash, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::unordered_set<Key, Hash, Compare, Alloc> v;
        while (p > pbegin) {
            --p;
            v.insert(p->as<Key>());
        }
        return v;
    }
};

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct convert<std::unordered_set<Key, Hash, Compare, Alloc>> {
    msghandle::object const& operator()(msghandle::object const& o, std::unordered_set<Key, Hash, Compare, Alloc>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::unordered_set<Key, Hash, Compare, Alloc> tmp;
        while(p > pbegin) {
            --p;
            tmp.insert(p->as<Key>());
        }
        v = std::move(tmp);
        return o;
    }
};

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct pack<std::unordered_set<Key, Hash, Compare, Alloc>> {
    template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::unordered_set<Key, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(typename std::unordered_set<Key, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<std::unordered_set<Key, Hash, Compare, Alloc>> {
    void operator()(msghandle::object::with_zone& o, const std::unordered_set<Key, Hash, Compare, Alloc>& v) const {
        o.type = msghandle::type::ARRAY;
        if(v.empty()) {
            o.via.array.ptr = MSGHANDLE_NULLPTR;
            o.via.array.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            msghandle::object* const pend = p + size;
            o.via.array.ptr = p;
            o.via.array.size = size;
            typename std::unordered_set<Key, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(*it, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};


template <typename Key, typename Hash, typename Compare, typename Alloc>
struct as<std::unordered_multiset<Key, Hash, Compare, Alloc>, typename std::enable_if<msghandle::has_as<Key>::value>::type> {
    std::unordered_multiset<Key, Hash, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::unordered_multiset<Key, Hash, Compare, Alloc> v;
        while (p > pbegin) {
            --p;
            v.insert(p->as<Key>());
        }
        return v;
    }
};

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct convert<std::unordered_multiset<Key, Hash, Compare, Alloc>> {
    msghandle::object const& operator()(msghandle::object const& o, std::unordered_multiset<Key, Hash, Compare, Alloc>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::unordered_multiset<Key, Hash, Compare, Alloc> tmp;
        while(p > pbegin) {
            --p;
            tmp.insert(p->as<Key>());
        }
        v = std::move(tmp);
        return o;
    }
};

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct pack<std::unordered_multiset<Key, Hash, Compare, Alloc>> {
    template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::unordered_multiset<Key, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(typename std::unordered_multiset<Key, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};

template <typename Key, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<std::unordered_multiset<Key, Hash, Compare, Alloc>> {
    void operator()(msghandle::object::with_zone& o, const std::unordered_multiset<Key, Hash, Compare, Alloc>& v) const {
        o.type = msghandle::type::ARRAY;
        if(v.empty()) {
            o.via.array.ptr = MSGHANDLE_NULLPTR;
            o.via.array.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            msghandle::object* const pend = p + size;
            o.via.array.ptr = p;
            o.via.array.size = size;
            typename std::unordered_multiset<Key, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(*it, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CPP11_UNORDERED_SET_HPP
