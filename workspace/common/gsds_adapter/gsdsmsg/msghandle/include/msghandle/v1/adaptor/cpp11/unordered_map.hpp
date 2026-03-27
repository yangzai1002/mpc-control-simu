//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014-2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_CPP11_UNORDERED_MAP_HPP
#define MSGHANDLE_V1_TYPE_CPP11_UNORDERED_MAP_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <unordered_map>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct as<
    std::unordered_map<K, V, Hash, Compare, Alloc>,
    typename std::enable_if<msghandle::has_as<K>::value || msghandle::has_as<V>::value>::type> {
    std::unordered_map<K, V, Hash, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::unordered_map<K, V, Hash, Compare, Alloc> v;
        for (; p != pend; ++p) {
            v.emplace(p->key.as<K>(), p->val.as<V>());
        }
        return v;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct convert<std::unordered_map<K, V, Hash, Compare, Alloc>> {
    msghandle::object const& operator()(msghandle::object const& o, std::unordered_map<K, V, Hash, Compare, Alloc>& v) const {
        if(o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::unordered_map<K, V, Hash, Compare, Alloc> tmp;
        for(; p != pend; ++p) {
            K key;
            p->key.convert(key);
            p->val.convert(tmp[std::move(key)]);
        }
        v = std::move(tmp);
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct pack<std::unordered_map<K, V, Hash, Compare, Alloc>> {
    template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::unordered_map<K, V, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for(typename std::unordered_map<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<std::unordered_map<K, V, Hash, Compare, Alloc>> {
    void operator()(msghandle::object::with_zone& o, const std::unordered_map<K, V, Hash, Compare, Alloc>& v) const {
        o.type = msghandle::type::MAP;
        if(v.empty()) {
            o.via.map.ptr  = MSGHANDLE_NULLPTR;
            o.via.map.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object_kv* p = static_cast<msghandle::object_kv*>(o.zone.allocate_align(sizeof(msghandle::object_kv)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv)));
            msghandle::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename std::unordered_map<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msghandle::object(it->first, o.zone);
                p->val = msghandle::object(it->second, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};


template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct as<
    std::unordered_multimap<K, V, Hash, Compare, Alloc>,
    typename std::enable_if<msghandle::has_as<K>::value || msghandle::has_as<V>::value>::type> {
    std::unordered_multimap<K, V, Hash, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::unordered_multimap<K, V, Hash, Compare, Alloc> v;
        for (; p != pend; ++p) {
            v.emplace(p->key.as<K>(), p->val.as<V>());
        }
        return v;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct convert<std::unordered_multimap<K, V, Hash, Compare, Alloc>> {
    msghandle::object const& operator()(msghandle::object const& o, std::unordered_multimap<K, V, Hash, Compare, Alloc>& v) const {
        if(o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::unordered_multimap<K, V, Hash, Compare, Alloc> tmp;
        for(; p != pend; ++p) {
            std::pair<K, V> value;
            p->key.convert(value.first);
            p->val.convert(value.second);
            tmp.insert(std::move(value));
        }
        v = std::move(tmp);
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct pack<std::unordered_multimap<K, V, Hash, Compare, Alloc>> {
    template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::unordered_multimap<K, V, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for(typename std::unordered_multimap<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<std::unordered_multimap<K, V, Hash, Compare, Alloc>> {
    void operator()(msghandle::object::with_zone& o, const std::unordered_multimap<K, V, Hash, Compare, Alloc>& v) const {
        o.type = msghandle::type::MAP;
        if(v.empty()) {
            o.via.map.ptr  = MSGHANDLE_NULLPTR;
            o.via.map.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object_kv* p = static_cast<msghandle::object_kv*>(o.zone.allocate_align(sizeof(msghandle::object_kv)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv)));
            msghandle::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename std::unordered_multimap<K, V, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msghandle::object(it->first, o.zone);
                p->val = msghandle::object(it->second, o.zone);
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


#endif // MSGHANDLE_V1_TYPE_CPP11_UNORDERED_MAP_HPP
