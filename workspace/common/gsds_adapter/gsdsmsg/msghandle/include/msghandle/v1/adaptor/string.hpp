//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_STRING_HPP
#define MSGHANDLE_V1_TYPE_STRING_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <string>
#include <cstring>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct convert<std::string> {
    msghandle::object const& operator()(msghandle::object const& o, std::string& v) const {
        switch (o.type) {
        case msghandle::type::BIN:
            v.assign(o.via.bin.ptr, o.via.bin.size);
            break;
        case msghandle::type::STR:
            v.assign(o.via.str.ptr, o.via.str.size);
            break;
        default:
            throw msghandle::type_error();
            break;
        }
        return o;
    }
};

template <>
struct pack<std::string> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::string& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_str(size);
        o.pack_str_body(v.data(), size);
        return o;
    }
};

template <>
struct print<std::string> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::string& v,const std::string& k) const {
        o.stream() <<k<<"<std::string>"<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct checkcode<std::string> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::string& v,const std::string& k) const {
        o.stream() <<k<<"std::string"<<std::endl;
        return o;
    }
};

template <>
struct reflectget<std::string> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::string& v,const std::string& k, const std::string filter) const {
        o.stream() <<k<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct object<std::string> {
    void operator()(msghandle::object& o, const std::string& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.type = msghandle::type::STR;
        o.via.str.ptr = v.data();
        o.via.str.size = size;
    }
};

template <>
struct object_with_zone<std::string> {
    void operator()(msghandle::object::with_zone& o, const std::string& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.type = msghandle::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.str.ptr = ptr;
        o.via.str.size = size;
        std::memcpy(ptr, v.data(), v.size());
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_STRING_HPP
