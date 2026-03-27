//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2020 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_TYPE_COMPLEX_HPP
#define MSGHANDLE_V1_TYPE_COMPLEX_HPP

#include <complex>
#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/meta.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

template <typename T>
struct as<std::complex<T>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::complex<T> operator()(msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY)
            throw msghandle::type_error();
        if (o.via.array.size != 2)
            throw msghandle::type_error();
        return std::complex<T>(o.via.array.ptr[0].as<T>(), o.via.array.ptr[1].as<T>());
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename T>
struct convert<std::complex<T> > {
    msghandle::object const& operator()(msghandle::object const& o, std::complex<T>& v) const {
        if(o.type != msghandle::type::ARRAY)
            throw msghandle::type_error();
        if(o.via.array.size != 2)
            throw msghandle::type_error();
        T real;
        T imag;
        o.via.array.ptr[0].convert(real);
        o.via.array.ptr[1].convert(imag);
        v.real(real);
        v.imag(imag);
        return o;
    }
};

template <typename T>
struct pack<std::complex<T> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, std::complex<T> const& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        o.pack_array(2);
        o.pack(v.real());
        o.pack(v.imag());
        return o;
    }
};

template <typename T>
struct print<std::complex<T> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, std::complex<T> const& v,const std::string& k) const {
        o.stream() <<k<<"_real:"<<v.real()<<std::endl;
        o.stream() <<k<<"_imag:"<<v.imag()<<std::endl;
        return o;
    }
};

template <typename T>
struct checkcode<std::complex<T> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, std::complex<T> const& v,const std::string& k) const {
        o.stream() <<k<<"std::complex"<<std::endl;
        return o;
    }
};

template <typename T>
struct reflectget<std::complex<T> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, std::complex<T> const& v,const std::string& k, const std::string filter) const {
        std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
        if(result1 == "real")
            o.stream() <<v.real()<<std::endl;
        else if(result1 == "imag")
            o.stream() <<v.imag()<<std::endl;
        return o;
    }
};

template <typename T>
struct object_with_zone<std::complex<T> > {
    void operator()(msghandle::object::with_zone& o, std::complex<T> const& v) const {
        o.type = msghandle::type::ARRAY;
        msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*2, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        o.via.array.ptr = p;
        o.via.array.size = 2;
        p[0] = msghandle::object(v.real(), o.zone);
        p[1] = msghandle::object(v.imag(), o.zone);
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle


#endif // MSGHANDLE_V1_TYPE_COMPLEX_HPP
