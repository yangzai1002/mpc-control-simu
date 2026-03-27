//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_INT_HPP
#define MSGHANDLE_V1_TYPE_INT_HPP

#include "msghandle/v1/adaptor/int_decl.hpp"
#include "msghandle/object.hpp"

#include <limits>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1){
/// @endcond

namespace type {
namespace detail {

template <typename T>
struct convert_integer_sign<T, true> {
    static T convert(msghandle::object const& o) {
        if(o.type == msghandle::type::POSITIVE_INTEGER) {
            if(o.via.u64 > static_cast<uint64_t>(std::numeric_limits<T>::max()))
            { throw msghandle::type_error(); }
            return static_cast<T>(o.via.u64);
        } else if(o.type == msghandle::type::NEGATIVE_INTEGER) {
            if(o.via.i64 < static_cast<int64_t>(std::numeric_limits<T>::min()))
            { throw msghandle::type_error(); }
            return static_cast<T>(o.via.i64);
        }
        throw msghandle::type_error();
    }
};

template <typename T>
struct convert_integer_sign<T, false> {
    static T convert(msghandle::object const& o) {
        if(o.type == msghandle::type::POSITIVE_INTEGER) {
            if(o.via.u64 > static_cast<uint64_t>(std::numeric_limits<T>::max()))
            { throw msghandle::type_error(); }
            return static_cast<T>(o.via.u64);
        }
        throw msghandle::type_error();
    }
};

template <typename T>
struct is_signed {
    static const bool value = std::numeric_limits<T>::is_signed;
};

template <typename T>
inline T convert_integer(msghandle::object const& o)
{
    return detail::convert_integer_sign<T, is_signed<T>::value>::convert(o);
}

template <>
struct object_sign<true> {
    template <typename T>
    static void make(msghandle::object& o, T v) {
        if (v < 0) {
            o.type = msghandle::type::NEGATIVE_INTEGER;
            o.via.i64 = v;
        }
        else {
            o.type = msghandle::type::POSITIVE_INTEGER;
            o.via.u64 = static_cast<uint64_t>(v);
        }
    }
};

template <>
struct object_sign<false> {
    template <typename T>
    static void make(msghandle::object& o, T v) {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
};

template <typename T>
inline void object_char(msghandle::object& o, T v) {
    return object_sign<is_signed<T>::value>::make(o, v);
}

}  // namespace detail
}  // namespace type

namespace adaptor {

template <>
struct convert<char> {
    msghandle::object const& operator()(msghandle::object const& o, char& v) const
    { v = type::detail::convert_integer<char>(o); return o; }
};

template <>
struct convert<wchar_t> {
    msghandle::object const& operator()(msghandle::object const& o, wchar_t& v) const
    { v = type::detail::convert_integer<wchar_t>(o); return o; }
};

template <>
struct convert<signed char> {
    msghandle::object const& operator()(msghandle::object const& o, signed char& v) const
    { v = type::detail::convert_integer<signed char>(o); return o; }
};

template <>
struct convert<signed short> {
    msghandle::object const& operator()(msghandle::object const& o, signed short& v) const
    { v = type::detail::convert_integer<signed short>(o); return o; }
};

template <>
struct convert<signed int> {
    msghandle::object const& operator()(msghandle::object const& o, signed int& v) const
    { v = type::detail::convert_integer<signed int>(o); return o; }
};

template <>
struct convert<signed long> {
    msghandle::object const& operator()(msghandle::object const& o, signed long& v) const
    { v = type::detail::convert_integer<signed long>(o); return o; }
};

template <>
struct convert<signed long long> {
    msghandle::object const& operator()(msghandle::object const& o, signed long long& v) const
    { v = type::detail::convert_integer<signed long long>(o); return o; }
};


template <>
struct convert<unsigned char> {
    msghandle::object const& operator()(msghandle::object const& o, unsigned char& v) const
    { v = type::detail::convert_integer<unsigned char>(o); return o; }
};

template <>
struct convert<unsigned short> {
    msghandle::object const& operator()(msghandle::object const& o, unsigned short& v) const
    { v = type::detail::convert_integer<unsigned short>(o); return o; }
};

template <>
struct convert<unsigned int> {
    msghandle::object const& operator()(msghandle::object const& o, unsigned int& v) const
    { v = type::detail::convert_integer<unsigned int>(o); return o; }
};

template <>
struct convert<unsigned long> {
    msghandle::object const& operator()(msghandle::object const& o, unsigned long& v) const
    { v = type::detail::convert_integer<unsigned long>(o); return o; }
};

template <>
struct convert<unsigned long long> {
    msghandle::object const& operator()(msghandle::object const& o, unsigned long long& v) const
    { v = type::detail::convert_integer<unsigned long long>(o); return o; }
};


template <>
struct pack<char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, char v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_char(v);
        return o;
    }
};

template <>
struct pack<wchar_t> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, wchar_t v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_wchar(v);
        return o;
    }
};

template <>
struct pack<signed char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed char v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_signed_char(v);
        return o;
    }
};

template <>
struct pack<signed short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed short v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_short(v);
        return o;
    }
};

template <>
struct pack<signed int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed int v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_int(v);
        return o;
    }
};

template <>
struct pack<signed long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_long(v);
        return o;
    }
};

template <>
struct pack<signed long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long long v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_long_long(v);
        return o;
    }
};


template <>
struct pack<unsigned char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned char v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_unsigned_char(v);
        return o;
    }
};

template <>
struct pack<unsigned short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned short v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_unsigned_short(v);
        return o;
    }
};

template <>
struct pack<unsigned int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned int v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_unsigned_int(v);
        return o;
    }
};

template <>
struct pack<unsigned long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_unsigned_long(v);
        return o;
    }
};

template <>
struct pack<unsigned long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long long v,SMsgHandleInfo& msghandle_info) const
    {
        if(msghandle_info.is_vector_T==1)
            msghandle_info.is_continuous_memory=1;
        else
            o.pack_unsigned_long_long(v);
        return o;
    }
};




template <>
struct print<char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, char v,const std::string& k) const
    {
        o.stream() <<k<<"<int8>"<<":"<<std::to_string(static_cast<int>(v))<<std::endl;
        return o;
    }
};

template <>
struct print<wchar_t> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, wchar_t v,const std::string& k) const
    {
        o.stream() <<k<<"<int32>"<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct print<signed char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed char v,const std::string& k) const
    {
        o.stream() <<k<<"<int8>"<<":"<<std::to_string(static_cast<int>(v))<<std::endl;
        return o;
    }
};

template <>
struct print<signed short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed short v,const std::string& k) const
    {
        o.stream() <<k<<"<int16>"<<":"<<std::to_string(static_cast<int>(v))<<std::endl;
        return o;
    }
};

template <>
struct print<signed int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed int v,const std::string& k) const
    {
        o.stream() <<k<<"<int32>"<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct print<signed long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long v,const std::string& k) const
    {
        o.stream() <<k<<"<int64>"<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct print<signed long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long long v,const std::string& k) const
    {
        o.stream() <<k<<"<int64>"<<":"<<v<<std::endl;
        return o;
    }
};


template <>
struct print<unsigned char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned char v,const std::string& k) const
    {
        o.stream() <<k<<"<uint8>"<<":"<<std::to_string(static_cast<int>(v))<<std::endl;
        return o;
    }
};

template <>
struct print<unsigned short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned short v,const std::string& k) const
    {
        o.stream() <<k<<"<uint16>"<<":"<<std::to_string(static_cast<int>(v))<<std::endl;
        return o;
    }
};

template <>
struct print<unsigned int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned int v,const std::string& k) const
    {
        o.stream() <<k<<"<uint32>"<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct print<unsigned long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long v,const std::string& k) const
    {
        o.stream() <<k<<"<uint64>"<<":"<<v<<std::endl;
        return o;
    }
};

template <>
struct print<unsigned long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long long v,const std::string& k) const
    {
        o.stream() <<k<<"<uint64>"<<":"<<v<<std::endl;
        return o;
    }
};




template <>
struct checkcode<char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, char v,const std::string& k) const
    {
        o.stream() <<k<<"char"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<wchar_t> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, wchar_t v,const std::string& k) const
    {
        o.stream() <<k<<"wchar_t"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<signed char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed char v,const std::string& k) const
    {
        o.stream() <<k<<"signed char"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<signed short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed short v,const std::string& k) const
    {
        o.stream() <<k<<"signed short"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<signed int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed int v,const std::string& k) const
    {
        o.stream() <<k<<"signed int"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<signed long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long v,const std::string& k) const
    {
        o.stream() <<k<<"signed long"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<signed long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long long v,const std::string& k) const
    {
        o.stream() <<k<<"signed long long"<<std::endl;
        return o;
    }
};


template <>
struct checkcode<unsigned char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned char v,const std::string& k) const
    {
        o.stream() <<k<<"unsigned char"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<unsigned short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned short v,const std::string& k) const
    {
        o.stream() <<k<<"unsigned short"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<unsigned int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned int v,const std::string& k) const
    {
        o.stream() <<k<<"unsigned int"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<unsigned long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long v,const std::string& k) const
    {
        o.stream() <<k<<"unsigned long"<<std::endl;
        return o;
    }
};

template <>
struct checkcode<unsigned long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long long v,const std::string& k) const
    {
        o.stream() <<k<<"unsigned long long"<<std::endl;
        return o;
    }
};


template <>
struct reflectget<char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, char v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<wchar_t> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, wchar_t v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<signed char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed char v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<signed short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed short v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<signed int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed int v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<signed long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<signed long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, signed long long v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};


template <>
struct reflectget<unsigned char> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned char v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<unsigned short> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned short v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<unsigned int> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned int v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<unsigned long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct reflectget<unsigned long long> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, unsigned long long v,const std::string& k, const std::string filter) const
    {
        o.stream() <<v<<std::endl;
        return o;
    }
};




template <>
struct object<char> {
    void operator()(msghandle::object& o, char v) const
    { type::detail::object_char(o, v); }
};

template <>
struct object<wchar_t> {
    void operator()(msghandle::object& o, wchar_t v) const
    { type::detail::object_char(o, v); }
};

template <>
struct object<signed char> {
    void operator()(msghandle::object& o, signed char v) const {
        if (v < 0) {
            o.type = msghandle::type::NEGATIVE_INTEGER;
            o.via.i64 = v;
        }
        else {
            o.type = msghandle::type::POSITIVE_INTEGER;
            o.via.u64 = static_cast<uint64_t>(v);
        }
    }
};

template <>
struct object<signed short> {
    void operator()(msghandle::object& o, signed short v) const {
        if (v < 0) {
            o.type = msghandle::type::NEGATIVE_INTEGER;
            o.via.i64 = v;
        }
        else {
            o.type = msghandle::type::POSITIVE_INTEGER;
            o.via.u64 = static_cast<uint64_t>(v);
        }
    }
};

template <>
struct object<signed int> {
    void operator()(msghandle::object& o, signed int v) const {
        if (v < 0) {
            o.type = msghandle::type::NEGATIVE_INTEGER;
            o.via.i64 = v;
        }
        else {
            o.type = msghandle::type::POSITIVE_INTEGER;
            o.via.u64 = static_cast<uint64_t>(v);
        }
    }
};

template <>
struct object<signed long> {
    void operator()(msghandle::object& o, signed long v) const {
        if (v < 0) {
            o.type = msghandle::type::NEGATIVE_INTEGER;
            o.via.i64 = v;
        }
        else {
            o.type = msghandle::type::POSITIVE_INTEGER;
            o.via.u64 = static_cast<uint64_t>(v);
        }
    }
};

template <>
struct object<signed long long> {
    void operator()(msghandle::object& o, signed long long v) const {
        if (v < 0) {
            o.type = msghandle::type::NEGATIVE_INTEGER;
            o.via.i64 = v;
        }
        else{
            o.type = msghandle::type::POSITIVE_INTEGER;
            o.via.u64 = static_cast<uint64_t>(v);
        }
    }
};

template <>
struct object<unsigned char> {
    void operator()(msghandle::object& o, unsigned char v) const {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
};

template <>
struct object<unsigned short> {
    void operator()(msghandle::object& o, unsigned short v) const {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
};

template <>
struct object<unsigned int> {
    void operator()(msghandle::object& o, unsigned int v) const {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
};

template <>
struct object<unsigned long> {
    void operator()(msghandle::object& o, unsigned long v) const {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
};

template <>
struct object<unsigned long long> {
    void operator()(msghandle::object& o, unsigned long long v) const {
        o.type = msghandle::type::POSITIVE_INTEGER;
        o.via.u64 = v;
    }
};


template <>
struct object_with_zone<char> {
    void operator()(msghandle::object::with_zone& o, char v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<wchar_t> {
    void operator()(msghandle::object::with_zone& o, wchar_t v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<signed char> {
    void operator()(msghandle::object::with_zone& o, signed char v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<signed short> {
    void operator()(msghandle::object::with_zone& o, signed short v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<signed int> {
    void operator()(msghandle::object::with_zone& o, signed int v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<signed long> {
    void operator()(msghandle::object::with_zone& o, signed long v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<signed long long> {
    void operator()(msghandle::object::with_zone& o, const signed long long& v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<unsigned char> {
    void operator()(msghandle::object::with_zone& o, unsigned char v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<unsigned short> {
    void operator()(msghandle::object::with_zone& o, unsigned short v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<unsigned int> {
    void operator()(msghandle::object::with_zone& o, unsigned int v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<unsigned long> {
    void operator()(msghandle::object::with_zone& o, unsigned long v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<unsigned long long> {
    void operator()(msghandle::object::with_zone& o, const unsigned long long& v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_INT_HPP
