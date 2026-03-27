//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2014 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_OBJECT_FWD_HPP
#define MSGHANDLE_V1_OBJECT_FWD_HPP

#include "msghandle/object_fwd_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

struct object_array {
    uint32_t size;
    msghandle::object* ptr;
};

struct object_map {
    uint32_t size;
    msghandle::object_kv* ptr;
};

struct object_str {
    uint32_t size;
    const char* ptr;
};

struct object_bin {
    uint32_t size;
    const char* ptr;
};

struct object_ext {
    int8_t type() const { return static_cast<int8_t>(ptr[0]); }
    const char* data() const { return &ptr[1]; }
    uint32_t size;
    const char* ptr;
};


#if !defined(MSGHANDLE_USE_CPP03)

template <typename T>
struct has_as {
private:
    template <typename U>
    static auto check_(U*) ->
        // Check v1 specialization
        typename std::is_same<
            decltype(adaptor::as<U>()(std::declval<msghandle::object>())),
            T
        >::type;
    template <typename...>
    static std::false_type check_(...);
public:
    using type = decltype(check_<T>(MSGHANDLE_NULLPTR));
    static constexpr bool value = type::value;
};

#endif // !defined(MSGHANDLE_USE_CPP03)

/// Object class that corresponding to MessagePack format object
/**
 * See https://github.com/msghandle/msghandle-c/wiki/v1_1_cpp_object
 */
struct object {
    union union_type {
        bool boolean;
        uint64_t u64;
        int64_t  i64;
#if defined(MSGHANDLE_USE_LEGACY_NAME_AS_FLOAT)
        MSGHANDLE_DEPRECATED("please use f64 instead")
        double   dec; // obsolete
#endif // MSGHANDLE_USE_LEGACY_NAME_AS_FLOAT
        double   f64;
        msghandle::object_array array;
        msghandle::object_map map;
        msghandle::object_str str;
        msghandle::object_bin bin;
        msghandle::object_ext ext;
    };

    msghandle::type::object_type type;
    union_type via;

    /// Cheking nil
    /**
     * @return If the object is nil, then return true, else return false.
     */
    bool is_nil() const { return type == msghandle::type::NIL; }

#if defined(MSGHANDLE_USE_CPP03)

    /// Get value as T
    /**
     * If the object can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type you want to get.
     * @return The converted object.
     */
    template <typename T>
    T as() const;

#else  // defined(MSGHANDLE_USE_CPP03)

    /// Get value as T
    /**
     * If the object can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type you want to get.
     * @return The converted object.
     */
    template <typename T>
    typename std::enable_if<msghandle::has_as<T>::value, T>::type as() const;

    /// Get value as T
    /**
     * If the object can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type you want to get.
     * @return The converted object.
     */
    template <typename T>
    typename std::enable_if<!msghandle::has_as<T>::value, T>::type as() const;

#endif // defined(MSGHANDLE_USE_CPP03)

    /// Convert the object
    /**
     * If the object can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type of v.
     * @param v The value you want to get. `v` is output parameter. `v` is overwritten by converted value from the object.
     * @return The reference of `v`.
     */
    template <typename T>
    typename msghandle::enable_if<
        !msghandle::is_array<T>::value && !msghandle::is_pointer<T>::value,
        T&
    >::type
    convert(T& v) const;

    template <typename T, std::size_t N>
    T (&convert(T(&v)[N]) const)[N];


#if !defined(MSGHANDLE_DISABLE_LEGACY_CONVERT)
    /// Convert the object (obsolete)
    /**
     * If the object can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type of v.
     * @param v The pointer of the value you want to get. `v` is output parameter. `*v` is overwritten by converted value from the object.
     * @return The pointer of `v`.
     */
    template <typename T>
    MSGHANDLE_DEPRECATED("please use reference version instead")
    typename msghandle::enable_if<
        msghandle::is_pointer<T>::value,
        T
    >::type
    convert(T v) const;
#endif // !defined(MSGHANDLE_DISABLE_LEGACY_CONVERT)

    /// Convert the object if not nil
    /**
     * If the object is not nil and can't be converted to T, msghandle::type_error would be thrown.
     * @tparam T The type of v.
     * @param v The value you want to get. `v` is output parameter. `v` is overwritten by converted value from the object if the object is not nil.
     * @return If the object is nil, then return false, else return true.
     */
    template <typename T>
    bool convert_if_not_nil(T& v) const;

    /// Default constructor. The object is set to nil.
    object();

    /// Construct object from T
    /**
     * If `v` is the type that is corresponding to MessegePack format str, bin, ext, array, or map,
     * you need to call `object(const T& v, msghandle::zone& z)` instead of this constructor.
     *
     * @tparam T The type of `v`.
     * @param v The value you want to convert.
     */
    template <typename T>
    explicit object(const T& v);

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
    object(const T& v, msghandle::zone& z);

    /// Construct object from T (obsolete)
    /**
     * The object is constructed on the zone `z`.
     * Use `object(const T& v, msghandle::zone& z)` instead of this constructor.
     * See https://github.com/msghandle/msghandle-c/wiki/v1_1_cpp_object
     *
     * @tparam T The type of `v`.
     * @param v The value you want to convert.
     * @param z The pointer to the zone that is used by the object.
     */
    template <typename T>
    MSGHANDLE_DEPRECATED("please use zone reference version instead of the pointer version")
    object(const T& v, msghandle::zone* z);

    template <typename T>
    object& operator=(const T& v);

    struct with_zone;

protected:
    struct implicit_type;

public:
    implicit_type convert() const;
};

class type_error : public std::bad_cast { };

struct object::implicit_type {
    implicit_type(object const& o) : obj(o) { }
    ~implicit_type() { }

    template <typename T>
    operator T();

private:
    object const& obj;
};

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_OBJECT_FWD_HPP
