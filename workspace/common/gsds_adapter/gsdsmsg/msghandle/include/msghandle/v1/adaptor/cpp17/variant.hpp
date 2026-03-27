//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2023 Uy Ha
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_TYPE_VARIANT_HPP
#define MSGHANDLE_V1_TYPE_VARIANT_HPP

#if defined(MSGHANDLE_USE_STD_VARIANT_ADAPTOR)

#include "msghandle/cpp_version.hpp"

#if MSGHANDLE_CPP_VERSION >= 201703

#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/versioning.hpp"

#include <variant>

namespace msghandle {
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
namespace adaptor {
namespace detail {
template <
    typename Variant,
    typename T,
    typename... Ts,
    std::size_t current_index,
    std::size_t... indices
>
Variant construct_variant(
    std::size_t index,
    msghandle::object& object,
    std::index_sequence<current_index, indices...>
) {
    if constexpr(sizeof...(Ts) == 0) {
        return object.as<T>();
    }
    else {
        if (index == current_index) {
            return object.as<T>();
        }
        return construct_variant<Variant, Ts...>(
            index,
            object,
            std::index_sequence<indices...>()
        );
    }
}

struct object_variant_overload {
    object_variant_overload(msghandle::object& obj, msghandle::zone& zone)
        : obj{obj}
        , zone{zone} {}

    template<typename T>
    void operator()(T const& value) {
        obj = msghandle::object(value, zone);
    }

    msghandle::object& obj;
    msghandle::zone& zone;
};
} // namespace detail

template <typename... Ts>
struct as<std::variant<Ts...>, typename std::enable_if<(msghandle::has_as<Ts>::value && ...)>::type> {
    std::variant<Ts...> operator()(msghandle::object const& o) const {
        if (  o.type != msghandle::type::ARRAY
           || o.via.array.size != 2
           || o.via.array.ptr[0].type != msghandle::type::POSITIVE_INTEGER
           || o.via.array.ptr[0].via.u64 >= sizeof...(Ts)) {
            throw msghandle::type_error{};
        }

        return detail::construct_variant<std::variant<Ts...>, Ts...>(
            o.via.array.ptr[0].as<std::size_t>(),
            o.via.array.ptr[1],
            std::make_index_sequence<sizeof...(Ts)>()
        );
    }
};

template<typename... Ts>
struct convert<std::variant<Ts...>> {
    msghandle::object const& operator()(msghandle::object const& o, std::variant<Ts...>& v) const {
        if (  o.type != msghandle::type::ARRAY
           || o.via.array.size != 2
           || o.via.array.ptr[0].type != msghandle::type::POSITIVE_INTEGER
           || o.via.array.ptr[0].via.u64 >= sizeof...(Ts)) {
            throw msghandle::type_error{};
        }

        v =  detail::construct_variant<std::variant<Ts...>, Ts...>(
            o.via.array.ptr[0].as<std::size_t>(),
            o.via.array.ptr[1],
            std::make_index_sequence<sizeof...(Ts)>()
        );
        return o;
    }
};

template <typename... Ts>
struct pack<std::variant<Ts...>>{
    template<typename Stream>
    msghandle::packer<Stream>& operator()(
        msghandle::packer<Stream>& o,
        std::variant<Ts...> const& v
    ) const {
        o.pack_array(2);
        o.pack_uint64(v.index());
        std::visit([&o](auto const& value){o.pack(value);}, v);
        return o;
    }
};


template<typename... Ts>
struct object_with_zone<std::variant<Ts...>> {
    void operator()(
        msghandle::object::with_zone& o,
        std::variant<Ts...> const& v
    ) const {
        msghandle::object *p =
            static_cast<msghandle::object *>(
                o.zone.allocate_align(
                    sizeof(msghandle::object) * 2,
                    MSGHANDLE_ZONE_ALIGNOF(msghandle::object)
                )
            );

        o.type = msghandle::type::ARRAY;
        o.via.array.size = 2;
        o.via.array.ptr = p;
        o.via.array.ptr[0]= msghandle::object(v.index(), o.zone);
        std::visit(detail::object_variant_overload(o.via.array.ptr[1], o.zone), v);
    }
};
} // namespace adaptor
}
} // namespace msghandle

#endif // MSGHANDLE_CPP_VERSION >= 201703
#endif // defined(MSGHANDLE_USE_STD_VARIANT_ADAPTOR)
#endif // MSGHANDLE_V1_TYPE_VARIANT_HPP
