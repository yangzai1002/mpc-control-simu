//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_PAIR_HPP
#define MSGHANDLE_V1_TYPE_PAIR_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/meta.hpp"

#include <utility>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

template <typename T1, typename T2>
struct as<std::pair<T1, T2>,
          typename std::enable_if<msghandle::any_of<msghandle::has_as, T1, T2>::value>::type> {
    std::pair<T1, T2> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        if (o.via.array.size != 2) { throw msghandle::type_error(); }
        return std::make_pair(o.via.array.ptr[0].as<T1>(), o.via.array.ptr[1].as<T2>());
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename T1, typename T2>
struct convert<std::pair<T1, T2> > {
    msghandle::object const& operator()(msghandle::object const& o, std::pair<T1, T2>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        if(o.via.array.size != 2) { throw msghandle::type_error(); }
        o.via.array.ptr[0].convert(v.first);
        o.via.array.ptr[1].convert(v.second);
        return o;
    }
};

template <typename T1, typename T2>
struct pack<std::pair<T1, T2> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::pair<T1, T2>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        o.pack_array(2);
        o.pack(v.first);
        o.pack(v.second);
        return o;
    }
};

template <typename T1, typename T2>
struct print<std::pair<T1, T2> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::pair<T1, T2>& v,const std::string& k) const {
        o.stream()<< k <<"<std::pair>"<<":\n";
        std::string key = " "+k;
        o.print(v.first,key);
        o.print(v.second,key);
        return o;
    }
};


template <typename T1, typename T2>
struct checkcode<std::pair<T1, T2> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::pair<T1, T2>& v,const std::string& k) const {
        o.stream()<< k <<"std::pair\n";
        std::string key = " "+k;
        o.checkcode(v.first,key);
        o.checkcode(v.second,key);
        return o;
    }
};

template <typename T1, typename T2>
struct reflectget<std::pair<T1, T2> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::pair<T1, T2>& v,const std::string& k, const std::string filter) const {
        std::string key = k;
        std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
        std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
        if(result1 == "first")
            o.reflectget(v.first,result2);
        if(result1 == "second")
            o.reflectget(v.second,result2);
        return o;
    }
};

template <typename T1, typename T2>
struct object_with_zone<std::pair<T1, T2> > {
    void operator()(msghandle::object::with_zone& o, const std::pair<T1, T2>& v) const {
        o.type = msghandle::type::ARRAY;
        msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*2, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        o.via.array.ptr = p;
        o.via.array.size = 2;
        p[0] = msghandle::object(v.first, o.zone);
        p[1] = msghandle::object(v.second, o.zone);
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_PAIR_HPP
