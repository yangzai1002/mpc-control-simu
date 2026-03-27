//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_NIL_HPP
#define MSGHANDLE_V1_TYPE_NIL_HPP

#include "msghandle/v1/adaptor/nil_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

struct nil_t { };

inline bool operator<(nil_t const& lhs, nil_t const& rhs) {
    return &lhs < &rhs;
}

inline bool operator==(nil_t const& lhs, nil_t const& rhs) {
    return &lhs == &rhs;
}

}  // namespace type

namespace adaptor {

template <>
struct convert<type::nil_t> {
    msghandle::object const& operator()(msghandle::object const& o, type::nil_t&) const {
        if(o.type != msghandle::type::NIL) { throw msghandle::type_error(); }
        return o;
    }
};

template <>
struct pack<type::nil_t> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const type::nil_t&,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        o.pack_nil();
        return o;
    }
};

template <>
struct object<type::nil_t> {
    void operator()(msghandle::object& o, type::nil_t) const {
        o.type = msghandle::type::NIL;
    }
};

template <>
struct object_with_zone<type::nil_t> {
    void operator()(msghandle::object::with_zone& o, type::nil_t v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_NIL_HPP
