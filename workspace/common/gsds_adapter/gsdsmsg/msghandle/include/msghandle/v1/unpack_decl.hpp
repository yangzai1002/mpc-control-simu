//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_UNPACK_DECL_HPP
#define MSGHANDLE_V1_UNPACK_DECL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/unpack_define.hpp"
#include "msghandle/object.hpp"
#include "msghandle/zone.hpp"
#include "msghandle/cpp_config.hpp"
#include "msghandle/sysdep.hpp"
#include "msghandle/parse_return.hpp"

#include <memory>
#include <stdexcept>

#if !defined(MSGHANDLE_USE_CPP03)
#include <atomic>
#endif


#if defined(_MSC_VER)
// avoiding confliction std::max, std::min, and macro in windows.h
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif // defined(_MSC_VER)

#ifdef _msghandle_atomic_counter_header
#include _msghandle_atomic_counter_header
#endif

const size_t YF_COUNTER_SIZE = sizeof(_msghandle_atomic_counter_t);

#ifndef MSGHANDLE_UNPACKER_INIT_BUFFER_SIZE
#define MSGHANDLE_UNPACKER_INIT_BUFFER_SIZE (64*1024)
#endif

#ifndef MSGHANDLE_UNPACKER_RESERVE_SIZE
#define MSGHANDLE_UNPACKER_RESERVE_SIZE (32*1024)
#endif


// backward compatibility
#ifndef MSGHANDLE_UNPACKER_DEFAULT_INITIAL_BUFFER_SIZE
#define MSGHANDLE_UNPACKER_DEFAULT_INITIAL_BUFFER_SIZE MSGHANDLE_UNPACKER_INIT_BUFFER_SIZE
#endif


namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

/// The type of reference or copy judging function.
/**
 * @param type msghandle data type.
 * @param size msghandle data size.
 * @param user_data The user_data that is set by msghandle::unpack functions.
 *
 * @return If the data should be referenced, then return true, otherwise (should be copied) false.
 *
 * This function is called when unpacking STR, BIN, or EXT.
 *
 */
typedef bool (*unpack_reference_func)(msghandle::type::object_type type, std::size_t size, void* user_data);

struct unpack_error;
struct parse_error;
struct insufficient_bytes;
struct size_overflow;
struct array_size_overflow;
struct map_size_overflow;
struct str_size_overflow;
struct bin_size_overflow;
struct ext_size_overflow;
struct depth_size_overflow;

class unpack_limit {
public:
    unpack_limit(
        std::size_t array = 0xffffffff,
        std::size_t map = 0xffffffff,
        std::size_t str = 0xffffffff,
        std::size_t bin = 0xffffffff,
        std::size_t ext = 0xffffffff,
        std::size_t depth = 0xffffffff)
        :array_(array),
         map_(map),
         str_(str),
         bin_(bin),
         ext_(ext),
         depth_(depth) {}
    std::size_t array() const { return array_; }
    std::size_t map() const { return map_; }
    std::size_t str() const { return str_; }
    std::size_t bin() const { return bin_; }
    std::size_t ext() const { return ext_; }
    std::size_t depth() const { return depth_; }

private:
    std::size_t array_;
    std::size_t map_;
    std::size_t str_;
    std::size_t bin_;
    std::size_t ext_;
    std::size_t depth_;
};

namespace detail {

class unpack_user;

void unpack_uint8(uint8_t d, msghandle::object& o);

void unpack_uint16(uint16_t d, msghandle::object& o);

void unpack_uint32(uint32_t d, msghandle::object& o);

void unpack_uint64(uint64_t d, msghandle::object& o);

void unpack_int8(int8_t d, msghandle::object& o);

void unpack_int16(int16_t d, msghandle::object& o);

void unpack_int32(int32_t d, msghandle::object& o);

void unpack_int64(int64_t d, msghandle::object& o);

void unpack_float(float d, msghandle::object& o);

void unpack_double(double d, msghandle::object& o);

void unpack_nil(msghandle::object& o);

void unpack_true(msghandle::object& o);

void unpack_false(msghandle::object& o);

struct unpack_array;

void unpack_array_item(msghandle::object& c, msghandle::object const& o);

struct unpack_map;

void unpack_map_item(msghandle::object& c, msghandle::object const& k, msghandle::object const& v);

void unpack_str(unpack_user& u, const char* p, uint32_t l, msghandle::object& o);

void unpack_bin(unpack_user& u, const char* p, uint32_t l, msghandle::object& o);

void unpack_ext(unpack_user& u, const char* p, std::size_t l, msghandle::object& o);

class unpack_stack;

void init_count(void* buffer);

void decr_count(void* buffer);

void incr_count(void* buffer);

#if defined(MSGHANDLE_USE_CPP03)

_msghandle_atomic_counter_t get_count(void* buffer);

#else  // defined(MSGHANDLE_USE_CPP03)

std::atomic<unsigned int> const& get_count(void* buffer);

#endif // defined(MSGHANDLE_USE_CPP03)

struct fix_tag {
    char f1[65]; // FIXME unique size is required. or use is_same meta function.
};

template <typename T>
struct value;

template <typename T>
typename msghandle::enable_if<sizeof(T) == sizeof(fix_tag)>::type load(uint32_t& dst, const char* n);

template <typename T>
typename msghandle::enable_if<sizeof(T) == 1>::type load(T& dst, const char* n);

template <typename T>
typename msghandle::enable_if<sizeof(T) == 2>::type load(T& dst, const char* n);

template <typename T>
typename msghandle::enable_if<sizeof(T) == 4>::type load(T& dst, const char* n);

template <typename T>
typename msghandle::enable_if<sizeof(T) == 8>::type load(T& dst, const char* n);

class context;

} // detail


typedef object_handle unpacked;

/// Unpacking class for a stream deserialization.
class unpacker;

/// Unpack msghandle::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
object_handle unpack(
    const char* data, std::size_t len,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());


/// Unpack msghandle::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 *
 */
void unpack(
    object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 *
 */
void unpack(
    object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 *
 */
void unpack(
    object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 *
 */
void unpack(
    object_handle& result,
    const char* data, std::size_t len,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param z The msghandle::zone that is used as a memory of unpacked msghandle objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return msghandle::object that contains unpacked data.
 *
 */
msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param z The msghandle::zone that is used as a memory of unpacked msghandle objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return msghandle::object that contains unpacked data.
 *
 */
msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param z The msghandle::zone that is used as a memory of unpacked msghandle objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return msghandle::object that contains unpacked data.
 *
 */
msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msghandle::object from a buffer.
/**
 * @param z The msghandle::zone that is used as a memory of unpacked msghandle objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * @return msghandle::object that contains unpacked data.
 *
 */
msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());


/// Unpack msghandle::object from a buffer. [obsolete]
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msghandle::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msghandle::object.
 *
 * This function is obsolete. Use the reference inteface version of unpack functions instead of the pointer interface version.
 */
void unpack(
    object_handle* result,
    const char* data, std::size_t len, std::size_t* off = MSGHANDLE_NULLPTR, bool* referenced = MSGHANDLE_NULLPTR,
    unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR, unpack_limit const& limit = unpack_limit());


namespace detail {

parse_return
unpack_imp(const char* data, std::size_t len, std::size_t& off,
           msghandle::zone& result_zone, msghandle::object& result, bool& referenced,
           unpack_reference_func f, void* user_data,
           unpack_limit const& limit);

} // detail


/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_UNPACK_DECL_HPP
