//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2016 MIZUKI Hirata
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_ITERATOR_HPP
#define MSGHANDLE_V1_ITERATOR_HPP
#if !defined(MSGHANDLE_USE_CPP03)

#include "msghandle/v1/fbuffer_decl.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

inline msghandle::object_kv* begin(msghandle::object_map &map) { return map.ptr; }
inline const msghandle::object_kv* begin(const msghandle::object_map &map) { return map.ptr; }
inline msghandle::object_kv* end(msghandle::object_map &map) { return map.ptr + map.size; }
inline const msghandle::object_kv* end(const msghandle::object_map &map) { return map.ptr + map.size; }

inline msghandle::object* begin(msghandle::object_array &array) { return array.ptr; }
inline const msghandle::object* begin(const msghandle::object_array &array) { return array.ptr; }
inline msghandle::object* end(msghandle::object_array &array) { return array.ptr + array.size; }
inline const msghandle::object* end(const msghandle::object_array &array) { return array.ptr + array.size; }

/// @cond
}
/// @endcond

}

#endif // !defined(MSGHANDLE_USE_CPP03)
#endif // MSGHANDLE_V1_ITERATOR_HPP
