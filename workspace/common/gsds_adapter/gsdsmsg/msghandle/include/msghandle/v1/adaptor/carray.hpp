//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_CARRAY_HPP
#define MSGHANDLE_V1_TYPE_CARRAY_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/object_fwd.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename T, std::size_t N>
struct convert<T[N]> {
    msghandle::object const& operator()(msghandle::object const& o, T* v) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        if (o.via.array.size > N) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr;
        msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
        do {
            p->convert(*v);
            ++p;
            ++v;
        } while(p < pend);
        return o;
    }
};

template <std::size_t N>
struct convert<char[N]> {
    msghandle::object const& operator()(msghandle::object const& o, char(&v)[N]) const {
        switch (o.type) {
        case msghandle::type::BIN:
            if (o.via.bin.size > N) { throw msghandle::type_error(); }
            std::memcpy(v, o.via.bin.ptr, o.via.bin.size);
            break;
        case msghandle::type::STR:
            if (o.via.str.size > N) { throw msghandle::type_error(); }
            std::memcpy(v, o.via.str.ptr, o.via.str.size);
            if (o.via.str.size < N) v[o.via.str.size] = '\0';
            break;
        default:
            throw msghandle::type_error();
            break;
        }
        return o;
    }
};

template <std::size_t N>
struct convert<unsigned char[N]> {
    msghandle::object const& operator()(msghandle::object const& o, unsigned char(&v)[N]) const {
        switch (o.type) {
        case msghandle::type::BIN:
            if (o.via.bin.size > N) { throw msghandle::type_error(); }
            std::memcpy(v, o.via.bin.ptr, o.via.bin.size);
            break;
        case msghandle::type::STR:
            if (o.via.str.size > N) { throw msghandle::type_error(); }
            std::memcpy(v, o.via.str.ptr, o.via.str.size);
            if (o.via.str.size < N) v[o.via.str.size] = '\0';
            break;
        default:
            throw msghandle::type_error();
            break;
        }
        return o;
    }
};


template <typename T, std::size_t N>
struct pack<T[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const T(&v)[N],SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(N);
        o.pack_array(size);
        const T* ptr = v;
        for (; ptr != &v[N]; ++ptr) o.pack(*ptr);
        return o;
    }
};

template <std::size_t N>
struct pack<char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.pack_str(adjusted_size);
        o.pack_str_body(p, adjusted_size);
        return o;
    }
};

template <std::size_t N>
struct pack<const char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(N);
        char const* p = v;
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.pack_str(adjusted_size);
        o.pack_str_body(p, adjusted_size);
        return o;
    }
};

template <std::size_t N>
struct pack<unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        o.pack_bin(size);
        o.pack_bin_body(reinterpret_cast<char const*>(p), size);
        return o;
    }
};

template <std::size_t N>
struct pack<const unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        o.pack_bin(size);
        o.pack_bin_body(reinterpret_cast<char const*>(p), size);
        return o;
    }
};


//print
template <typename T, std::size_t N>
struct print<T[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const T(&v)[N],const std::string& k) const {
        int index =0;
        o.stream()<< k << "<T[N]>" << ":\n";
        const T* ptr = v;
        for (; ptr != &v[N]; ++ptr)
        {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(*ptr,key);
            index++;
        }
        return o;
    }
};

template <std::size_t N>
struct print<char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],const std::string& k) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        std::string tmp_str(p, adjusted_size);
        o.stream() <<k<<"<char[N]>"<<":"<<tmp_str<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct print<const char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],const std::string& k) const {
        uint32_t size = checked_get_container_size(N);
        char const* p = v;
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        std::string tmp_str(p, adjusted_size);
        o.stream() <<k<<"<const char[N]>"<<":"<<tmp_str<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct print<unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],const std::string& k) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        std::string tmp_str(reinterpret_cast<char const*>(p), size);
        o.stream() <<k<<"<unsigned char[N]>"<<":"<<tmp_str<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct print<const unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],const std::string& k) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        std::string tmp_str(reinterpret_cast<char const*>(p), size);
        o.stream() <<k<<"<const unsigned char[N]>"<<":"<<tmp_str<<std::endl;
        return o;
    }
};

//checkcode
template <typename T, std::size_t N>
struct checkcode<T[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const T(&v)[N],const std::string& k) const {
        uint32_t size = checked_get_container_size(N);
        o.stream()<< k <<"std::vector\n";
        std::string key = " "+k;
        if(size==0)
        {
            T it;
            o.checkcode(it,key);
        }
        else
        {
            o.checkcode(v[0],key);
        }
        return o;
    }
};

template <std::size_t N>
struct checkcode<char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],const std::string& k) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.stream() <<k<<"char"<<adjusted_size<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct checkcode<const char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],const std::string& k) const {
        uint32_t size = checked_get_container_size(N);
        char const* p = v;
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.stream() <<k<<"const char"<<adjusted_size<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct checkcode<unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],const std::string& k) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        o.stream() <<k<<"unsigned char"<<size<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct checkcode<const unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],const std::string& k) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        o.stream() <<k<<"const unsigned char"<<size<<std::endl;
        return o;
    }
};
//



//reflectget
template <typename T, std::size_t N>
struct reflectget<T[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const T(&v)[N],const std::string& k, const std::string filter) const {
        int index =0;
        const T* ptr = v;
        for (; ptr != &v[N]; ++ptr)
        {
            std::string key = "["+std::to_string(index) + "]";
            std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
            std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
            if(key == result1)
                o.reflectget(*ptr,key,result2);
            index++;
        }
        return o;
    }
};

template <std::size_t N>
struct reflectget<char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],const std::string& k, const std::string filter) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        std::string tmp_str(p, adjusted_size);
        o.stream() <<tmp_str<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct reflectget<const char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const char(&v)[N],const std::string& k, const std::string filter) const {
        uint32_t size = checked_get_container_size(N);
        char const* p = v;
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        std::string tmp_str(p, adjusted_size);
        o.stream() <<tmp_str<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct reflectget<unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],const std::string& k, const std::string filter) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        std::string tmp_str(reinterpret_cast<char const*>(p), size);
        o.stream() <<tmp_str<<std::endl;
        return o;
    }
};

template <std::size_t N>
struct reflectget<const unsigned char[N]> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const unsigned char(&v)[N],const std::string& k, const std::string filter) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        std::string tmp_str(reinterpret_cast<char const*>(p), size);
        o.stream() <<tmp_str<<std::endl;
        return o;
    }
};
//

template <typename T, std::size_t N>
struct object_with_zone<T[N]> {
    void operator()(msghandle::object::with_zone& o, const T(&v)[N]) const {
        uint32_t size = checked_get_container_size(N);
        o.type = msghandle::type::ARRAY;
        msghandle::object* ptr = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object) * size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        o.via.array.ptr = ptr;
        o.via.array.size = size;
        const T* pv = v;
        for (; pv != &v[size]; ++pv) {
            *ptr++ = msghandle::object(*pv, o.zone);
        }
    }
};

template <std::size_t N>
struct object_with_zone<char[N]> {
    void operator()(msghandle::object::with_zone& o, const char(&v)[N]) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.type = msghandle::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(adjusted_size, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.str.ptr = ptr;
        o.via.str.size = adjusted_size;
        std::memcpy(ptr, p, adjusted_size);
    }
};

template <std::size_t N>
struct object_with_zone<const char[N]> {
    void operator()(msghandle::object::with_zone& o, const char(&v)[N]) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.type = msghandle::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(adjusted_size, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.str.ptr = ptr;
        o.via.str.size = adjusted_size;
        std::memcpy(ptr, p, adjusted_size);
    }
};

template <std::size_t N>
struct object_with_zone<unsigned char[N]> {
    void operator()(msghandle::object::with_zone& o, const unsigned char(&v)[N]) const {
        uint32_t size = checked_get_container_size(N);
        o.type = msghandle::type::BIN;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.bin.ptr = ptr;
        o.via.bin.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <std::size_t N>
struct object_with_zone<const unsigned char[N]> {
    void operator()(msghandle::object::with_zone& o, const unsigned char(&v)[N]) const {
        uint32_t size = checked_get_container_size(N);
        o.type = msghandle::type::BIN;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.bin.ptr = ptr;
        o.via.bin.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <std::size_t N>
struct object<char[N]> {
    void operator()(msghandle::object& o, const char(&v)[N]) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.type = msghandle::type::STR;
        o.via.str.ptr = p;
        o.via.str.size = adjusted_size;
    }
};

template <std::size_t N>
struct object<const char[N]> {
    void operator()(msghandle::object& o, const char(&v)[N]) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(N);
        char const* p2 = static_cast<char const*>(std::memchr(p, '\0', size));
        uint32_t adjusted_size = p2 ? static_cast<uint32_t>(p2 - p) : size;
        o.type = msghandle::type::STR;
        o.via.str.ptr = p;
        o.via.str.size = adjusted_size;
    }
};


} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CARRAY_HPP
