//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_MAP_HPP
#define MSGHANDLE_V1_TYPE_MAP_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/cpp_version.hpp"
#include "msghandle/v1/adaptor/map_decl.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"

#include <map>
#include <vector>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

template <typename K, typename V, typename Compare, typename Alloc>
class assoc_vector : public std::vector< std::pair<K, V>, Alloc > {
#if !defined(MSGHANDLE_USE_CPP03)
    using std::vector<std::pair<K, V>, Alloc>::vector;
#endif // !defined(MSGHANDLE_USE_CPP03)
};

namespace detail {
    template <typename K, typename V, typename Compare, typename Alloc>
    struct pair_first_less {
        bool operator() (const std::pair<K, V>& x, const std::pair<K, V>& y) const
            { return Compare()(x.first, y.first); }
    };
}

}  //namespace type

namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

template <typename K, typename V, typename Compare, typename Alloc>
struct as<
    type::assoc_vector<K, V, Compare, Alloc>,
    typename std::enable_if<msghandle::has_as<K>::value || msghandle::has_as<V>::value>::type> {
    type::assoc_vector<K, V, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        type::assoc_vector<K, V, Compare, Alloc> v;
        v.reserve(o.via.map.size);
        msghandle::object_kv* p = o.via.map.ptr;
        msghandle::object_kv* const pend = o.via.map.ptr + o.via.map.size;
        for (; p < pend; ++p) {
            v.emplace_back(p->key.as<K>(), p->val.as<V>());
        }
        std::sort(v.begin(), v.end(), type::detail::pair_first_less<K, V, Compare, Alloc>());
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename K, typename V, typename Compare, typename Alloc>
struct convert<type::assoc_vector<K, V, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, type::assoc_vector<K, V, Compare, Alloc>& v) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        v.resize(o.via.map.size);
        if (o.via.map.size != 0) {
            msghandle::object_kv* p = o.via.map.ptr;
            msghandle::object_kv* const pend = o.via.map.ptr + o.via.map.size;
            std::pair<K, V>* it(&v.front());
            for (; p < pend; ++p, ++it) {
                p->key.convert(it->first);
                p->val.convert(it->second);
            }
            std::sort(v.begin(), v.end(), type::detail::pair_first_less<K, V, Compare, Alloc>());
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct pack<type::assoc_vector<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const type::assoc_vector<K, V, Compare, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for (typename type::assoc_vector<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct print<type::assoc_vector<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const type::assoc_vector<K, V, Compare, Alloc>& v,const std::string& k) const {

        int index =0;
        o.stream()<< k << "<type::assoc_vector>"<<":\n";
        for (typename type::assoc_vector<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(*it->first,key);
            o.print(*it->second,key);
            index++;
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct checkcode<type::assoc_vector<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const type::assoc_vector<K, V, Compare, Alloc>& v,const std::string& k) const {

        o.stream()<< k <<"std::map\n";
        std::string key = " "+k;
        if(v.size()==0)
        {
            K Kit;
            o.checkcode(Kit,key);
            V Vlu;
            o.checkcode(Vlu,key);
        }
        else
        {
            o.checkcode(*v.begin()->first,key);
            o.checkcode(*v.begin()->second,key);
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct reflectget<type::assoc_vector<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const type::assoc_vector<K, V, Compare, Alloc>& v,const std::string& k, const std::string filter) const {

        int index =0;
        for (typename type::assoc_vector<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = "["+std::to_string(index) + "]";
            std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
            std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
            if(result1 == "first")
                o.reflectget(*it->first,key,result2);
            else if(result1 == "second")
                o.reflectget(*it->second,key,result2);
            index++;
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct object_with_zone<type::assoc_vector<K, V, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const type::assoc_vector<K, V, Compare, Alloc>& v) const {
        o.type = msghandle::type::MAP;
        if (v.empty()) {
            o.via.map.ptr  = MSGHANDLE_NULLPTR;
            o.via.map.size = 0;
        }
        else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object_kv* p = static_cast<msghandle::object_kv*>(o.zone.allocate_align(sizeof(msghandle::object_kv)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv)));
            msghandle::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename type::assoc_vector<K, V, Compare, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msghandle::object(it->first, o.zone);
                p->val = msghandle::object(it->second, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

#if !defined(MSGHANDLE_USE_CPP03)

template <typename K, typename V, typename Compare, typename Alloc>
struct as<
    std::map<K, V, Compare, Alloc>,
    typename std::enable_if<msghandle::has_as<K>::value || msghandle::has_as<V>::value>::type> {
    std::map<K, V, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::map<K, V, Compare, Alloc> v;
        for (; p != pend; ++p) {
            v.emplace(p->key.as<K>(), p->val.as<V>());
        }
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename K, typename V, typename Compare, typename Alloc>
struct convert<std::map<K, V, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, std::map<K, V, Compare, Alloc>& v) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::map<K, V, Compare, Alloc> tmp;
        for (; p != pend; ++p) {
            K key;
            p->key.convert(key);
#if MSGHANDLE_CPP_VERSION >= 201103L
            p->val.convert(tmp[std::move(key)]);
#else
            p->val.convert(tmp[key]);
#endif
        }
#if MSGHANDLE_CPP_VERSION >= 201103L
        v = std::move(tmp);
#else
        tmp.swap(v);
#endif
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct pack<std::map<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::map<K, V, Compare, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for (typename std::map<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct print<std::map<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::map<K, V, Compare, Alloc>& v,const std::string& k) const {

        int index =0;
        o.stream()<< k << "<std::map>"<<":\n";
        for (typename std::map<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(it->first,key);
            o.print(it->second,key);
            index++;
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct checkcode<std::map<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::map<K, V, Compare, Alloc>& v,const std::string& k) const {

        o.stream()<< k <<"std::map\n";
        std::string key = " "+k;
        if(v.size()==0)
        {
            K Kit;
            o.checkcode(Kit,key);
            V Vlu;
            o.checkcode(Vlu,key);
        }
        else
        {
            o.checkcode(v.begin()->first,key);
            o.checkcode(v.begin()->second,key);
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct reflectget<std::map<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::map<K, V, Compare, Alloc>& v,const std::string& k, const std::string filter) const {

        int index =0;
        for (typename std::map<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = "["+std::to_string(index) + "]";
            std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
            std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
            if(result1 == key)
            {
                std::string result2_1 = result2.substr(0, result2.find(".")); // 截取到第一个"."之前的部分
                std::string result2_2 = result2.substr(result2.find(".") + 1); // 截取第一个"."之后的部分
                std::string key_2 = result2_2.substr(0, result2_2.find("."));
                if(result2_1 == "first")
                    o.reflectget(it->first,key_2,result2_2);
                else if(result2_1 == "second")
                    o.reflectget(it->second,key_2,result2_2);
            }
            index++;
        }
        return o;
    }
};


template <typename K, typename V, typename Compare, typename Alloc>
struct object_with_zone<std::map<K, V, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const std::map<K, V, Compare, Alloc>& v) const {
        o.type = msghandle::type::MAP;
        if (v.empty()) {
            o.via.map.ptr  = MSGHANDLE_NULLPTR;
            o.via.map.size = 0;
        }
        else {
            uint32_t size = checked_get_container_size(v.size());

            msghandle::object_kv* p = static_cast<msghandle::object_kv*>(o.zone.allocate_align(sizeof(msghandle::object_kv)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv)));
            msghandle::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename std::map<K, V, Compare, Alloc>::const_iterator it(v.begin());
            do {
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif // defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
                p->key = msghandle::object(it->first, o.zone);
                p->val = msghandle::object(it->second, o.zone);
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif // defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && !defined(__clang__)
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

#if !defined(MSGHANDLE_USE_CPP03)

template <typename K, typename V, typename Compare, typename Alloc>
struct as<
    std::multimap<K, V, Compare, Alloc>,
    typename std::enable_if<msghandle::has_as<K>::value || msghandle::has_as<V>::value>::type> {
    std::multimap<K, V, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::multimap<K, V, Compare, Alloc> v;
        for (; p != pend; ++p) {
            v.emplace(p->key.as<K>(), p->val.as<V>());
        }
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename K, typename V, typename Compare, typename Alloc>
struct convert<std::multimap<K, V, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, std::multimap<K, V, Compare, Alloc>& v) const {
        if (o.type != msghandle::type::MAP) { throw msghandle::type_error(); }
        msghandle::object_kv* p(o.via.map.ptr);
        msghandle::object_kv* const pend(o.via.map.ptr + o.via.map.size);
        std::multimap<K, V, Compare, Alloc> tmp;
        for (; p != pend; ++p) {
            std::pair<K, V> value;
            p->key.convert(value.first);
            p->val.convert(value.second);
#if MSGHANDLE_CPP_VERSION >= 201103L
            tmp.insert(std::move(value));
#else
            tmp.insert(value);
#endif
        }
#if MSGHANDLE_CPP_VERSION >= 201103L
        v = std::move(tmp);
#else
        tmp.swap(v);
#endif
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct pack<std::multimap<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multimap<K, V, Compare, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_map(size);
        for (typename std::multimap<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(it->first);
            o.pack(it->second);
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct print<std::multimap<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multimap<K, V, Compare, Alloc>& v,const std::string& k) const {

        int index =0;
        o.stream()<< k << "<std::multimap>"<<":\n";
        for (typename std::multimap<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(*it->first,key);
            o.print(*it->second,key);
            index++;
        }
        return o;

    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct checkcode<std::multimap<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multimap<K, V, Compare, Alloc>& v,const std::string& k) const {

        o.stream()<< k <<"std::map\n";
        std::string key = " "+k;
        if(v.size()==0)
        {
            K Kit;
            o.checkcode(Kit,key);
            V Vlu;
            o.checkcode(Vlu,key);
        }
        else
        {
            o.checkcode(*v.begin()->first,key);
            o.checkcode(*v.begin()->second,key);
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct reflectget<std::multimap<K, V, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multimap<K, V, Compare, Alloc>& v,const std::string& k, const std::string filter) const {

        int index =0;
        for (typename std::multimap<K, V, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = "["+std::to_string(index) + "]";
            std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
            std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
            if(result1 == "first")
                o.reflectget(*it->first,key,result2);
            else if(result1 == "second")
                o.reflectget(*it->second,key,result2);
            index++;
        }
        return o;
    }
};

template <typename K, typename V, typename Compare, typename Alloc>
struct object_with_zone<std::multimap<K, V, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const std::multimap<K, V, Compare, Alloc>& v) const {
        o.type = msghandle::type::MAP;
        if (v.empty()) {
            o.via.map.ptr  = MSGHANDLE_NULLPTR;
            o.via.map.size = 0;
        }
        else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object_kv* p = static_cast<msghandle::object_kv*>(o.zone.allocate_align(sizeof(msghandle::object_kv)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv)));
            msghandle::object_kv* const pend = p + size;
            o.via.map.ptr  = p;
            o.via.map.size = size;
            typename std::multimap<K, V, Compare, Alloc>::const_iterator it(v.begin());
            do {
                p->key = msghandle::object(it->first, o.zone);
                p->val = msghandle::object(it->second, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_MAP_HPP
