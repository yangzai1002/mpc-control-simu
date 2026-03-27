//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_RAW_HPP
#define MSGHANDLE_V1_TYPE_RAW_HPP

#include "msghandle/v1/adaptor/raw_decl.hpp"

#include <cstring>
#include <string>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

struct raw_ref {
    raw_ref() : size(0), ptr(MSGHANDLE_NULLPTR) {}
    raw_ref(const char* p, uint32_t s) : size(s), ptr(p) {}

    uint32_t size;
    const char* ptr;

    std::string str() const { return std::string(ptr, size); }

    bool operator== (const raw_ref& x) const
    {
        return size == x.size && (size == 0 || std::memcmp(ptr, x.ptr, size) == 0);
    }

    bool operator!= (const raw_ref& x) const
    {
        return !(*this == x);
    }

    bool operator< (const raw_ref& x) const
    {
        if(size == x.size) { return (size == 0 ? false : std::memcmp(ptr, x.ptr, size) < 0); }
        else { return size < x.size; }
    }

    bool operator> (const raw_ref& x) const
    {
        if(size == x.size) { return (size == 0 ? false : std::memcmp(ptr, x.ptr, size) > 0); }
        else { return size > x.size; }
    }
};

} // namespace type

namespace adaptor {

template <>
struct convert<type::raw_ref> {
    msghandle::object const& operator()(msghandle::object const& o, type::raw_ref& v) const {
        if(o.type != msghandle::type::BIN) { throw msghandle::type_error(); }
        v.ptr  = o.via.bin.ptr;
        v.size = o.via.bin.size;
        return o;
    }
};

template <>
struct pack<type::raw_ref> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const type::raw_ref& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        o.pack_bin(v.size);
        o.pack_bin_body(v.ptr, v.size);
        return o;
    }
};

template <>
struct object<type::raw_ref> {
    void operator()(msghandle::object& o, const type::raw_ref& v) const {
        o.type = msghandle::type::BIN;
        o.via.bin.ptr = v.ptr;
        o.via.bin.size = v.size;
    }
};

template <>
struct object_with_zone<type::raw_ref> {
    void operator()(msghandle::object::with_zone& o, const type::raw_ref& v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_RAW_HPP
