//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_TYPE_TR1_UNORDERED_SET_HPP
#define MSGHANDLE_TYPE_TR1_UNORDERED_SET_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#if defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#define MSGHANDLE_HAS_STD_UNORDERED_SET
#include <unordered_set>
#define MSGHANDLE_STD_TR1 std

#else   // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if __GNUC__ >= 4

#define MSGHANDLE_HAS_STD_TR1_UNORDERED_SET

#include <tr1/unordered_set>
#define MSGHANDLE_STD_TR1 std::tr1

#endif // __GNUC__ >= 4

#endif  // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if defined(MSGHANDLE_STD_TR1)

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename T, typename Hash, typename Compare, typename Alloc>
struct convert<MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc> tmp;
        while(p > pbegin) {
            --p;
            tmp.insert(p->as<T>());
        }
        tmp.swap(v);
        return o;
    }
};

template <typename T, typename Hash, typename Compare, typename Alloc>
struct pack<MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(typename MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};

template <typename T, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc>& v) const {
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
            typename MSGHANDLE_STD_TR1::unordered_set<T, Hash, Compare, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(*it, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};


template <typename T, typename Hash, typename Compare, typename Alloc>
struct convert<MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc> tmp;
        while(p > pbegin) {
            --p;
            tmp.insert(p->as<T>());
        }
        tmp.swap(v);
        return o;
    }
};

template <typename T, typename Hash, typename Compare, typename Alloc>
struct pack<MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc>& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(typename MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};

template <typename T, typename Hash, typename Compare, typename Alloc>
struct object_with_zone<MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc>& v) const {
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
            typename MSGHANDLE_STD_TR1::unordered_multiset<T, Hash, Compare, Alloc>::const_iterator it(v.begin());
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

#undef MSGHANDLE_STD_TR1

#endif // MSGHANDLE_STD_TR1

#endif // MSGHANDLE_TYPE_TR1_UNORDERED_SET_HPP
