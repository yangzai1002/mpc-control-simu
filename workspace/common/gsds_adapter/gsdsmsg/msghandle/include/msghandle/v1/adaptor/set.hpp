//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_SET_HPP
#define MSGHANDLE_V1_TYPE_SET_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/cpp_version.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <set>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

template <typename T, typename Compare, typename Alloc>
struct as<std::set<T, Compare, Alloc>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::set<T, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::set<T, Compare, Alloc> v;
        while (p > pbegin) {
            --p;
            v.insert(p->as<T>());
        }
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename T, typename Compare, typename Alloc>
struct convert<std::set<T, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, std::set<T, Compare, Alloc>& v) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::set<T, Compare, Alloc> tmp;
        while (p > pbegin) {
            --p;
            tmp.insert(p->as<T>());
        }
#if MSGHANDLE_CPP_VERSION >= 201103L
        v = std::move(tmp);
#else
        tmp.swap(v);
#endif
        return o;
    }
};

template <typename T, typename Compare, typename Alloc>
struct pack<std::set<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::set<T, Compare, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for (typename std::set<T, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};
template <typename T, typename Compare, typename Alloc>
struct print<std::set<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::set<T, Compare, Alloc>& v,const std::string& k) const {
        int index =0;
        o.stream()<< k <<"<std::set>"<<":\n";
        for (typename std::set<T, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(*it,key);
            index++;
        }
        return o;
    }
};
template <typename T, typename Compare, typename Alloc>
struct checkcode<std::set<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::set<T, Compare, Alloc>& v,const std::string& k) const {
        o.stream()<< k <<"std::set\n";
        std::string key = " "+k;
        if(v.size()==0)
        {
            T it;
            o.checkcode(it,key);
        }
        else
        {
            o.checkcode(*v.begin(),key);
        }
        return o;
    }
};
template <typename T, typename Compare, typename Alloc>
struct reflectget<std::set<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::set<T, Compare, Alloc>& v,const std::string& k, const std::string filter) const {
        int index =0;
        for (typename std::set<T, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
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

template <typename T, typename Compare, typename Alloc>
struct object_with_zone<std::set<T, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const std::set<T, Compare, Alloc>& v) const {
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
            typename std::set<T, Compare, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(*it, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

#if !defined(MSGHANDLE_USE_CPP03)

template <typename T, typename Compare, typename Alloc>
struct as<std::multiset<T, Compare, Alloc>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::multiset<T, Compare, Alloc> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::multiset<T, Compare, Alloc> v;
        while (p > pbegin) {
            --p;
            v.insert(p->as<T>());
        }
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename T, typename Compare, typename Alloc>
struct convert<std::multiset<T, Compare, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, std::multiset<T, Compare, Alloc>& v) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        msghandle::object* p = o.via.array.ptr + o.via.array.size;
        msghandle::object* const pbegin = o.via.array.ptr;
        std::multiset<T, Compare, Alloc> tmp;
        while (p > pbegin) {
            --p;
            tmp.insert(p->as<T>());
        }
#if MSGHANDLE_CPP_VERSION >= 201103L
        v = std::move(tmp);
#else
        tmp.swap(v);
#endif
        return o;
    }
};

template <typename T, typename Compare, typename Alloc>
struct pack<std::multiset<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multiset<T, Compare, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for (typename std::multiset<T, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};
template <typename T, typename Compare, typename Alloc>
struct print<std::multiset<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multiset<T, Compare, Alloc>& v,const std::string& k) const {
        int index =0;
        o.stream()<< k <<"<std::multiset>"<<":\n";
        for (typename std::set<T, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(*it,key);
            index++;
        }
        return o;
    }
};
template <typename T, typename Compare, typename Alloc>
struct checkcode<std::multiset<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multiset<T, Compare, Alloc>& v,const std::string& k) const {
        o.stream()<< k <<"std::multiset\n";
        std::string key = " "+k;
        if(v.size()==0)
        {
            T it;
            o.checkcode(it,key);
        }
        else
        {
            o.checkcode(*v.begin(),key);
        }
        return o;
    }
};
template <typename T, typename Compare, typename Alloc>
struct reflectget<std::multiset<T, Compare, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::multiset<T, Compare, Alloc>& v,const std::string& k, const std::string filter) const {
        int index =0;
        for (typename std::multiset<T, Compare, Alloc>::const_iterator it(v.begin()), it_end(v.end());
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


template <typename T, typename Compare, typename Alloc>
struct object_with_zone<std::multiset<T, Compare, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const std::multiset<T, Compare, Alloc>& v) const {
        o.type = msghandle::type::ARRAY;
        if (v.empty()) {
            o.via.array.ptr = MSGHANDLE_NULLPTR;
            o.via.array.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            msghandle::object* const pend = p + size;
            o.via.array.ptr = p;
            o.via.array.size = size;
            typename std::multiset<T, Compare, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(*it, o.zone);
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

#endif // MSGHANDLE_V1_TYPE_SET_HPP
