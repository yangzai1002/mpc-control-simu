//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2017 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_CREATE_OBJECT_VISITOR_HPP
#define MSGHANDLE_V2_CREATE_OBJECT_VISITOR_HPP

#include "msghandle/unpack_decl.hpp"
#include "msghandle/unpack_exception.hpp"
#include "msghandle/v2/create_object_visitor_decl.hpp"
#include "msghandle/v2/null_visitor.hpp"
#include "msghandle/assert.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace detail {

class create_object_visitor : public msghandle::v2::null_visitor {
public:
    create_object_visitor(unpack_reference_func f, void* user_data, unpack_limit const& limit)
        :m_func(f), m_user_data(user_data), m_limit(limit) {
        m_stack.reserve(MSGHANDLE_EMBED_STACK_SIZE);
        m_stack.push_back(&m_obj);
    }

#if !defined(MSGHANDLE_USE_CPP03)
    create_object_visitor(create_object_visitor&& other)
        :m_func(other.m_func),
         m_user_data(other.m_user_data),
         m_limit(std::move(other.m_limit)),
         m_stack(std::move(other.m_stack)),
         m_zone(other.m_zone),
         m_referenced(other.m_referenced) {
        other.m_zone = MSGHANDLE_NULLPTR;
        m_stack[0] = &m_obj;
    }
    create_object_visitor& operator=(create_object_visitor&& other) {
        this->~create_object_visitor();
        new (this) create_object_visitor(std::move(other));
        return *this;
    }
#endif // !defined(MSGHANDLE_USE_CPP03)

    void init() {
        m_stack.resize(1);
        m_obj = msghandle::object();
        m_stack[0] = &m_obj;
    }
    msghandle::object const& data() const
    {
        return m_obj;
    }
    msghandle::zone const& zone() const { return *m_zone; }
    msghandle::zone& zone() { return *m_zone; }
    void set_zone(msghandle::zone& zone) { m_zone = &zone; }
    bool referenced() const { return m_referenced; }
    void set_referenced(bool referenced) { m_referenced = referenced; }
    // visit functions
    bool visit_nil() {
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::NIL;
        return true;
    }
    bool visit_boolean(bool v) {
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::BOOLEAN;
        obj->via.boolean = v;
        return true;
    }
    bool visit_positive_integer(uint64_t v) {
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::POSITIVE_INTEGER;
        obj->via.u64 = v;
        return true;
    }
    bool visit_negative_integer(int64_t v) {
        msghandle::object* obj = m_stack.back();
        if(v >= 0) {
            obj->type = msghandle::type::POSITIVE_INTEGER;
            obj->via.u64 = static_cast<uint64_t>(v);
        }
        else {
            obj->type = msghandle::type::NEGATIVE_INTEGER;
            obj->via.i64 = v;
        }
        return true;
    }
    bool visit_float32(float v) {
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::FLOAT32;
        obj->via.f64 = v;
        return true;
    }
    bool visit_float64(double v) {
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::FLOAT64;
        obj->via.f64 = v;
        return true;
    }
    bool visit_str(const char* v, uint32_t size) {
        MSGHANDLE_ASSERT(v || size == 0);
        if (size > m_limit.str()) throw msghandle::str_size_overflow("str size overflow");
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::STR;
        if (m_func && m_func(obj->type, size, m_user_data)) {
            obj->via.str.ptr = v;
            obj->via.str.size = size;
            set_referenced(true);
        }
        else {
            if (v) {
                char* tmp = static_cast<char*>(zone().allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
                std::memcpy(tmp, v, size);
                obj->via.str.ptr = tmp;
                obj->via.str.size = size;
            }
            else {
                obj->via.str.ptr = MSGHANDLE_NULLPTR;
                obj->via.str.size = 0;
            }
        }
        return true;
    }
    bool visit_bin(const char* v, uint32_t size) {
        MSGHANDLE_ASSERT(v || size == 0);
        if (size > m_limit.bin()) throw msghandle::bin_size_overflow("bin size overflow");
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::BIN;
        if (m_func && m_func(obj->type, size, m_user_data)) {
            obj->via.bin.ptr = v;
            obj->via.bin.size = size;
            set_referenced(true);
        }
        else {
            if (v) {
                char* tmp = static_cast<char*>(zone().allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
                std::memcpy(tmp, v, size);
                obj->via.bin.ptr = tmp;
                obj->via.bin.size = size;
            }
            else {
                obj->via.bin.ptr = MSGHANDLE_NULLPTR;
                obj->via.bin.size = 0;
            }
        }
        return true;
    }
    bool visit_ext(const char* v, uint32_t size) {
        MSGHANDLE_ASSERT(v || size == 0);
        if (size > m_limit.ext()) throw msghandle::ext_size_overflow("ext size overflow");
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::EXT;
        if (m_func && m_func(obj->type, size, m_user_data)) {
            obj->via.ext.ptr = v;
            obj->via.ext.size = static_cast<uint32_t>(size - 1);
            set_referenced(true);
        }
        else {
            if (v) {
                char* tmp = static_cast<char*>(zone().allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
                std::memcpy(tmp, v, size);
                obj->via.ext.ptr = tmp;
                obj->via.ext.size = static_cast<uint32_t>(size - 1);
            }
            else {
                obj->via.ext.ptr = MSGHANDLE_NULLPTR;
                obj->via.ext.size = 0;
            }
        }
        return true;
    }
    bool start_array(uint32_t num_elements) {
        if (num_elements > m_limit.array()) throw msghandle::array_size_overflow("array size overflow");
        if (m_stack.size() > m_limit.depth()) throw msghandle::depth_size_overflow("depth size overflow");
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::ARRAY;
        obj->via.array.size = num_elements;
        if (num_elements == 0) {
            obj->via.array.ptr = MSGHANDLE_NULLPTR;
        }
        else {

#if SIZE_MAX == UINT_MAX
            if (num_elements > SIZE_MAX/sizeof(msghandle::object))
                throw msghandle::array_size_overflow("array size overflow");
#endif // SIZE_MAX == UINT_MAX

            size_t size = num_elements*sizeof(msghandle::object);
            obj->via.array.ptr =
                static_cast<msghandle::object*>(m_zone->allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
        }
        m_stack.push_back(obj->via.array.ptr);
        return true;
    }
    bool start_array_item() {
        return true;
    }
    bool end_array_item() {
        ++m_stack.back();
        return true;
    }
    bool end_array() {
        m_stack.pop_back();
        return true;
    }
    bool start_map(uint32_t num_kv_pairs) {
        if (num_kv_pairs > m_limit.map()) throw msghandle::map_size_overflow("map size overflow");
        if (m_stack.size() > m_limit.depth()) throw msghandle::depth_size_overflow("depth size overflow");
        msghandle::object* obj = m_stack.back();
        obj->type = msghandle::type::MAP;
        obj->via.map.size = num_kv_pairs;
        if (num_kv_pairs == 0) {
            obj->via.map.ptr = MSGHANDLE_NULLPTR;
        }
        else {

#if SIZE_MAX == UINT_MAX
            if (num_kv_pairs > SIZE_MAX/sizeof(msghandle::object_kv))
                throw msghandle::map_size_overflow("map size overflow");
#endif // SIZE_MAX == UINT_MAX
            size_t size = num_kv_pairs*sizeof(msghandle::object_kv);
            obj->via.map.ptr =
                static_cast<msghandle::object_kv*>(m_zone->allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv)));
        }
        m_stack.push_back(reinterpret_cast<msghandle::object*>(obj->via.map.ptr));
        return true;
    }
    bool start_map_key() {
        return true;
    }
    bool end_map_key() {
        ++m_stack.back();
        return true;
    }
    bool start_map_value() {
        return true;
    }
    bool end_map_value() {
        ++m_stack.back();
        return true;
    }
    bool end_map() {
        m_stack.pop_back();
        return true;
    }
    void parse_error(size_t /*parsed_offset*/, size_t /*error_offset*/) {
        throw msghandle::parse_error("parse error");
    }
    void insufficient_bytes(size_t /*parsed_offset*/, size_t /*error_offset*/) {
        throw msghandle::insufficient_bytes("insufficient bytes");
    }
private:
public:
    unpack_reference_func m_func;
    void* m_user_data;
    unpack_limit m_limit;
    msghandle::object m_obj;
    std::vector<msghandle::object*> m_stack;
    msghandle::zone* m_zone;
    bool m_referenced;
};

} // detail

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V2_CREATE_OBJECT_VISITOR_HPP
