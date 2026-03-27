//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_TYPE_TR1_UNORDERED_MAP_HPP
#define MSGHANDLE_TYPE_TR1_UNORDERED_MAP_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#if defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#define MSGHANDLE_HAS_STD_UNORDERED_MAP
#include <unordered_map>
#define MSGHANDLE_STD_TR1 std

#else   // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if __GNUC__ >= 4

#define MSGHANDLE_HAS_STD_TR1_UNORDERED_MAP

#include <tr1/unordered_map>
#define MSGHANDLE_STD_TR1 std::tr1

#endif // __GNUC__ >= 4

#endif  // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if defined(MSGHANDLE_STD_TR1)

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename K, typename V, typename Hash, typename Pred, typename Alloc>
struct convert<MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc>& v) const {
        if(o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc> tmp;
        for(; p != pend; ++p) {
            K key;
            p->key.convert(key);
            p->val.convert(tmp[key]);
        }
        tmp.swap(v);
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Pred, typename Alloc>
struct pack<MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for(typename MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Pred, typename Alloc>
struct object_with_zone<MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc>& v) const {
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
            typename MSGHANDLE_STD_TR1::unordered_map<K, V, Hash, Pred, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msghandle::object(it->first, o.zone);
                p->val = msghandle::object(it->second, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

template <typename K, typename V, typename Hash, typename Pred, typename Alloc>
struct convert<MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc>& v) const {
        if(o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc> tmp;
        for(; p != pend; ++p) {
            std::pair<K, V> value;
            p->key.convert(value.first);
            p->val.convert(value.second);
            tmp.insert(value);
        }
        tmp.swap(v);
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Pred, typename Alloc>
struct pack<MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for(typename MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Hash, typename Pred, typename Alloc>
struct object_with_zone<MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc>& v) const {
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
            typename MSGHANDLE_STD_TR1::unordered_multimap<K, V, Hash, Pred, Alloc>::const_iterator it(v.begin());
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

#undef MSGHANDLE_STD_TR1

#endif // MSGHANDLE_STD_TR1

#endif // MSGHANDLE_TYPE_TR1_UNORDERED_MAP_HPP
