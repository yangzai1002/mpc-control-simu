//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_UNPACK_HPP
#define MSGHANDLE_V2_UNPACK_HPP

#if MSGHANDLE_DEFAULT_API_VERSION >= 2

#include "msghandle/unpack_decl.hpp"
#include "msghandle/parse.hpp"
#include "msghandle/create_object_visitor.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond


struct zone_push_finalizer {
    zone_push_finalizer(msghandle::zone& z):m_z(&z) {}
    void set_zone(msghandle::zone& z) { m_z = &z; }
    void operator()(char* buffer) {
        m_z->push_finalizer(&detail::decr_count, buffer);
    }
    msghandle::zone* m_z;
};

class unpacker : public parser<unpacker, zone_push_finalizer>,
                 public detail::create_object_visitor {
    typedef parser<unpacker, zone_push_finalizer> parser_t;
public:
    unpacker(unpack_reference_func f = &unpacker::default_reference_func,
             void* user_data = MSGHANDLE_NULLPTR,
             std::size_t initial_buffer_size = MSGHANDLE_UNPACKER_INIT_BUFFER_SIZE,
             unpack_limit const& limit = unpack_limit())
        :parser_t(m_finalizer, initial_buffer_size),
         detail::create_object_visitor(f, user_data, limit),
         m_z(new msghandle::zone),
         m_finalizer(*m_z) {
        set_zone(*m_z);
        set_referenced(false);
    }

    detail::create_object_visitor& visitor() { return *this; }
    /// Unpack one msghandle::object.
    /**
     *
     * @param result The object that contains unpacked data.
     * @param referenced If the unpacked object contains reference of the buffer,
     *                   then set as true, otherwise false.
     *
     * @return If one msghandle::object is unpacked, then return true, if msghandle::object is incomplete
     *         and additional data is required, then return false. If data format is invalid, throw
     *         msghandle::parse_error.
     *
     * See:
     * https://github.com/msghandle/msghandle-c/wiki/v1_1_cpp_unpacker#msghandle-controls-a-buffer
     */
    bool next(msghandle::object_handle& result, bool& referenced);

    /// Unpack one msghandle::object.
    /**
     *
     * @param result The object that contains unpacked data.
     *
     * @return If one msghandle::object is unpacked, then return true, if msghandle::object is incomplete
     *         and additional data is required, then return false. If data format is invalid, throw
     *         msghandle::parse_error.
     *
     * See:
     * https://github.com/msghandle/msghandle-c/wiki/v1_1_cpp_unpacker#msghandle-controls-a-buffer
     */
    bool next(msghandle::object_handle& result);
    msghandle::zone* release_zone();
    void reset_zone();
    bool flush_zone();
private:
    static bool default_reference_func(msghandle::type::object_type /*type*/, std::size_t /*len*/, void*) {
        return true;
    }
    msghandle::unique_ptr<msghandle::zone> m_z;
    zone_push_finalizer m_finalizer;
};

inline bool unpacker::next(msghandle::object_handle& result, bool& referenced) {
    bool ret = parser_t::next();
    if (ret) {
        referenced = detail::create_object_visitor::referenced();
        result.zone().reset( release_zone() );
        result.set(data());
        reset();
    }
    else {
        result.zone().reset();
        result.set(msghandle::object());
    }
    return ret;
}

inline bool unpacker::next(msghandle::object_handle& result) {
    bool referenced;
    return next(result, referenced);
}

inline msghandle::zone* unpacker::release_zone()
{
    if(!flush_zone()) {
        return MSGHANDLE_NULLPTR;
    }

    msghandle::zone* r =  new msghandle::zone;
    msghandle::zone* old = m_z.release();
    m_z.reset(r);
    set_zone(*m_z);
    m_finalizer.set_zone(*m_z);

    return old;
}

inline void unpacker::reset_zone()
{
    m_z->clear();
}

inline bool unpacker::flush_zone()
{
    if(referenced()) {
        try {
            m_z->push_finalizer(&detail::decr_count, get_raw_buffer());
        } catch (...) {
            return false;
        }
        set_referenced(false);

        detail::incr_count(get_raw_buffer());
    }

    return true;
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit
)
{
    msghandle::object obj;
    msghandle::unique_ptr<msghandle::zone> z(new msghandle::zone);
    referenced = false;
    std::size_t noff = off;
    parse_return ret = detail::unpack_imp(
        data, len, noff, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        off = noff;
        return msghandle::object_handle(obj, msghandle::move(z));
    case PARSE_EXTRA_BYTES:
        off = noff;
        return msghandle::object_handle(obj, msghandle::move(z));
    default:
        break;
    }
    return msghandle::object_handle();
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    return msghandle::v2::unpack(data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    std::size_t off = 0;
    return msghandle::v2::unpack(data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object_handle unpack(
    const char* data, std::size_t len,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return msghandle::v2::unpack(data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    msghandle::object obj;
    msghandle::unique_ptr<msghandle::zone> z(new msghandle::zone);
    referenced = false;
    std::size_t noff = off;
    parse_return ret = detail::unpack_imp(
        data, len, noff, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        off = noff;
        result.set(obj);
        result.zone() = msghandle::move(z);
        return;
    case PARSE_EXTRA_BYTES:
        off = noff;
        result.set(obj);
        result.zone() = msghandle::move(z);
        return;
    default:
        return;
    }
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f, void* user_data,
            unpack_limit const& limit)
{
    bool referenced;
    msghandle::v2::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    std::size_t off = 0;
    msghandle::v2::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msghandle::object_handle& result,
    const char* data, std::size_t len,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    msghandle::v2::unpack(result, data, len, off, referenced, f, user_data, limit);
}


inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    msghandle::object obj;
    std::size_t noff = off;
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, noff, z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        off = noff;
        return obj;
    case PARSE_EXTRA_BYTES:
        off = noff;
        return obj;
    default:
        break;
    }
    return obj;
}

inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    return msghandle::v2::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    std::size_t off = 0;
    return msghandle::v2::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msghandle::object unpack(
    msghandle::zone& z,
    const char* data, std::size_t len,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return msghandle::v2::unpack(z, data, len, off, referenced, f, user_data, limit);
}

namespace detail {

inline parse_return
unpack_imp(const char* data, std::size_t len, std::size_t& off,
           msghandle::zone& result_zone, msghandle::object& result, bool& referenced,
           unpack_reference_func f = MSGHANDLE_NULLPTR, void* user_data = MSGHANDLE_NULLPTR,
           unpack_limit const& limit = unpack_limit())
{
    create_object_visitor v(f, user_data, limit);
    v.set_zone(result_zone);
    referenced = false;
    v.set_referenced(referenced);
    parse_return ret = parse_imp(data, len, off, v);
    referenced = v.referenced();
    result = v.data();
    return ret;
}

} // namespace detail


/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_DEFAULT_API_VERSION >= 2

#endif // MSGHANDLE_V2_UNPACK_HPP
