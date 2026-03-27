//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_VECTOR_HPP
#define MSGHANDLE_V1_TYPE_VECTOR_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>


namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
    /// @endcond

    namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

    template <typename T, typename Alloc>
    struct as<std::vector<T, Alloc>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
        std::vector<T, Alloc> operator()(const msghandle::object& o) const {
            if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
            std::vector<T, Alloc> v;
            v.reserve(o.via.array.size);
            if (o.via.array.size > 0) {
                msghandle::object* p = o.via.array.ptr;
                msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
                do {
                    v.push_back(p->as<T>());
                    ++p;
                } while (p < pend);
            }
            return v;
        }
    };

#endif // !defined(MSGHANDLE_USE_CPP03)

    template <typename T, typename Alloc>
    struct convert<std::vector<T, Alloc> > {
        msghandle::object const& operator()(msghandle::object const& o, std::vector<T, Alloc>& v) const {
            auto start = std::chrono::high_resolution_clock::now();
            switch (o.type) {

            case msghandle::type::BIN:
            {
                std::cout << "o.via.bin.size = " << o.via.bin.size
                          <<",sizeof(T) = " << sizeof(T)
                          <<",resize = " << o.via.bin.size/sizeof(T)
                          <<std::endl;
                v.resize(o.via.bin.size/sizeof(T));
                auto start1 = std::chrono::high_resolution_clock::now();
                std::memcpy(&v.front(), o.via.bin.ptr, o.via.bin.size);
                auto start2 = std::chrono::high_resolution_clock::now();

                std::cout << "代码段start1耗时: " << std::chrono::duration_cast<std::chrono::milliseconds>(start1 - start).count() << "毫秒" << std::endl;
                std::cout << "代码段start2耗时: " << std::chrono::duration_cast<std::chrono::milliseconds>(start2 - start1).count() << "毫秒" << std::endl;
            }
                break;
            case msghandle::type::ARRAY:
            {
                v.resize(o.via.array.size);
                if (o.via.array.size > 0) {
                    msghandle::object* p = o.via.array.ptr;
                    msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
                    typename std::vector<T, Alloc>::iterator it = v.begin();
                    do {
                        p->convert(*it);
                        ++p;
                        ++it;
                    } while(p < pend);
                }
            }
                break;
            default:
                throw msghandle::type_error();
                break;
            }
            auto end = std::chrono::high_resolution_clock::now();

            // 计算持续时间
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            // 打印耗时
            std::cout << "代码段耗时: " << duration.count() << "毫秒" << std::endl;
            return o;
        }
    };

    template <typename T, typename Alloc>
    struct pack<std::vector<T, Alloc> > {
        template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::vector<T, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
            if(msghandle_info.is_vector_T==1){
                msghandle_info.is_continuous_memory=0;
                return o;}
            uint32_t size = checked_get_container_size(v.size());
            msghandle_info.is_vector_T = 1;
            if(size>0)
            {
                o.pack(v[0],msghandle_info);
                msghandle_info.is_vector_T = 0;
                if(msghandle_info.is_continuous_memory)
                {
                    o.pack_bin(size*sizeof(T));
                    if (size != 0) {
                        o.pack_bin_body(reinterpret_cast<char const*>(&v.front()), size*sizeof(T));
                    }
                }
                else
                {
                    o.pack_array(size);
                    for (typename std::vector<T, Alloc>::const_iterator it(v.begin()), it_end(v.end());
                         it != it_end; ++it) {
                        o.pack(*it,msghandle_info);
                    }
                }
            }
            else
                o.pack_array(size);
            msghandle_info.is_vector_T = 0;
            return o;
        }
    };

    template <typename T, typename Alloc>
    struct print<std::vector<T, Alloc> > {
        template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::vector<T, Alloc>& v,const std::string& k) const {

            int index =0;
            o.stream()<< k <<"<std::vector>"<<":\n";
            if(v.size() > 128)
            {
                std::string key = " "+k+"["+std::to_string(v.size()-1) + "]";
                o.print(v[v.size()-1],key);
                return o;
            }
            for (typename std::vector<T, Alloc>::const_iterator it(v.begin()), it_end(v.end());
                 it != it_end; ++it) {
                std::string key = " "+k+"["+std::to_string(index) + "]";
                o.print(*it,key);
                index++;

            }
            return o;
        }
    };

    template <typename T, typename Alloc>
    struct checkcode<std::vector<T, Alloc> > {
        template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::vector<T, Alloc>& v,const std::string& k) const {

            o.stream()<< k <<"std::vector\n";
            std::string key = " "+k;
            if(v.size()==0)
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

    template <typename T, typename Alloc>
    struct reflectget<std::vector<T, Alloc> > {
        template <typename Stream>
        msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::vector<T, Alloc>& v,const std::string& k, const std::string filter) const {

            int index =0;
            for (typename std::vector<T, Alloc>::const_iterator it(v.begin()), it_end(v.end());
                 it != it_end; ++it) {
                std::string key = "["+std::to_string(index) + "]";
                std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
                std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
                if(key == result1)
                    o.reflectget(*it,key,result2);
                index++;
            }
            return o;
        }
    };

    template <typename T, typename Alloc>
    struct object_with_zone<std::vector<T, Alloc> > {
        void operator()(msghandle::object::with_zone& o, const std::vector<T, Alloc>& v) const {
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
                typename std::vector<T, Alloc>::const_iterator it(v.begin());
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

#endif // MSGHANDLE_V1_TYPE_VECTOR_HPP
