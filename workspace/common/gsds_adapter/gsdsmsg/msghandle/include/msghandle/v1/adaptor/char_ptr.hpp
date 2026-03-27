//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014-2015 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_CHAR_PTR_HPP
#define MSGHANDLE_V1_TYPE_CHAR_PTR_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/object_fwd.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <cstring>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct pack<const char*> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char* v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.pack_str(size);
        o.pack_str_body(v, size);
        return o;
    }
};

template <>
struct object_with_zone<const char*> {
    void operator()(msghandle::object::with_zone& o, const char* v) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.type = msghandle::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.str.ptr = ptr;
        o.via.str.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <>
struct object<const char*> {
    void operator()(msghandle::object& o, const char* v) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.type = msghandle::type::STR;
        o.via.str.ptr = v;
        o.via.str.size = size;
    }
};


template <>
struct pack<char*> {
    template <typename Stream>
    packer<Stream>& operator()(packer<Stream>& o, char* v,SMsgHandleInfo& msghandle_info) const {
        return o << static_cast<const char*>(v);
    }
};

template <>
struct object_with_zone<char*> {
    void operator()(msghandle::object::with_zone& o, char* v) const {
        o << static_cast<const char*>(v);
    }
};

template <>
struct object<char*> {
    void operator()(msghandle::object& o, char* v) const {
        o << static_cast<const char*>(v);
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CHAR_PTR_HPP
