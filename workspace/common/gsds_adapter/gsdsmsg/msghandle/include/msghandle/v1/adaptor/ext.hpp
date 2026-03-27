//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_EXT_HPP
#define MSGHANDLE_V1_TYPE_EXT_HPP

#include "msghandle/v1/adaptor/ext_decl.hpp"
#include "msghandle/adaptor/check_container_size.hpp"
#include <cstring>
#include <string>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

class ext {
public:
    ext() : m_data(1, 0) {}
    ext(int8_t t, const char* p, uint32_t s) {
        msghandle::detail::check_container_size_for_ext<sizeof(std::size_t)>(s);
        m_data.reserve(static_cast<std::size_t>(s) + 1);
        m_data.push_back(static_cast<char>(t));
        m_data.insert(m_data.end(), p, p + s);
    }
    ext(int8_t t, uint32_t s) {
        msghandle::detail::check_container_size_for_ext<sizeof(std::size_t)>(s);
        m_data.resize(static_cast<std::size_t>(s) + 1);
        m_data[0] = static_cast<char>(t);
    }
    explicit ext(ext_ref const&);
    int8_t type() const {
        return static_cast<int8_t>(m_data[0]);
    }
    const char* data() const {
        return &m_data[0] + 1;
    }
    char* data() {
        return &m_data[0] + 1;
    }
    uint32_t size() const {
        return static_cast<uint32_t>(m_data.size()) - 1;
    }
    bool operator== (const ext& x) const {
        return m_data == x.m_data;
    }

    bool operator!= (const ext& x) const {
        return !(*this == x);
    }

    bool operator< (const ext& x) const {
        return m_data < x.m_data;
    }

    bool operator> (const ext& x) const {
        return m_data > x.m_data;
    }
private:
    std::vector<char> m_data;
    friend class ext_ref;
};

} // namespace type

namespace adaptor {

template <>
struct convert<msghandle::type::ext> {
    msghandle::object const& operator()(msghandle::object const& o, msghandle::type::ext& v) const {
        if(o.type != msghandle::type::EXT) {
            throw msghandle::type_error();
        }
        v = msghandle::type::ext(o.via.ext.type(), o.via.ext.data(), o.via.ext.size);
        return o;
    }
};

template <>
struct pack<msghandle::type::ext> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const msghandle::type::ext& v,SMsgHandleInfo& msghandle_info) const {
        // size limit has already been checked at ext's constructor
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = v.size();
        o.pack_ext(size, v.type());
        o.pack_ext_body(v.data(), size);
        return o;
    }
};

template <>
struct object_with_zone<msghandle::type::ext> {
    void operator()(msghandle::object::with_zone& o, const msghandle::type::ext& v) const {
        // size limit has already been checked at ext's constructor
        uint32_t size = v.size();
        o.type = msghandle::type::EXT;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size + 1, MSGHANDLE_ZONE_ALIGNOF(char)));
        o.via.ext.ptr = ptr;
        o.via.ext.size = size;
        ptr[0] = static_cast<char>(v.type());
        std::memcpy(ptr + 1, v.data(), size);
    }
};

} // namespace adaptor

namespace type {

class ext_ref {
public:
    // ext_ref should be default constructible to support 'convert'.
    // A default constructed ext_ref object::m_ptr doesn't have the buffer to point to.
    // In order to avoid nullptr checking branches, m_ptr points to m_size.
    // So type() returns unspecified but valid value. It might be a zero because m_size
    // is initialized as zero, but shouldn't assume that.
    ext_ref() : m_ptr(static_cast<char*>(static_cast<void*>(&m_size))), m_size(0) {}
    ext_ref(const char* p, uint32_t s) :
        m_ptr(s == 0 ? static_cast<char*>(static_cast<void*>(&m_size)) : p),
        m_size(s == 0 ? 0 : s - 1) {
        msghandle::detail::check_container_size_for_ext<sizeof(std::size_t)>(s);
    }

    // size limit has already been checked at ext's constructor
    ext_ref(ext const& x) : m_ptr(&x.m_data[0]), m_size(x.size()) {}

    const char* data() const {
        return m_ptr + 1;
    }

    uint32_t size() const {
        return m_size;
    }

    int8_t type() const {
        return static_cast<int8_t>(m_ptr[0]);
    }

    std::string str() const {
        return std::string(m_ptr + 1, m_size);
    }

    bool operator== (const ext_ref& x) const {
        return m_size == x.m_size && std::memcmp(m_ptr, x.m_ptr, m_size) == 0;
    }

    bool operator!= (const ext_ref& x) const {
        return !(*this == x);
    }

    bool operator< (const ext_ref& x) const {
        if (m_size < x.m_size) return true;
        if (m_size > x.m_size) return false;
        return std::memcmp(m_ptr, x.m_ptr, m_size) < 0;
    }

    bool operator> (const ext_ref& x) const {
        if (m_size > x.m_size) return true;
        if (m_size < x.m_size) return false;
        return std::memcmp(m_ptr, x.m_ptr, m_size) > 0;
    }

private:
    const char* m_ptr;
    uint32_t m_size;
    friend struct adaptor::object<msghandle::type::ext_ref>;
};

inline ext::ext(ext_ref const& x) {
    // size limit has already been checked at ext_ref's constructor
    m_data.reserve(x.size() + 1);

    m_data.push_back(static_cast<char>(x.type()));
    m_data.insert(m_data.end(), x.data(), x.data() + x.size());
}

} // namespace type

namespace adaptor {

template <>
struct convert<msghandle::type::ext_ref> {
    msghandle::object const& operator()(msghandle::object const& o, msghandle::type::ext_ref& v) const {
        if(o.type != msghandle::type::EXT) { throw msghandle::type_error(); }
        v = msghandle::type::ext_ref(o.via.ext.ptr, o.via.ext.size + 1);
        return o;
    }
};

template <>
struct pack<msghandle::type::ext_ref> {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const msghandle::type::ext_ref& v,SMsgHandleInfo& msghandle_info) const {
        // size limit has already been checked at ext_ref's constructor
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = v.size();
        o.pack_ext(size, v.type());
        o.pack_ext_body(v.data(), size);
        return o;
    }
};

template <>
struct object<msghandle::type::ext_ref> {
    void operator()(msghandle::object& o, const msghandle::type::ext_ref& v) const {
        // size limit has already been checked at ext_ref's constructor
        uint32_t size = v.size();
        o.type = msghandle::type::EXT;
        o.via.ext.ptr = v.m_ptr;
        o.via.ext.size = size;
    }
};

template <>
struct object_with_zone<msghandle::type::ext_ref> {
    void operator()(msghandle::object::with_zone& o, const msghandle::type::ext_ref& v) const {
        static_cast<msghandle::object&>(o) << v;
    }
};

} // namespace adaptor

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_EXT_HPP
