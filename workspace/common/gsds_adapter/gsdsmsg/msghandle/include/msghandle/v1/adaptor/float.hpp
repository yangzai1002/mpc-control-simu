//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_FLOAT_HPP
#define MSGHANDLE_V1_TYPE_FLOAT_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/object_fwd.hpp"
#include <vector>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

// FIXME check overflow, underflow

namespace adaptor {

template <>
struct convert<float> {
    msghandle::object const& operator()(msghandle::object const& o, float& v) const {
        if(o.type == msghandle::type::FLOAT32 || o.type == msghandle::type::FLOAT64) {
            v = static_cast<float>(o.via.f64);
        }
        else if (o.type == msghandle::type::POSITIVE_INTEGER) {
            v = static_cast<float>(o.via.u64);
        }
        else if (o.type == msghandle::type::NEGATIVE_INTEGER) {
            v = static_cast<float>(o.via.i64);
        }
        else {
            throw msghandle::type_error();
        }
        return o;
    }
};

template <>
struct pack<float> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const float& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=1;
            return o;}
        o.pack_float(v);
        return o;
    }
};

template <>
struct print<float> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const float& v,const std::string& k) const {
        o.stream() <<k<<"<float>"<<":"<< std::fixed<<v<<std::endl;
        return o;
    }
};

template <>
struct checkcode<float> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const float& v,const std::string& k) const {
        o.stream() <<k<<"float"<<std::endl;
        return o;
    }
};
template <>
struct reflectget<float> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const float& v,const std::string& k, const std::string filter) const {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct convert<double> {
    msghandle::object const& operator()(msghandle::object const& o, double& v) const {
        if(o.type == msghandle::type::FLOAT32 || o.type == msghandle::type::FLOAT64) {
            v = o.via.f64;
        }
        else if (o.type == msghandle::type::POSITIVE_INTEGER) {
            v = static_cast<double>(o.via.u64);
        }
        else if (o.type == msghandle::type::NEGATIVE_INTEGER) {
            v = static_cast<double>(o.via.i64);
        }
        else {
            throw msghandle::type_error();
        }
        return o;
    }
};

template <>
struct pack<double> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const double& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=1;
            return o;}
        o.pack_double(v);
        return o;
    }
};

template <>
struct print<double> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const double& v,const std::string& k) const {
        o.stream() <<k<<"<double>"<<":"<< std::fixed<<v<<std::endl;
        return o;
    }
};

template <>
struct checkcode<double> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const double& v,const std::string& k) const {
        o.stream() <<k<<"double"<<std::endl;
        return o;
    }
};

template <>
struct reflectget<double> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const double& v,const std::string& k, const std::string filter) const {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct object<float> {
    void operator()(msghandle::object& o, float v) const {
        o.type = msghandle::type::FLOAT32;
        o.via.f64 = static_cast<double>(v);
    }
};

template <>
struct object<double> {
    void operator()(msghandle::object& o, double v) const {
        o.type = msghandle::type::FLOAT64;
        o.via.f64 = v;
    }
};

template <>
struct object_with_zone<float> {
    void operator()(msghandle::object::with_zone& o, float v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

template <>
struct object_with_zone<double> {
    void operator()(msghandle::object::with_zone& o, double v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_FLOAT_HPP
