//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_CPP11_TUPLE_HPP
#define MSGHANDLE_V1_TYPE_CPP11_TUPLE_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"
#include "msghandle/meta.hpp"

#include <tuple>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

// --- Pack from tuple to packer stream ---
template <typename Stream, typename Tuple, std::size_t N>
struct StdTuplePacker {
    static void pack(
        msghandle::packer<Stream>& o,
        const Tuple& v) {
        StdTuplePacker<Stream, Tuple, N-1>::pack(o, v);
        o.pack(std::get<N-1>(v));
    }
};

template <typename Stream, typename Tuple>
struct StdTuplePacker<Stream, Tuple, 0> {
    static void pack (
        msghandle::packer<Stream>&,
        const Tuple&) {
    }
};

namespace adaptor {

template <typename... Args>
struct pack<std::tuple<Args...>> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(
        msghandle::packer<Stream>& o,
        const std::tuple<Args...>& v) const {
        uint32_t size = checked_get_container_size(sizeof...(Args));
        o.pack_array(size);
        StdTuplePacker<Stream, decltype(v), sizeof...(Args)>::pack(o, v);
        return o;
    }
};

} // namespace adaptor

// --- Convert from tuple to object ---

template <typename... Args>
struct StdTupleAs;

template <typename T, typename... Args>
struct StdTupleAsImpl {
    static std::tuple<T, Args...> as(msghandle::object const& o) {
        return std::tuple_cat(
            std::make_tuple(o.via.array.ptr[o.via.array.size - sizeof...(Args) - 1].as<T>()),
            StdTupleAs<Args...>::as(o));
    }
};

template <typename... Args>
struct StdTupleAs {
    static std::tuple<Args...> as(msghandle::object const& o) {
        return StdTupleAsImpl<Args...>::as(o);
    }
};

template <>
struct StdTupleAs<> {
    static std::tuple<> as (msghandle::object const&) {
        return std::tuple<>();
    }
};

template <typename Tuple, std::size_t N>
struct StdTupleConverter {
    static void convert(
        msghandle::object const& o,
        Tuple& v) {
        StdTupleConverter<Tuple, N-1>::convert(o, v);
        if (o.via.array.size >= N)
            o.via.array.ptr[N-1].convert<typename std::remove_reference<decltype(std::get<N-1>(v))>::type>(std::get<N-1>(v));
    }
};

template <typename Tuple>
struct StdTupleConverter<Tuple, 0> {
    static void convert (
        msghandle::object const&,
        Tuple&) {
    }
};

namespace adaptor {

template <typename... Args>
struct as<std::tuple<Args...>, typename std::enable_if<msghandle::any_of<msghandle::has_as, Args...>::value>::type>  {
    std::tuple<Args...> operator()(
        msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        return StdTupleAs<Args...>::as(o);
    }
};

template <typename... Args>
struct convert<std::tuple<Args...>> {
    msghandle::object const& operator()(
        msghandle::object const& o,
        std::tuple<Args...>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        StdTupleConverter<decltype(v), sizeof...(Args)>::convert(o, v);
        return o;
    }
};

} // namespace adaptor

// --- Convert from tuple to object with zone ---
template <typename Tuple, std::size_t N>
struct StdTupleToObjectWithZone {
    static void convert(
        msghandle::object::with_zone& o,
        const Tuple& v) {
        StdTupleToObjectWithZone<Tuple, N-1>::convert(o, v);
        o.via.array.ptr[N-1] = msghandle::object(std::get<N-1>(v), o.zone);
    }
};

template <typename Tuple>
struct StdTupleToObjectWithZone<Tuple, 0> {
    static void convert (
        msghandle::object::with_zone&,
        const Tuple&) {
    }
};

namespace adaptor {

template <typename... Args>
struct object_with_zone<std::tuple<Args...>> {
    void operator()(
        msghandle::object::with_zone& o,
        std::tuple<Args...> const& v) const {
        uint32_t size = checked_get_container_size(sizeof...(Args));
        o.type = msghandle::type::ARRAY;
        o.via.array.ptr = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        o.via.array.size = size;
        StdTupleToObjectWithZone<decltype(v), sizeof...(Args)>::convert(o, v);
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_CPP11_TUPLE_HPP
