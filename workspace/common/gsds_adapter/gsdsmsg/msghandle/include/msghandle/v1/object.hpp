//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2014 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_OBJECT_HPP
#define MSGHANDLE_V1_OBJECT_HPP

#include "msghandle/object_decl.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <cstring>
#include <stdexcept>
#include <typeinfo>
#include <limits>
#include <ostream>
#include <typeinfo>
#include <iomanip>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

struct object_kv {
    msghandle::object key;
    msghandle::object val;
};

struct object::with_zone : msghandle::object {
    with_zone(msghandle::zone& z) : zone(z) { }
    msghandle::zone& zone;
private:
    with_zone();
};


/// The class holds object and zone
class object_handle {
public:
    /// Constructor that creates nil object and null zone.
    object_handle() {}

    /// Constructor that creates an object_handle holding object `obj` and zone `z`.
    /**
     * @param obj object
     * @param z zone
     */
    object_handle(
        msghandle::object const& obj,
#if defined(MSGHANDLE_USE_CPP03)
        msghandle::unique_ptr<msghandle::zone> z
#else  // defined(MSGHANDLE_USE_CPP03)
        msghandle::unique_ptr<msghandle::zone>&& z
#endif // defined(MSGHANDLE_USE_CPP03)
    ) :
        m_obj(obj), m_zone(msghandle::move(z)) { }

    void set(msghandle::object const& obj)
        { m_obj = obj; }

    /// Get object reference
    /**
     * @return object
     */
    const msghandle::object& get() const
        { return m_obj; }

    /**
     * @return object (to mimic smart pointers).
     */
    const msghandle::object& operator*() const
        { return get(); }

    /**
     * @return the address of the object (to mimic smart pointers).
     */
    const msghandle::object* operator->() const
        { return &get(); }

    /// Get unique_ptr reference of zone.
    /**
     * @return unique_ptr reference of zone
     */
    msghandle::unique_ptr<msghandle::zone>& zone()
        { return m_zone; }

    /// Get unique_ptr const reference of zone.
    /**
     * @return unique_ptr const reference of zone
     */
    const msghandle::unique_ptr<msghandle::zone>& zone() const
        { return m_zone; }

#if defined(MSGHANDLE_USE_CPP03)
    struct object_handle_ref {
        object_handle_ref(object_handle* oh):m_oh(oh) {}
        object_handle* m_oh;
    };

    object_handle(object_handle& other):
        m_obj(other.m_obj),
        m_zone(msghandle::move(other.m_zone)) {
    }

    object_handle(object_handle_ref ref):
        m_obj(ref.m_oh->m_obj),
        m_zone(msghandle::move(ref.m_oh->m_zone)) {
    }

    object_handle& operator=(object_handle& other) {
        m_obj = other.m_obj;
        m_zone = msghandle::move(other.m_zone);
        return *this;
    }

    object_handle& operator=(object_handle_ref ref) {
        m_obj = ref.m_oh->m_obj;
        m_zone = msghandle::move(ref.m_oh->m_zone);
        return *this;
    }

    operator object_handle_ref() {
        return object_handle_ref(this);
    }
#endif // defined(MSGHANDLE_USE_CPP03)

private:
    msghandle::object m_obj;
    msghandle::unique_ptr<msghandle::zone> m_zone;
};

namespace detail {

template <std::size_t N>
inline std::size_t add_ext_type_size(std::size_t size) {
    return size + 1;
}

template <>
inline std::size_t add_ext_type_size<4>(std::size_t size) {
    return size == 0xffffffff ? size : size + 1;
}

} // namespace detail
class object_parser {
private:
    enum next_ret {
        cont,
        finish,
        abort
    };
    struct elem {
        elem(msghandle::object const* p, std::size_t r)
            : rest(r), is_map(false), is_key(false) {
            as.obj_ptr = p;
        }

        elem(msghandle::object_kv const* p, std::size_t r)
            : rest(r), is_map(true), is_key(true) {
            as.kv_ptr = p;
        }

        msghandle::object const& get() const {
            if (is_map) {
                if (is_key) {
                    return as.kv_ptr->key;
                }
                else {
                    return as.kv_ptr->val;
                }
            }
            else {
                return *as.obj_ptr;
            }
        }

        template <typename Visitor>
        next_ret next(Visitor& v) {
            if (rest == 0) {
                if (is_map) {
                    if (!v.end_map()) return abort;
                }
                else {
                    if (!v.end_array()) return abort;
                }
                return finish;
            }
            else {
                if (is_map) {
                    if (is_key) {
                        if (!v.end_map_key()) return abort;
                        if (!v.start_map_value()) return abort;
                        is_key = false;
                    }
                    else {
                        if (!v.end_map_value()) return abort;
                        --rest;
                        if (rest == 0) {
                            if (!v.end_map()) return abort;
                            return finish;
                        }
                        if (!v.start_map_key()) return abort;
                        ++as.kv_ptr;
                        is_key = true;
                    }
                }
                else {
                    if (!v.end_array_item()) return abort;
                    --rest;
                    if (rest == 0) {
                        if (!v.end_array()) return abort;
                        return finish;
                    }
                    if (!v.start_array_item()) return abort;
                    ++as.obj_ptr;
                }
                return cont;
            }
        }

        union {
            msghandle::object const* obj_ptr;
            msghandle::object_kv const* kv_ptr;
        } as;
        std::size_t rest;
        bool is_map;
        bool is_key;
    };
public:
    explicit object_parser(msghandle::object const& obj):m_current(&obj) {}
    template <typename Visitor>
    void parse(Visitor& v) {
        while (true) {
            bool start_collection = false;
            switch(m_current->type) {
            case msghandle::type::NIL:
                if (!v.visit_nil()) return;
                break;
            case msghandle::type::BOOLEAN:
                if (!v.visit_boolean(m_current->via.boolean)) return;
                break;
            case msghandle::type::POSITIVE_INTEGER:
                if (!v.visit_positive_integer(m_current->via.u64)) return;
                break;
            case msghandle::type::NEGATIVE_INTEGER:
                if (!v.visit_negative_integer(m_current->via.i64)) return;
                break;
            case msghandle::type::FLOAT32:
                if (!v.visit_float32(static_cast<float>(m_current->via.f64))) return;
                break;
            case msghandle::type::FLOAT64:
                if (!v.visit_float64(m_current->via.f64)) return;
                break;
            case msghandle::type::STR:
                if (!v.visit_str(m_current->via.str.ptr, m_current->via.str.size)) return;
                break;
            case msghandle::type::BIN:
                if (!v.visit_bin(m_current->via.bin.ptr, m_current->via.bin.size)) return;
                break;
            case msghandle::type::EXT:
                msghandle::detail::check_container_size<sizeof(std::size_t)>(m_current->via.ext.size);
                if (!v.visit_ext(m_current->via.ext.ptr, m_current->via.ext.size + 1)) return;
                break;
            case msghandle::type::ARRAY:
                if (!v.start_array(m_current->via.array.size)) return;
                m_ctx.push_back(elem(m_current->via.array.ptr, m_current->via.array.size));
                start_collection = m_current->via.array.size != 0;
                if (start_collection) {
                    if (!v.start_array_item()) return;
                }
                break;
            case msghandle::type::MAP:
                if (!v.start_map(m_current->via.map.size)) return;
                m_ctx.push_back(elem(m_current->via.map.ptr, m_current->via.map.size));
                start_collection = m_current->via.map.size != 0;
                if (start_collection) {
                    if (!v.start_map_key()) return;
                }
                break;
            default:
                throw msghandle::type_error();
                break;
            }
            if (m_ctx.empty()) return;
            if (!start_collection) {
                while (true) {
                    next_ret r = m_ctx.back().next(v);
                    if (r == finish) {
                        m_ctx.pop_back();
                        if (m_ctx.empty()) return;
                    }
                    else if (r == cont) {
                        break;
                    }
                    else {
                        // abort
                        return;
                    }
                }
            }
            m_current = &m_ctx.back().get();
        }
    }
private:
    msghandle::object const* m_current;
    std::vector<elem> m_ctx;
};

template <typename Stream>
struct object_pack_visitor {
    explicit object_pack_visitor(msghandle::packer<Stream>& pk)
        :m_packer(pk) {}
    bool visit_nil() {
        m_packer.pack_nil();
        return true;
    }
    bool visit_boolean(bool v) {
        if (v) m_packer.pack_true();
        else m_packer.pack_false();
        return true;
    }
    bool visit_positive_integer(uint64_t v) {
        m_packer.pack_uint64(v);
        return true;
    }
    bool visit_negative_integer(int64_t v) {
        m_packer.pack_int64(v);
        return true;
    }
    bool visit_float32(float v) {
        m_packer.pack_float(v);
        return true;
    }
    bool visit_float64(double v) {
        m_packer.pack_double(v);
        return true;
    }
    bool visit_str(const char* v, uint32_t size) {
        m_packer.pack_str(size);
        m_packer.pack_str_body(v, size);
        return true;
    }
    bool visit_bin(const char* v, uint32_t size) {
        m_packer.pack_bin(size);
        m_packer.pack_bin_body(v, size);
        return true;
    }
    bool visit_ext(const char* v, uint32_t size) {
        m_packer.pack_ext(size - 1, static_cast<int8_t>(*v));
        m_packer.pack_ext_body(v + 1, size - 1);
        return true;
    }
    bool start_array(uint32_t num_elements) {
        m_packer.pack_array(num_elements);
        return true;
    }
    bool start_array_item() {
        return true;
    }
    bool end_array_item() {
        return true;
    }
    bool end_array() {
        return true;
    }
    bool start_map(uint32_t num_kv_pairs) {
        m_packer.pack_map(num_kv_pairs);
        return true;
    }
    bool start_map_key() {
        return true;
    }
    bool end_map_key() {
        return true;
    }
    bool start_map_value() {
        return true;
    }
    bool end_map_value() {
        return true;
    }
    bool end_map() {
        return true;
    }
private:
    msghandle::packer<Stream>& m_packer;
};


struct object_stringize_visitor {
    explicit object_stringize_visitor(std::ostream& os)
        :m_os(os) {}
    bool visit_nil() {
        m_os << "null";
        return true;
    }
    bool visit_boolean(bool v) {
        if (v) m_os << "true";
        else m_os << "false";
        return true;
    }
    bool visit_positive_integer(uint64_t v) {
        m_os << v;
        return true;
    }
    bool visit_negative_integer(int64_t v) {
        m_os << v;
        return true;
    }
    bool visit_float32(float v) {
        m_os << v;
        return true;
    }
    bool visit_float64(double v) {
        m_os << v;
        return true;
    }
    bool visit_str(const char* v, uint32_t size) {
        m_os << '"';
        for (uint32_t i = 0; i < size; ++i) {
            char c = v[i];
            switch (c) {
            case '\\':
                m_os << "\\\\";
                break;
            case '"':
                m_os << "\\\"";
                break;
            case '/':
                m_os << "\\/";
                break;
            case '\b':
                m_os << "\\b";
                break;
            case '\f':
                m_os << "\\f";
                break;
            case '\n':
                m_os << "\\n";
                break;
            case '\r':
                m_os << "\\r";
                break;
            case '\t':
                m_os << "\\t";
                break;
            default: {
                unsigned int code = static_cast<unsigned int>(c);
                if (code < 0x20 || code == 0x7f) {
                    std::ios::fmtflags flags(m_os.flags());
                    m_os << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (code & 0xff);
                    m_os.flags(flags);
                }
                else {
                    m_os << c;
                }
            } break;
            }
        }
        m_os << '"';
        return true;
    }
    bool visit_bin(const char* /*v*/, uint32_t size) {
        m_os << "\"BIN(size:" << size << ")\"";
        return true;
    }
    bool visit_ext(const char* v, uint32_t size) {
        if (size == 0) {
            m_os << "\"EXT(size:0)\"";
        }
        else {
            m_os << "\"EXT(type:" << static_cast<int>(v[0]) << ",size:" << size - 1 << ")\"";
        }
        return true;
    }
    bool start_array(uint32_t num_elements) {
        m_current_size.push_back(num_elements);
        m_os << "[";
        return true;
    }
    bool start_array_item() {
        return true;
    }
    bool end_array_item() {
        --m_current_size.back();
        if (m_current_size.back() != 0) {
            m_os << ",";
        }
        return true;
    }
    bool end_array() {
        m_current_size.pop_back();
        m_os << "]";
        return true;
    }
    bool start_map(uint32_t num_kv_pairs) {
        m_current_size.push_back(num_kv_pairs);
        m_os << "{";
        return true;
    }
    bool start_map_key() {
        return true;
    }
    bool end_map_key() {
        m_os << ":";
        return true;
    }
    bool start_map_value() {
        return true;
    }
    bool end_map_value() {
        --m_current_size.back();
        if (m_current_size.back() != 0) {
            m_os << ",";
        }
        return true;
    }
    bool end_map() {
        m_current_size.pop_back();
        m_os << "}";
        return true;
    }
private:
    std::ostream& m_os;
    std::vector<uint32_t> m_current_size;
};

struct aligned_zone_size_visitor {
    explicit aligned_zone_size_visitor(std::size_t& s)
        :m_size(s) {}
    bool visit_nil() {
        return true;
    }
    bool visit_boolean(bool) {
        return true;
    }
    bool visit_positive_integer(uint64_t) {
        return true;
    }
    bool visit_negative_integer(int64_t) {
        return true;
    }
    bool visit_float32(float) {
        return true;
    }
    bool visit_float64(double) {
        return true;
    }
    bool visit_str(const char*, uint32_t size) {
        m_size += msghandle::aligned_size(size, MSGHANDLE_ZONE_ALIGNOF(char));
        return true;
    }
    bool visit_bin(const char*, uint32_t size) {
        m_size += msghandle::aligned_size(size, MSGHANDLE_ZONE_ALIGNOF(char));
        return true;
    }
    bool visit_ext(const char*, uint32_t size) {
        m_size += msghandle::aligned_size(size, MSGHANDLE_ZONE_ALIGNOF(char));
        return true;
    }
    bool start_array(uint32_t num_elements) {
        m_size += msghandle::aligned_size(
            sizeof(msghandle::object) * num_elements,
            MSGHANDLE_ZONE_ALIGNOF(msghandle::object));
        return true;
    }
    bool start_array_item() {
        return true;
    }
    bool end_array_item() {
        return true;
    }
    bool end_array() {
        return true;
    }
    bool start_map(uint32_t num_kv_pairs) {
        m_size += msghandle::aligned_size(
            sizeof(msghandle::object_kv) * num_kv_pairs,
            MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv));
        return true;
    }
    bool start_map_key() {
        return true;
    }
    bool end_map_key() {
        return true;
    }
    bool start_map_value() {
        return true;
    }
    bool end_map_value() {
        return true;
    }
    bool end_map() {
        return true;
    }
private:
    std::size_t& m_size;
};

inline std::size_t aligned_zone_size(msghandle::object const& obj) {
    std::size_t s = 0;
    aligned_zone_size_visitor vis(s);
    msghandle::object_parser(obj).parse(vis);
    return s;
}

/// clone object
/**
 * Clone (deep copy) object.
 * The copied object is located on newly allocated zone.
 * @param obj copy source object
 *
 * @return object_handle that holds deep copied object and zone.
 */
inline object_handle clone(msghandle::object const& obj) {
    std::size_t size = msghandle::aligned_zone_size(obj);
    msghandle::unique_ptr<msghandle::zone> z(size == 0 ? MSGHANDLE_NULLPTR : new msghandle::zone(size));
    msghandle::object newobj = z.get() ? msghandle::object(obj, *z) : obj;
    return object_handle(newobj, msghandle::move(z));
}

template <typename T>
inline object::implicit_type::operator T() { return obj.as<T>(); }

namespace detail {
template <typename Stream, typename T>
struct packer_serializer {
    static msghandle::packer<Stream>& pack(msghandle::packer<Stream>& o, const T& v,SMsgHandleInfo& msghandle_info) {
        v.msghandle_pack(o,msghandle_info);
        return o;
    }
};

template <typename Stream, typename T>
struct packer_print {
    static msghandle::packer<Stream>& print(msghandle::packer<Stream>& o, const T& v,const std::string& k) {
        v.msghandle_print(o,k);
//        v.msghandle_pack(o);
        return o;
    }
};


template <typename Stream, typename T>
struct packer_checkcode {
    static msghandle::packer<Stream>& checkcode(msghandle::packer<Stream>& o, const T& v,const std::string& k) {
        v.msghandle_checkcode(o,k);
        //        v.msghandle_pack(o);
        return o;
    }
};

template <typename Stream, typename T>
struct packer_reflectget {
    static msghandle::packer<Stream>& reflectget(msghandle::packer<Stream>& o, const T& v,const std::string& k, const std::string filter) {
        v.msghandle_reflectget(o,k,filter);
        //        v.msghandle_pack(o);
        return o;
    }
};
} // namespace detail



// Adaptor functors' member functions definitions.
template <typename T, typename Enabler>
inline
msghandle::object const&
adaptor::convert<T, Enabler>::operator()(msghandle::object const& o, T& v) const {
    v.msghandle_unpack(o.convert());
    return o;
}

template <typename T, typename Enabler>
template <typename Stream>
inline
    msghandle::packer<Stream>&
    adaptor::print<T, Enabler>::operator()(msghandle::packer<Stream>& o, T const& v) const {
    return msghandle::detail::packer_print<Stream, T>::print(o, v);
}

template <typename T, typename Enabler>
template <typename Stream>
inline
    msghandle::packer<Stream>&
    adaptor::print<T, Enabler>::operator()(msghandle::packer<Stream>& o, T const& v,const std::string& k) const {
    return msghandle::detail::packer_print<Stream, T>::print(o, v,k);
}

template <typename T, typename Enabler>
template <typename Stream>
inline
    msghandle::packer<Stream>&
    adaptor::checkcode<T, Enabler>::operator()(msghandle::packer<Stream>& o, T const& v,const std::string& k) const {
    return msghandle::detail::packer_checkcode<Stream, T>::checkcode(o, v,k);
}

template <typename T, typename Enabler>
template <typename Stream>
inline
    msghandle::packer<Stream>&
    adaptor::reflectget<T, Enabler>::operator()(msghandle::packer<Stream>& o, T const& v,const std::string& k, const std::string filter) const {
    return msghandle::detail::packer_reflectget<Stream, T>::reflectget(o, v,k,filter);
}

template <typename T, typename Enabler>
template <typename Stream>
inline
    msghandle::packer<Stream>&
    adaptor::pack<T, Enabler>::operator()(msghandle::packer<Stream>& o, T const& v,SMsgHandleInfo& msghandle_info) const {
    return msghandle::detail::packer_serializer<Stream, T>::pack(o, v,msghandle_info);
}

template <typename T, typename Enabler>
inline
void
adaptor::object_with_zone<T, Enabler>::operator()(msghandle::object::with_zone& o, T const& v) const {
    v.msghandle_object(static_cast<msghandle::object*>(&o), o.zone);
}

// Adaptor functor specialization to object
namespace adaptor {

template <>
struct convert<msghandle::object> {
    msghandle::object const& operator()(msghandle::object const& o, msghandle::object& v) const {
        v = o;
        return o;
    }
};

template <>
struct pack<msghandle::object> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, msghandle::object const& v) const {
        object_pack_visitor<Stream> vis(o);
        msghandle::object_parser(v).parse(vis);
        return o;
    }
};

template <>
struct object_with_zone<msghandle::object> {
    void operator()(msghandle::object::with_zone& o, msghandle::object const& v) const {
        object_with_zone_visitor vis(o);
        msghandle::object_parser(v).parse(vis);
    }
private:
    struct object_with_zone_visitor {
        explicit object_with_zone_visitor(msghandle::object::with_zone& owz)
            :m_zone(owz.zone), m_ptr(&owz) {
            m_objs.push_back(&owz);
        }
        bool visit_nil() {
            m_ptr->type = msghandle::type::NIL;
            return true;
        }
        bool visit_boolean(bool v) {
            m_ptr->type = msghandle::type::BOOLEAN;
            m_ptr->via.boolean = v;
            return true;
        }
        bool visit_positive_integer(uint64_t v) {
            m_ptr->type = msghandle::type::POSITIVE_INTEGER;
            m_ptr->via.u64 = v;
            return true;
        }
        bool visit_negative_integer(int64_t v) {
            m_ptr->type = msghandle::type::NEGATIVE_INTEGER;
            m_ptr->via.i64 = v;
            return true;
        }
        bool visit_float32(float v) {
            m_ptr->type = msghandle::type::FLOAT32;
            m_ptr->via.f64 = v;
            return true;
        }
        bool visit_float64(double v) {
            m_ptr->type = msghandle::type::FLOAT64;
            m_ptr->via.f64 = v;
            return true;
        }
        bool visit_str(const char* v, uint32_t size) {
            m_ptr->type = msghandle::type::STR;
            m_ptr->via.str.size = size;
            char* ptr = static_cast<char*>(m_zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
            m_ptr->via.str.ptr = ptr;
            std::memcpy(ptr, v, size);
            return true;
        }
        bool visit_bin(const char* v, uint32_t size) {
            m_ptr->type = msghandle::type::BIN;
            m_ptr->via.bin.size = size;
            char* ptr = static_cast<char*>(m_zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
            m_ptr->via.bin.ptr = ptr;
            std::memcpy(ptr, v, size);
            return true;
        }
        bool visit_ext(const char* v, uint32_t size) {
            m_ptr->type = msghandle::type::EXT;

            // v contains type but length(size) doesn't count the type byte.
            // See https://github.com/msghandle/msghandle/blob/master/spec.md#ext-format-family
            m_ptr->via.ext.size = size - 1;

            char* ptr = static_cast<char*>(m_zone.allocate_align(size, MSGHANDLE_ZONE_ALIGNOF(char)));
            m_ptr->via.ext.ptr = ptr;
            std::memcpy(ptr, v, size);
            return true;
        }
        bool start_array(uint32_t num_elements) {
            m_ptr->type = msghandle::type::ARRAY;
            m_ptr->via.array.ptr = static_cast<msghandle::object*>(
                m_zone.allocate_align(
                    sizeof(msghandle::object) * num_elements, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            m_ptr->via.array.size = num_elements;
            m_objs.push_back(elem(m_ptr->via.array.ptr));
            return true;
        }
        bool start_array_item() {
            m_ptr = m_objs.back().get_item();
            return true;
        }
        bool end_array_item() {
            ++m_objs.back().as.obj;
            return true;
        }
        bool end_array() {
            m_objs.pop_back();
            return true;
        }
        bool start_map(uint32_t num_kv_pairs) {
            m_ptr->type = msghandle::type::MAP;
            m_ptr->via.map.ptr = (msghandle::object_kv*)m_zone.allocate_align(
                sizeof(msghandle::object_kv) * num_kv_pairs, MSGHANDLE_ZONE_ALIGNOF(msghandle::object_kv));
            m_ptr->via.map.size = num_kv_pairs;
            m_objs.push_back(elem(m_ptr->via.map.ptr));
            return true;
        }
        bool start_map_key() {
            m_ptr = m_objs.back().get_key();
            return true;
        }
        bool end_map_key() {
            return true;
        }
        bool start_map_value() {
            m_ptr = m_objs.back().get_val();
            return true;
        }
        bool end_map_value() {
            ++m_objs.back().as.kv;
            return true;
        }
        bool end_map() {
            m_objs.pop_back();
            return true;
        }
    private:
        struct elem {
            elem(msghandle::object* obj)
                :is_obj(true) {
                as.obj = obj;
            }
            elem(msghandle::object_kv* kv)
                :is_obj(false) {
                as.kv = kv;
            }
            msghandle::object* get_item() {
                return as.obj;
            }
            msghandle::object* get_key() {
                return &as.kv->key;
            }
            msghandle::object* get_val() {
                return &as.kv->val;
            }
            union {
                msghandle::object* obj;
                msghandle::object_kv* kv;
            } as;
            bool is_obj;
        };
        std::vector<elem> m_objs;
        msghandle::zone& m_zone;
        msghandle::object* m_ptr;
    };
};

// Adaptor functor specialization to object::with_zone

template <>
struct object_with_zone<msghandle::object::with_zone> {
    void operator()(
        msghandle::object::with_zone& o,
        msghandle::object::with_zone const& v) const {
        o << static_cast<msghandle::object const&>(v);
    }
};


} // namespace adaptor


// obsolete
template <typename Type>
class define : public Type {
public:
    typedef Type msghandle_type;
    typedef define<Type> define_type;
    define() {}
    define(const msghandle_type& v) : msghandle_type(v) {}

    template <typename Packer>
    void msghandle_pack(Packer& o,SMsgHandleInfo& msghandle_info) const
    {
        msghandle::operator<<(o, static_cast<const msghandle_type&>(*this));
    }

    void msghandle_unpack(object const& o)
    {
        msghandle::operator>>(o, static_cast<msghandle_type&>(*this));
    }
};

// deconvert operator

template <typename Stream>
template <typename T>
inline msghandle::packer<Stream>& packer<Stream>::pack(const T& v,SMsgHandleInfo& msghandle_info)
{
    // msghandle::operator<<(*this, v);
    msghandle::v1::packfunc(*this, v,msghandle_info);
    return *this;
}

template <typename Stream>
template <typename T>
inline msghandle::packer<Stream>& packer<Stream>::print(const T& v,const std::string& k)
{
//    msghandle::operator<<(*this, v,k);
    msghandle::v1::operatorfunc(*this, v,k);
    return *this;
}

template <typename Stream>
template <typename T>
inline msghandle::packer<Stream>& packer<Stream>::checkcode(const T& v,const std::string& k)
{
    msghandle::v1::operatorcheckcode(*this, v,k);
    return *this;
}

template <typename Stream>
template <typename T>
inline msghandle::packer<Stream>& packer<Stream>::reflectget(const T& v,const std::string& k, const std::string filter)
{
    std::string result1 = filter.substr(0, filter.find(".")); // 截取到第一个"."之前的部分
    std::string result2 = filter.substr(filter.find(".") + 1); // 截取第一个"."之后的部分
    if(k == result1)
        msghandle::v1::operatorreflectget(*this, v,k,result2);
    return *this;
}

struct object_equal_visitor {
    object_equal_visitor(msghandle::object const& obj, bool& result)
        :m_ptr(&obj), m_result(result) {}
    bool visit_nil() {
        if (m_ptr->type != msghandle::type::NIL) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_boolean(bool v) {
        if (m_ptr->type != msghandle::type::BOOLEAN || m_ptr->via.boolean != v) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_positive_integer(uint64_t v) {
        if (m_ptr->type != msghandle::type::POSITIVE_INTEGER || m_ptr->via.u64 != v) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_negative_integer(int64_t v) {
        if (m_ptr->type != msghandle::type::NEGATIVE_INTEGER || m_ptr->via.i64 != v) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_float32(float v) {
        if (m_ptr->type != msghandle::type::FLOAT32 || m_ptr->via.f64 != v) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_float64(double v) {
        if (m_ptr->type != msghandle::type::FLOAT64 || m_ptr->via.f64 != v) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_str(const char* v, uint32_t size) {
        if (m_ptr->type != msghandle::type::STR ||
            m_ptr->via.str.size != size ||
            std::memcmp(m_ptr->via.str.ptr, v, size) != 0) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_bin(const char* v, uint32_t size) {
        if (m_ptr->type != msghandle::type::BIN ||
            m_ptr->via.bin.size != size ||
            std::memcmp(m_ptr->via.bin.ptr, v, size) != 0) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool visit_ext(const char* v, uint32_t size) {
        if (m_ptr->type != msghandle::type::EXT ||
            m_ptr->via.ext.size != size - 1 ||
            std::memcmp(m_ptr->via.ext.ptr, v, size) != 0) {
            m_result = false;
            return false;
        }
        return true;
    }
    bool start_array(uint32_t num_elements) {
        if (m_ptr->type != msghandle::type::ARRAY ||
            m_ptr->via.array.size != num_elements) {
            m_result = false;
            return false;
        }
        m_objs.push_back(elem(m_ptr->via.array.ptr));
        return true;
    }
    bool start_array_item() {
        m_ptr = m_objs.back().get_item();
        return true;
    }
    bool end_array_item() {
        ++m_objs.back().as.obj;
        return true;
    }
    bool end_array() {
        m_objs.pop_back();
        return true;
    }
    bool start_map(uint32_t num_kv_pairs) {
        if (m_ptr->type != msghandle::type::MAP ||
            m_ptr->via.array.size != num_kv_pairs) {
            m_result = false;
            return false;
        }
        m_objs.push_back(elem(m_ptr->via.map.ptr));
        return true;
    }
    bool start_map_key() {
        m_ptr = m_objs.back().get_key();
        return true;
    }
    bool end_map_key() {
        return true;
    }
    bool start_map_value() {
        m_ptr = m_objs.back().get_val();
        return true;
    }
    bool end_map_value() {
        ++m_objs.back().as.kv;
        return true;
    }
    bool end_map() {
        m_objs.pop_back();
        return true;
    }
private:
    struct elem {
        elem(msghandle::object const* obj)
            :is_obj(true) {
            as.obj = obj;
        }
        elem(msghandle::object_kv const* kv)
            :is_obj(false) {
            as.kv = kv;
        }
        msghandle::object const* get_item() {
            return as.obj;
        }
        msghandle::object const* get_key() {
            return &as.kv->key;
        }
        msghandle::object const* get_val() {
            return &as.kv->val;
        }
        union {
            msghandle::object const* obj;
            msghandle::object_kv const* kv;
        } as;
        bool is_obj;
    };
    std::vector<elem> m_objs;
    msghandle::object const* m_ptr;
    bool& m_result;
};

inline bool operator==(const msghandle::object& x, const msghandle::object& y)
{
    if(x.type != y.type) { return false; }
    bool b = true;
    object_equal_visitor vis(y, b);
    msghandle::object_parser(x).parse(vis);
    return b;
}

template <typename T>
inline bool operator==(const msghandle::object& x, const T& y)
try {
    return x == msghandle::object(y);
} catch (msghandle::type_error&) {
    return false;
}

inline bool operator!=(const msghandle::object& x, const msghandle::object& y)
{ return !(x == y); }

template <typename T>
inline bool operator==(const T& y, const msghandle::object& x)
{ return x == y; }

template <typename T>
inline bool operator!=(const msghandle::object& x, const T& y)
{ return !(x == y); }

template <typename T>
inline bool operator!=(const T& y, const msghandle::object& x)
{ return x != y; }


inline object::implicit_type object::convert() const
{
    return object::implicit_type(*this);
}

template <typename T>
inline
typename msghandle::enable_if<
    !msghandle::is_array<T>::value && !msghandle::is_pointer<T>::value,
    T&
>::type
object::convert(T& v) const
{
    msghandle::operator>>(*this, v);
    return v;
}

template <typename T, std::size_t N>
inline T(&object::convert(T(&v)[N]) const)[N]
{
    msghandle::operator>>(*this, v);
    return v;
}

#if !defined(MSGHANDLE_DISABLE_LEGACY_CONVERT)
template <typename T>
inline
typename msghandle::enable_if<
    msghandle::is_pointer<T>::value,
    T
>::type
object::convert(T v) const
{
    convert(*v);
    return v;
}
#endif // !defined(MSGHANDLE_DISABLE_LEGACY_CONVERT)

template <typename T>
inline bool object::convert_if_not_nil(T& v) const
{
    if (is_nil()) {
        return false;
    }
    convert(v);
    return true;
}

#if defined(MSGHANDLE_USE_CPP03)

template <typename T>
inline T object::as() const
{
    T v;
    convert(v);
    return v;
}

#else  // defined(MSGHANDLE_USE_CPP03)

template <typename T>
inline typename std::enable_if<msghandle::has_as<T>::value, T>::type object::as() const {
    return msghandle::adaptor::as<T>()(*this);
}

template <typename T>
inline typename std::enable_if<!msghandle::has_as<T>::value, T>::type object::as() const {
    T v;
//    std::vector<msghandle::type::fix_int32> key_hash_list_a = v.
    convert(v);
    return v;
}

#endif // defined(MSGHANDLE_USE_CPP03)

inline object::object()
{
    type = msghandle::type::NIL;
}

template <typename T>
inline object::object(const T& v)
{
    *this << v;
}

template <typename T>
inline object& object::operator=(const T& v)
{
    *this = object(v);
    return *this;
}

template <typename T>
inline object::object(const T& v, msghandle::zone& z)
{
    with_zone oz(z);
    msghandle::operator<<(oz, v);
    type = oz.type;
    via = oz.via;
}

template <typename T>
inline object::object(const T& v, msghandle::zone* z)
{
    with_zone oz(*z);
    msghandle::operator<<(oz, v);
    type = oz.type;
    via = oz.via;
}


// obsolete
template <typename T>
inline void convert(T& v, msghandle::object const& o)
{
    o.convert(v);
}

// obsolete
template <typename Stream, typename T>
inline void pack(msghandle::packer<Stream>& o, const T& v)
{
    o.pack(v);
}

// obsolete
template <typename Stream, typename T>
inline void print(msghandle::packer<Stream>& o, const T& v,const std::string& k)
{
    o.pack(v);
}

// obsolete
template <typename Stream, typename T>
inline void pack_copy(msghandle::packer<Stream>& o, T v)
{
    pack(o, v);
}

template <typename Stream>
inline msghandle::packer<Stream>& operator<< (msghandle::packer<Stream>& o, const msghandle::object& v)
{
    object_pack_visitor<Stream> vis(o);
    msghandle::object_parser(v).parse(vis);
    return o;
}

template <typename Stream>
inline msghandle::packer<Stream>& operator<< (msghandle::packer<Stream>& o, const msghandle::object::with_zone& v)
{
    return o << static_cast<msghandle::object>(v);
}

inline std::ostream& operator<< (std::ostream& s, const msghandle::object& v)
{
    object_stringize_visitor vis(s);
    msghandle::object_parser(v).parse(vis);
    return s;
}

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_OBJECT_HPP
