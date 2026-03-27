//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_CPP11_TIMESPEC_HPP
#define MSGHANDLE_V1_TYPE_CPP11_TIMESPEC_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/object.hpp"

#include <ctime>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct convert<timespec> {
    msghandle::object const& operator()(msghandle::object const& o, timespec& v) const {
        if(o.type != msghandle::type::EXT) { throw msghandle::type_error(); }
        if(o.via.ext.type() != -1) { throw msghandle::type_error(); }
        switch(o.via.ext.size) {
        case 4: {
            uint32_t sec;
            _msghandle_load32(uint32_t, o.via.ext.data(), &sec);
            v.tv_sec = static_cast<decltype(v.tv_sec)>(sec);
            v.tv_nsec = 0;
        } break;
        case 8: {
            uint64_t value;
            _msghandle_load64(uint64_t, o.via.ext.data(), &value);
            v.tv_sec = static_cast<decltype(v.tv_sec)>(value & 0x00000003ffffffffLL);
            v.tv_nsec= static_cast<decltype(v.tv_nsec)>(value >> 34);
        } break;
        case 12: {
            uint32_t nanosec;
            _msghandle_load32(uint32_t, o.via.ext.data(), &nanosec);
            int64_t sec;
            _msghandle_load64(int64_t, o.via.ext.data() + 4, &sec);
            v.tv_sec = static_cast<decltype(v.tv_sec)>(sec);
            v.tv_nsec = static_cast<decltype(v.tv_nsec)>(nanosec);
        } break;
        default:
            throw msghandle::type_error();
        }
        return o;
    }
};

template <>
struct pack<timespec> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const timespec& v) const {
        if ((static_cast<uint64_t>(v.tv_sec) >> 34) == 0) {
            uint64_t data64 = (static_cast<uint64_t>(v.tv_nsec) << 34) | static_cast<uint64_t>(v.tv_sec);
            if ((data64 & 0xffffffff00000000L) == 0) {
                // timestamp 32
                o.pack_ext(4, -1);
                uint32_t data32 = static_cast<uint32_t>(data64);
                char buf[4];
                _msghandle_store32(buf, data32);
                o.pack_ext_body(buf, 4);
            }
            else {
                // timestamp 64
                o.pack_ext(8, -1);
                char buf[8];
                _msghandle_store64(buf, data64);
                o.pack_ext_body(buf, 8);
            }
        }
        else {
            // timestamp 96
            o.pack_ext(12, -1);
            char buf[12];
            _msghandle_store32(&buf[0], static_cast<uint32_t>(v.tv_nsec));
            _msghandle_store64(&buf[4], v.tv_sec);
            o.pack_ext_body(buf, 12);
        }
        return o;
    }
};

template <>
struct object_with_zone<timespec> {
    void operator()(msghandle::object::with_zone& o, const timespec& v) const {
        if ((static_cast<uint64_t>(v.tv_sec) >> 34) == 0) {
            uint64_t data64 = (static_cast<uint64_t>(v.tv_nsec) << 34) | static_cast<uint64_t>(v.tv_sec);
            if ((data64 & 0xffffffff00000000L) == 0) {
                // timestamp 32
                o.type = msghandle::type::EXT;
                o.via.ext.size = 4;
                char* p = static_cast<char*>(o.zone.allocate_no_align(o.via.ext.size + 1));
                p[0] = static_cast<char>(-1);
                uint32_t data32 = static_cast<uint32_t>(data64);
                _msghandle_store32(&p[1], data32);
                o.via.ext.ptr = p;
            }
            else {
                // timestamp 64
                o.type = msghandle::type::EXT;
                o.via.ext.size = 8;
                char* p = static_cast<char*>(o.zone.allocate_no_align(o.via.ext.size + 1));
                p[0] = static_cast<char>(-1);
                _msghandle_store64(&p[1], data64);
                o.via.ext.ptr = p;
            }
        }
        else {
            // timestamp 96
            o.type = msghandle::type::EXT;
            o.via.ext.size = 12;
            char* p = static_cast<char*>(o.zone.allocate_no_align(o.via.ext.size + 1));
            p[0] = static_cast<char>(-1);
            _msghandle_store32(&p[1], static_cast<uint32_t>(v.tv_nsec));
            _msghandle_store64(&p[1 + 4], v.tv_sec);
            o.via.ext.ptr = p;
        }
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CPP11_TIMESPEC_HPP
