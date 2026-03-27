//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_BOOL_HPP
#define MSGHANDLE_V1_TYPE_BOOL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct convert<bool> {
    msghandle::object const& operator()(msghandle::object const& o, bool& v) const {
        if(o.type != msghandle::type::BOOLEAN) { throw msghandle::type_error(); }
        v = o.via.boolean;
        return o;
    }
};

template <>
struct pack<bool> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const bool& v,SMsgHandleInfo& msghandle_info) const {
        if(v) { o.pack_true(); }
        else { o.pack_false(); }
        return o;
    }
};
template <>
struct print<bool> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const bool& v,const std::string& k) const {
        o.stream() <<k<<"<bool>"<<":"<<v<<std::endl;
        return o;
    }
};
template <>
struct checkcode<bool> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const bool& v,const std::string& k) const {
        o.stream() <<k<<"bool"<<std::endl;
        return o;
    }
};

template <>
struct reflectget<bool> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const bool& v,const std::string& k, const std::string filter) const {
        o.stream() <<v<<std::endl;
        return o;
    }
};

template <>
struct object<bool> {
    void operator()(msghandle::object& o, bool v) const {
        o.type = msghandle::type::BOOLEAN;
        o.via.boolean = v;
    }
};

template <>
struct object_with_zone<bool> {
    void operator()(msghandle::object::with_zone& o, bool v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_BOOL_HPP
