//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V3_UNPACK_HPP
#define MSGHANDLE_V3_UNPACK_HPP

#include "msghandle/unpack_decl.hpp"
#include "msghandle/parse.hpp"
#include "msghandle/create_object_visitor.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v3) {
/// @endcond

inline msghandle::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit
)
{
    msghandle::object obj;
    msghandle::unique_ptr<msghandle::zone> z(new msghandle::zone);
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        return msghandle::object_handle(obj, msghandle::move(z));
    case PARSE_EXTRA_BYTES:
        return msghandle::object_handle(obj, msghandle::move(z));
    default:
        break;
    }
    return msghandle::object_handle();
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    bool referenced;
    return msghandle::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    std::size_t off = 0;
    return msghandle::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return msghandle::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    msghandle::object obj;
    msghandle::unique_ptr<msghandle::zone> z(new msghandle::zone);
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        result.set(obj);
        result.zone() = msghandle::move(z);
        return;
    case PARSE_EXTRA_BYTES:
        result.set(obj);
        result.zone() = msghandle::move(z);
        return;
    default:
        return;
    }
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    msghandle::v3::unpack_reference_func f, void* user_data,
            msghandle::unpack_limit const& limit)
{
    bool referenced;
    msghandle::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    std::size_t off = 0;
    msghandle::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    msghandle::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}


inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    msghandle::object obj;
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        return obj;
    case PARSE_EXTRA_BYTES:
        return obj;
    default:
        break;
    }
    return obj;
}

inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    bool referenced;
    return msghandle::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, bool& referenced,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    std::size_t off = 0;
    return msghandle::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len,
    msghandle::unpack_reference_func f, void* user_data,
    msghandle::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return msghandle::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msghandle


#endif // MSGHANDLE_V3_UNPACK_HPP
