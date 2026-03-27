//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_VECTOR_BOOL_HPP
#define MSGHANDLE_V1_TYPE_VECTOR_BOOL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/object_fwd.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"

#include <vector>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename Alloc>
struct convert<std::vector<bool, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, std::vector<bool, Alloc>& v) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        if (o.via.array.size > 0) {
            v.resize(o.via.array.size);
            msghandle::object* p = o.via.array.ptr;
            for (typename std::vector<bool, Alloc>::iterator it = v.begin(), end = v.end();
                 it != end;
                 ++it) {
                *it = p->as<bool>();
                ++p;
            }
        }
        return o;
    }
};

template <typename Alloc>
struct pack<std::vector<bool, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::vector<bool, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(typename std::vector<bool, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(static_cast<bool>(*it),msghandle_info);
        }
        return o;
    }
};

template <typename Alloc>
struct object_with_zone<std::vector<bool, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const std::vector<bool, Alloc>& v) const {
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
            typename std::vector<bool, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(static_cast<bool>(*it), o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_VECTOR_BOOL_HPP
