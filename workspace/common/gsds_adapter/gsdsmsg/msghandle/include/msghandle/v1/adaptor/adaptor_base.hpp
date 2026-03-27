//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_ADAPTOR_BASE_HPP
#define MSGHANDLE_V1_ADAPTOR_BASE_HPP

#include "msghandle/v1/adaptor/adaptor_base_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond


namespace adaptor {

// Adaptor functors

template <typename T, typename Enabler>
struct convert {
    msghandle::object const& operator()(msghandle::object const& o, T& v) const;
};

template <typename T, typename Enabler>
struct print {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, T const& v) const;

    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, T const& v,const std::string& k) const;
};

template <typename T, typename Enabler>
struct checkcode {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, T const& v,const std::string& k) const;
};

template <typename T, typename Enabler>
struct reflectget {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, T const& v,const std::string& k, const std::string filter) const;
};

template <typename T, typename Enabler>
struct pack {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, T const& v,SMsgHandleInfo& msghandle_info) const;
};

template <typename T, typename Enabler>
struct object {
    void operator()(msghandle::object& o, T const& v) const;
};

template <typename T, typename Enabler>
struct object_with_zone {
    void operator()(msghandle::object::with_zone& o, T const& v) const;
};

} // namespace adaptor

// operators

template <typename T>
inline
typename msghandle::enable_if<
    !is_array<T>::value,
    msghandle::object const&
>::type
operator>> (msghandle::object const& o, T& v) {
    return msghandle::adaptor::convert<T>()(o, v);
}
template <typename T, std::size_t N>
inline
msghandle::object const& operator>> (msghandle::object const& o, T(&v)[N]) {
    return msghandle::adaptor::convert<T[N]>()(o, v);
}

template <typename Stream, typename T>
inline
typename msghandle::enable_if<
    !is_array<T>::value,
    msghandle::packer<Stream>&
>::type
operator<< (msghandle::packer<Stream>& o, T const& v) {
    SMsgHandleInfo msghandle_info;
    return msghandle::adaptor::pack<T>()(o, v,msghandle_info);
}

template <typename Stream, typename T>
inline
msghandle::packer<Stream>& packfunc(msghandle::packer<Stream>& o, T const& v,SMsgHandleInfo& msghandle_info)
{
    return msghandle::adaptor::pack<T>()(o, v,msghandle_info);
//return msghandle::adaptor::print<T>()(o, v);
}

template <typename Stream, typename T>
inline
msghandle::packer<Stream>& operatorfunc(msghandle::packer<Stream>& o, T const& v,const std::string& k)
{
    return msghandle::v1::adaptor::print<T>()(o, v,k);
//return msghandle::adaptor::print<T>()(o, v);
}

template <typename Stream, typename T>
inline
msghandle::packer<Stream>& operatorcheckcode(msghandle::packer<Stream>& o, T const& v,const std::string& k)
{
    return msghandle::v1::adaptor::checkcode<T>()(o, v,k);
    //return msghandle::adaptor::print<T>()(o, v);
}


template <typename Stream, typename T>
inline
msghandle::packer<Stream>& operatorreflectget(msghandle::packer<Stream>& o, T const& v,const std::string& k, const std::string filter)
{
    return msghandle::v1::adaptor::reflectget<T>()(o, v,k,filter);
    //return msghandle::adaptor::print<T>()(o, v);
}

//template <typename Stream, typename T>
//inline
//    typename msghandle::enable_if<
//        !is_array<T>::value,
//        msghandle::packer<Stream>&
//        >::type
//    operator<< (msghandle::packer<Stream>& o, T const& v) {
//    return msghandle::adaptor::pack<T>()(o, v);
//}

template <typename Stream, typename T, std::size_t N>
inline
msghandle::packer<Stream>& operator<< (msghandle::packer<Stream>& o, const T(&v)[N]) {
    return msghandle::adaptor::pack<T[N]>()(o, v);
}

template <typename T>
inline
typename msghandle::enable_if<
    !is_array<T>::value
>::type
operator<< (msghandle::object& o, T const& v) {
    msghandle::adaptor::object<T>()(o, v);
}
template <typename T, std::size_t N>
inline
void operator<< (msghandle::v1::object& o, const T(&v)[N]) {
    msghandle::v1::adaptor::object<T[N]>()(o, v);
}

template <typename T>
inline
typename msghandle::enable_if<
    !is_array<T>::value
>::type
operator<< (msghandle::object::with_zone& o, T const& v) {
    msghandle::adaptor::object_with_zone<T>()(o, v);
}
template <typename T, std::size_t N>
inline
void operator<< (msghandle::object::with_zone& o, const T(&v)[N]) {
    msghandle::adaptor::object_with_zone<T[N]>()(o, v);
}

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle


#endif // MSGHANDLE_V1_ADAPTOR_BASE_HPP
