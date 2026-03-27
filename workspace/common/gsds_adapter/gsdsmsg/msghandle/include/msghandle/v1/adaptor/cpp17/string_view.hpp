//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2017 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_STRING_VIEW_HPP
#define MSGHANDLE_V1_TYPE_STRING_VIEW_HPP

#include "msghandle/cpp_version.hpp"

#if MSGHANDLE_CPP_VERSION >= 201703

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <string_view>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct convert<std::string_view> {
    msghandle::object const& operator()(msghandle::object const& o, std::string_view& v) const {
        switch (o.type) {
        case msghandle::type::BIN:
            v = std::string_view(o.via.bin.ptr, o.via.bin.size);
            break;
        case msghandle::type::STR:
            v = std::string_view(o.via.str.ptr, o.via.str.size);
            break;
        default:
            throw msghandle::type_error();
            break;
        }
        return o;
    }
};

template <>
struct pack<std::string_view> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::string_view& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.pack_str(size);
        o.pack_str_body(v.data(), size);
        return o;
    }
};

template <>
struct object<std::string_view> {
    void operator()(msghandle::object& o, const std::string_view& v) const {
        uint32_t size = checked_get_container_size(v.size());
        o.type = msghandle::type::STR;
        o.via.str.ptr = v.data();
        o.via.str.size = size;
    }
};

template <>
struct object_with_zone<std::string_view> {
    void operator()(msghandle::object::with_zone& o, const std::string_view& v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};


} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_CPP_VERSION >= 201703

#endif // MSGHANDLE_V1_TYPE_STRING_VIEW_HPP
