//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_DEQUE_HPP
#define MSGHANDLE_V1_TYPE_DEQUE_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"

#include <deque>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

#if !defined(MSGHANDLE_USE_CPP03)

template <typename T, typename Alloc>
struct as<std::deque<T, Alloc>, typename std::enable_if<msghandle::has_as<T>::value>::type> {
    std::deque<T, Alloc> operator()(const msghandle::object& o) const {
        if (o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        std::deque<T, Alloc> v;
        if (o.via.array.size > 0) {
            msghandle::object* p = o.via.array.ptr;
            msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
            do {
                v.push_back(p->as<T>());
                ++p;
            } while (p < pend);
        }
        return v;
    }
};

#endif // !defined(MSGHANDLE_USE_CPP03)

template <typename T, typename Alloc>
struct convert<std::deque<T, Alloc> > {
    msghandle::object const& operator()(msghandle::object const& o, std::deque<T, Alloc>& v) const {
        if(o.type != msghandle::type::ARRAY) { throw msghandle::type_error(); }
        v.resize(o.via.array.size);
        msghandle::object* p = o.via.array.ptr;
        msghandle::object* const pend = o.via.array.ptr + o.via.array.size;
        typename std::deque<T, Alloc>::iterator it = v.begin();
        for(; p < pend; ++p, ++it) {
            p->convert(*it);
        }
        return o;
    }
};

template <typename T, typename Alloc>
struct pack<std::deque<T, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::deque<T, Alloc>& v,SMsgHandleInfo& msghandle_info) const {
        if(msghandle_info.is_vector_T==1){
            msghandle_info.is_continuous_memory=0;
            return o;}
        uint32_t size = checked_get_container_size(v.size());
        o.pack_array(size);
        for(typename std::deque<T, Alloc>::const_iterator it(v.begin()), it_end(v.end());
            it != it_end; ++it) {
            o.pack(*it);
        }
        return o;
    }
};
template <typename T, typename Alloc>
struct print<std::deque<T, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::deque<T, Alloc>& v,const std::string& k) const {
        uint32_t size = checked_get_container_size(v.size());
        int index =0;
        o.stream()<< k <<"<std::deque>"<<":\n";
        for(typename std::deque<T, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = " "+k+"["+std::to_string(index) + "]";
            o.print(*it,key);
            index++;
        }
        return o;
    }
};
template <typename T, typename Alloc>
struct checkcode<std::deque<T, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::deque<T, Alloc>& v,const std::string& k) const {
        uint32_t size = checked_get_container_size(v.size());

        o.stream()<< k <<"std::deque\n";
        std::string key = " "+k;
        if(size==0)
        {
            T it;
            o.checkcode(it,key);
        }
        else
        {
            o.checkcode(*v.begin(),key);
        }
        return o;
    }
};

template <typename T, typename Alloc>
struct reflectget<std::deque<T, Alloc> > {
    template <typename Stream>
    msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& o, const std::deque<T, Alloc>& v,const std::string& k, const std::string filter) const {
        uint32_t size = checked_get_container_size(v.size());
        int index =0;
        for(typename std::deque<T, Alloc>::const_iterator it(v.begin()), it_end(v.end());
             it != it_end; ++it) {
            std::string key = "["+std::to_string(index) + "]";
            o.reflectget(*it,key,filter);
            index++;
        }
        return o;
    }
};

template <typename T, typename Alloc>
struct object_with_zone<std::deque<T, Alloc> > {
    void operator()(msghandle::object::with_zone& o, const std::deque<T, Alloc>& v) const {
        o.type = msghandle::type::ARRAY;
        if(v.empty()) {
            o.via.array.ptr = MSGHANDLE_NULLPTR;
            o.via.array.size = 0;
        } else {
            uint32_t size = checked_get_container_size(v.size());
            msghandle::object* p = static_cast<msghandle::object*>(o.zone.allocate_align(sizeof(msghandle::object)*size, MSGHANDLE_ZONE_ALIGNOF(msghandle::object)));
            msghandle::object* const pend = p + size;
            o.via.array.ptr = p;
            o.via.array.size = size;
            typename std::deque<T, Alloc>::const_iterator it(v.begin());
            do {
                *p = msghandle::object(*it, o.zone);
                ++p;
                ++it;
            } while(p < pend);
        }
    }
};

} // namespace adaptor

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_DEQUE_HPP
