//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_WSTRING_HPP
#define MSGHANDLE_V1_TYPE_WSTRING_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <vector>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

template <>
struct as<std::wstring> {
    std::wstring operator()(const msghandle::object& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        std::wstring v;
        v.reserve(o.via.array.size);
        if (o.via.array.size > 0) {
            msghandle::object* p = o.via.array.ptr;
            msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
            do {
                v.push_back(p->as<wchar_t>());
                ++p;
            } while (p < pend);
        }
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <>
struct convert<std::wstring> {
    msghandle::object const& operator()(msghandle::object const& o, std::wstring& v) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        v.resize(o.via.array.size);
        if (o.via.array.size > 0) {
            msghandle::object* p = o.via.array.ptr;
            msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
            std::wstring::iterator it = v.begin();
            do {
                p->convert(*it);
                ++p;
                ++it;
            } while(p < pend);
        }
        return o;
    }
};

template <>
struct pack<std::wstring> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::wstring& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for (std::wstring::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it,msghandle_info);
        }
        return o;
    }
};

template <>
struct object_with_zone<std::wstring> {
    void operator()(msghandle::object::with_zone& o, const std::wstring& v) const {
        o.type = msghandle::type::ARRAY;
        if (v.empty()) {
            o.via.array.ptr = MSGHANDLE_NULLPTR;
            o.via.array.size = 0;
        }
        else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            msghandle::object* const pend = p + size;
            o.via.array.ptr = p;
            o.via.array.size = size;
            std::wstring::const_iterator it(v.begin());
            do {
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif // defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
                *p = msghandle::object(*it, o.zone);
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif // defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
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

#endif // MSGHANDLE_V1_TYPE_WSTRING_HPP
