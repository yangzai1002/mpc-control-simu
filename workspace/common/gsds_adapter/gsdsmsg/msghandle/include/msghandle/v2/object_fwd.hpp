//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V2_OBJECT_FWD_HPP
#define MSGHANDLE_V2_OBJECT_FWD_HPP

#include "msghandle/v2/object_fwd_decl.hpp"
#include "msghandle/object_fwd.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

struct object : v1::object {
    object() {}
    object(v1::object const& o):v1::object(o) {}
    /// Construct object from T
    /**
     * If `v` is the type that is corresponding to MessegePack format str, bin, ext, array, or map,
     * you need to call `object(const T& v, msghandle::zone& z)` instead of this constructor.
     *
     * @tparam T The type of `v`.
     * @param v The value you want to convert.
     */
    template <typename T>
    explicit object(const T& v) {
        *this << v;
    }

    /// Construct object from T
    /**
     * The object is constructed on the zone `z`.
     * See https://github.com/msghandle/msghandle-c/wiki/v1_1_cpp_object
     *
     * @tparam T The type of `v`.
     * @param v The value you want to convert.
     * @param z The zone that is used by the object.
     */
    template <typename T>
    object(const T& v, msghandle::zone& z):v1::object(v, z) {}

public:
    /// Convert the object
    /**
     * If the object can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type of v.
     * @param v The value you want to get. `v` is output parameter. `v` is overwritten by converted value from the object.
     * @return The reference of `v`.
     */
    template <typename T>
    T& convert(T& v) const { return v1::object::convert(v); }

    using v1::object::with_zone;
    implicit_type convert() const;
};

#if !defined(MSGHANDLE_USE_CPP03)

namespace adaptor {

// If v1 has as specialization for T, then dispatch v1::adaptor::as<T>.
// So I call v1::has_as<T> meta function intentionally.
template <typename T>
struct as<T, typename std::enable_if<v1::has_as<T>::value>::type> : v1::adaptor::as<T> {
};

} // namespace adaptor

template <typename T>
struct has_as {
private:
    template <typename U>
    static auto check_(U*) ->
        typename std::enable_if<
            // check v2 specialization
            std::is_same<
                decltype(adaptor::as<U>()(std::declval<msghandle::object>())),
                U
            >::value
            ||
            // check v1 specialization
            v1::has_as<U>::value,
            std::true_type
        >::type;
    template <typename...>
    static std::false_type check_(...);
public:
    using type = decltype(check_<T>(MSGHANDLE_NULLPTR));
    static constexpr bool value = type::value;
};

#endif // !defined(MSGHANDLE_USE_CPP03)

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V2_OBJECT_FWD_HPP
