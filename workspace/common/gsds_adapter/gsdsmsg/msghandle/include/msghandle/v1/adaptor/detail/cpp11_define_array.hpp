//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_CPP11_DEFINE_ARRAY_HPP
#define MSGHANDLE_V1_CPP11_DEFINE_ARRAY_HPP

#include "msghandle/v1/adaptor/detail/cpp11_define_array_decl.hpp"
#include "msghandle/v1/adaptor/detail/cpp11_convert_helper.hpp"

#include <tuple>

namespace msghandle {
/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond
namespace type {

template <typename Tuple, std::size_t N>
struct define_array_imp {
    template <typename Packer>
    static void pack(Packer& pk, Tuple const& t,SMsgHandleInfo& msghandle_info) {
        define_array_imp<Tuple, N-1>::pack(pk, t,msghandle_info);
        pk.pack(std::get<N-1>(t),msghandle_info);
    }

    template <typename Packer>
    static void print(Packer& pk, Tuple const& t,std::vector<std::string>& nm) {
        define_array_imp<Tuple, N-1>::print(pk, t,nm);
        std::string key = nm[N-1];
        pk.print(std::get<N-1>(t),key);
    }

    template <typename Packer>
    static void checkcode(Packer& pk, Tuple const& t,std::vector<std::string>& nm) {
        define_array_imp<Tuple, N-1>::checkcode(pk, t,nm);
        std::string key = nm[N-1];
        pk.checkcode(std::get<N-1>(t),key);
    }

    template <typename Packer>
    static void reflectget(Packer& pk, Tuple const& t,std::vector<std::string>& nm, const std::string filter) {
        define_array_imp<Tuple, N-1>::reflectget(pk, t,nm,filter);
        std::string key = nm[N-1];

        pk.reflectget(std::get<N-1>(t),key,filter);
    }

    static void unpack(msghandle::object const& o, Tuple& t) {
        define_array_imp<Tuple, N-1>::unpack(o, t);
        const size_t size = o.via.array.size;
        if(size <= N-1) { return; }
        convert_helper(o.via.array.ptr[N-1], std::get<N-1>(t));
    }

    static void unpack(msghandle::object const& o, Tuple& t, std::map<int,int>& index_map) {
        define_array_imp<Tuple, N-1>::unpack(o, t,index_map);
        const size_t size = index_map.size();//o.via.array.size;
        if(size <= N-1) { return; }
//        if( (mask & (1 << (N-1))) == 0) { return; }
        if( index_map[static_cast<int>(N-1)] == -1) { return; }
        int index = index_map[static_cast<int>(N-1)];
        convert_helper(o.via.array.ptr[index], std::get<N-1>(t));
    }
    static void object(msghandle::object* o, msghandle::zone& z, Tuple const& t) {
        define_array_imp<Tuple, N-1>::object(o, z, t);
        o->via.array.ptr[N-1] = msghandle::object(std::get<N-1>(t), z);
    }
};

template <typename Tuple>
struct define_array_imp<Tuple, 1> {
    template <typename Packer>
    static void pack(Packer& pk, Tuple const& t,SMsgHandleInfo& msghandle_info) {
        pk.pack(std::get<0>(t),msghandle_info);
    }
    template <typename Packer>
    static void print(Packer& pk, Tuple const& t,std::vector<std::string>& nm) {
        std::string key = nm[0];
        pk.print(std::get<0>(t),key);
    }

    template <typename Packer>
    static void checkcode(Packer& pk, Tuple const& t,std::vector<std::string>& nm) {
        std::string key = nm[0];
        pk.checkcode(std::get<0>(t),key);
    }

    template <typename Packer>
    static void reflectget(Packer& pk, Tuple const& t,std::vector<std::string>& nm, const std::string filter) {
        std::string key = nm[0];
        pk.reflectget(std::get<0>(t),key,filter);
    }

    static void unpack(msghandle::object const& o, Tuple& t, std::map<int,int>& index_map) {
        const size_t size = o.via.array.size;
        if(size <= 0) { return; }
        convert_helper(o.via.array.ptr[0], std::get<0>(t));
    }
    static void unpack(msghandle::object const& o, Tuple& t) {
        const size_t size = o.via.array.size;
        if(size <= 0) { return; }
        convert_helper(o.via.array.ptr[0], std::get<0>(t));
    }
    static void object(msghandle::object* o, msghandle::zone& z, Tuple const& t) {
        o->via.array.ptr[0] = msghandle::object(std::get<0>(t), z);
    }
};

template <typename... Args>
struct define_array {
    typedef define_array<Args...> value_type;
    typedef std::tuple<Args...> tuple_type;
    define_array(Args&... args) :
        a(args...) {}
    template <typename Packer>
    void msghandle_pack(Packer& pk,SMsgHandleInfo& msghandle_info) const
    {
        pk.pack_array(sizeof...(Args));

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::pack(pk, a,msghandle_info);
    }
    template <typename Packer>
    void msghandle_print(Packer& pk,std::vector<std::string>& nm) const
    {
//        pk.pack_array(sizeof...(Args));

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::print(pk, a,nm);
    }

    template <typename Packer>
    void msghandle_checkcode(Packer& pk,std::vector<std::string>& nm) const
    {
        //        pk.pack_array(sizeof...(Args));

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::checkcode(pk, a,nm);
    }

    template <typename Packer>
    void msghandle_reflectget(Packer& pk,std::vector<std::string>& nm, const std::string filter) const
    {
        //        pk.pack_array(sizeof...(Args));

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::reflectget(pk, a,nm,filter);
    }

    void msghandle_unpack(msghandle::object const& o, std::map<int,int>& index_map)
    {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::unpack(o, a,index_map);
    }
    void msghandle_unpack(msghandle::object const& o)
    {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::unpack(o, a);
    }
    void msghandle_object(msghandle::object* o, msghandle::zone& z) const
    {
        o->type = msghandle::type::ARRAY;
        o->via.array.ptr = static_cast<msghandle::object*>(z.allocate_align(sizeof(msghandle::object)*sizeof...(Args), MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        o->via.array.size = sizeof...(Args);

        define_array_imp<std::tuple<Args&...>, sizeof...(Args)>::object(o, z, a);
    }

    std::tuple<Args&...> a;

};

template <>
struct define_array<> {
    typedef define_array<> value_type;
    typedef std::tuple<> tuple_type;
    template <typename Packer>
    void msghandle_pack(Packer& pk) const
    {
        pk.pack_array(0);
    }
    void msghandle_unpack(msghandle::object const& o)
    {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
    }
    void msghandle_object(msghandle::object* o, msghandle::zone&) const
    {
        o->type = msghandle::type::ARRAY;
        o->via.array.ptr = NULL;
        o->via.array.size = 0;
    }
};

inline define_array<> make_define_array()
{
    return define_array<>();
}

template <typename... Args>
inline define_array<Args...> make_define_array(Args&... args)
{
    return define_array<Args...>(args...);
}

}  // namespace type
/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond
}  // namespace msghandle

#endif // MSGHANDLE_V1_CPP11_DEFINE_ARRAY_HPP
