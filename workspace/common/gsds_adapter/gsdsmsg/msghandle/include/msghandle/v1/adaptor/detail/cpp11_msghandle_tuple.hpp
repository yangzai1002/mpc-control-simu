//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_CPP11_MSGHANDLE_TUPLE_HPP
#define MSGHANDLE_V1_CPP11_MSGHANDLE_TUPLE_HPP

#include "msghandle/v1/adaptor/detail/cpp11_msghandle_tuple_decl.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/pack.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

template <class... Args>
inline tuple<Args...> make_tuple(Args&&... args) {
    return tuple<Args...>(std::forward<Args>(args)...);
}

template<class... Args>
inline tuple<Args&&...> forward_as_tuple (Args&&... args) noexcept {
    return tuple<Args&&...>(std::forward<Args>(args)...);
}

template <class... Tuples>
inline auto tuple_cat(Tuples&&... args) ->
    decltype(
        std::tuple_cat(std::forward<typename std::remove_reference<Tuples>::type::base>(args)...)
    ) {
    return std::tuple_cat(std::forward<typename std::remove_reference<Tuples>::type::base>(args)...);
}

template <class... Args>
inline tuple<Args&...> tie(Args&... args) {
    return tuple<Args&...>(args...);
}
} // namespace type

// --- Pack from tuple to packer stream ---
template <typename Stream, typename Tuple, std::size_t N>
struct MsgpackTuplePacker {
    static void pack(
        msghandle::packer<Stream>& o,
        const Tuple& v) {
        MsgpackTuplePacker<Stream, Tuple, N-1>::pack(o, v);
        o.pack(v.template get<N-1>());
    }
};

template <typename Stream, typename Tuple>
struct MsgpackTuplePacker<Stream, Tuple, 1> {
    static void pack (
        msghandle::packer<Stream>& o,
        const Tuple& v) {
        o.pack(v.template get<0>());
    }
};

template <typename Stream, typename Tuple>
struct MsgpackTuplePacker<Stream, Tuple, 0> {
    static void pack (
        msghandle::packer<Stream>&,
        const Tuple&) {
    }
};

namespace adaptor {

template <typename... Args>
struct pack<msghandle::type::tuple<Args...>> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(
        msghandle::packer<Stream>& o,
        const msghandle::type::tuple<Args...>& v) const {
        o.pack_array(sizeof...(Args));
        MsgpackTuplePacker<Stream, decltype(v), sizeof...(Args)>::pack(o, v);
        return o;
    }
};

} // namespace adaptor

// --- Convert from tuple to object ---

template <typename T, typename... Args>
struct MsgpackTupleAsImpl {
    static msghandle::type::tuple<T, Args...> as(msghandle::object const& o) {
        return msghandle::type::tuple_cat(
            msghandle::type::make_tuple(o.via.array.ptr[o.via.array.size - sizeof...(Args) - 1].as<T>()),
            MsgpackTupleAs<Args...>::as(o));
    }
};

template <typename... Args>
struct MsgpackTupleAs {
    static msghandle::type::tuple<Args...> as(msghandle::object const& o) {
        return MsgpackTupleAsImpl<Args...>::as(o);
    }
};

template <>
struct MsgpackTupleAs<> {
    static msghandle::type::tuple<> as (msghandle::object const&) {
        return msghandle::type::tuple<>();
    }
};

template <typename Tuple, std::size_t N>
struct MsgpackTupleConverter {
    static void convert(
        msghandle::object const& o,
        Tuple& v) {
        MsgpackTupleConverter<Tuple, N-1>::convert(o, v);
        if (o.via.array.size >= N)
            o.via.array.ptr[N-1].convert<typename std::remove_reference<decltype(v.template get<N-1>())>::type>(v.template get<N-1>());
    }
};

template <typename Tuple>
struct MsgpackTupleConverter<Tuple, 1> {
    static void convert (
        msghandle::object const& o,
        Tuple& v) {
        o.via.array.ptr[0].convert<typename std::remove_reference<decltype(v.template get<0>())>::type>(v.template get<0>());
    }
};

template <typename Tuple>
struct MsgpackTupleConverter<Tuple, 0> {
    static void convert (
        msghandle::object const&,
        Tuple&) {
    }
};

namespace adaptor {

template <typename... Args>
struct as<msghandle::type::tuple<Args...>, typename std::enable_if<msghandle::any_of<msghandle::has_as, Args...>::value>::type>  {
    msghandle::type::tuple<Args...> operator()(
        msghandle::object const& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        return MsgpackTupleAs<Args...>::as(o);
    }
};

template <typename... Args>
struct convert<msghandle::type::tuple<Args...>> {
    msghandle::object const& operator()(
        msghandle::object const& o,
        msghandle::type::tuple<Args...>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        MsgpackTupleConverter<decltype(v), sizeof...(Args)>::convert(o, v);
        return o;
    }
};

} // namespace adaptor

// --- Convert from tuple to object with zone ---
template <typename Tuple, std::size_t N>
struct MsgpackTupleToObjectWithZone {
    static void convert(
        msghandle::object::with_zone& o,
        const Tuple& v) {
        MsgpackTupleToObjectWithZone<Tuple, N-1>::convert(o, v);
        o.via.array.ptr[N-1] = msghandle::object(v.template get<N-1>(), o.zone);
    }
};

template <typename Tuple>
struct MsgpackTupleToObjectWithZone<Tuple, 1> {
    static void convert (
        msghandle::object::with_zone& o,
        const Tuple& v) {
        o.via.array.ptr[0] = msghandle::object(v.template get<0>(), o.zone);
    }
};

template <typename Tuple>
struct MsgpackTupleToObjectWithZone<Tuple, 0> {
    static void convert (
        msghandle::object::with_zone&,
        const Tuple&) {
    }
};

namespace adaptor {

template <typename... Args>
    struct object_with_zone<msghandle::type::tuple<Args...>> {
    void operator()(
        msghandle::object::with_zone& o,
        msghandle::type::tuple<Args...> const& v) const {
        o.type = msghandle::type::ARRAY;
        o.via.array.ptr = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*sizeof...(Args), MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        o.via.array.size = sizeof...(Args);
        MsgpackTupleToObjectWithZone<decltype(v), sizeof...(Args)>::convert(o, v);
    }
};

}  // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
///@endcond

}  // namespace msghandle

#endif // MSGHANDLE_CPP11_MSGHANDLE_TUPLE_HPP
