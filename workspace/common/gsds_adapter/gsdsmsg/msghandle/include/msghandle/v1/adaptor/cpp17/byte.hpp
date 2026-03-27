//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_BYTE_HPP
#define MSGHANDLE_V1_TYPE_BYTE_HPP

#include "msghandle/cpp_version.hpp"

#if MSGHANDLE_CPP_VERSION >= 201703

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/adaptor/int_decl.hpp"
#include "msghandle/object.hpp"

#include <cstddef>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct convert<std::byte> {
    msghandle::object const& operator()(msghandle::object const& o, std::byte& v) const {
        v = static_cast<std::byte>(type::detail::convert_integer<unsigned char>(o));
        return o;
    }
};

template <>
struct pack<std::byte> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, std::byte v) const {
        o.pack_unsigned_char(static_cast<unsigned char>(v));
        return o;
    }
};

template <>
struct object<std::byte> {
    void operator()(msghandle::object& o, std::byte v) const {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = static_cast<unsigned char>(v);
    }
};

template <>
struct object_with_zone<std::byte> {
    void operator()(msghandle::object::with_zone& o, const std::byte& v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};


} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_CPP_VERSION >= 201703

#endif // MSGHANDLE_V1_TYPE_BYTE_HPP
