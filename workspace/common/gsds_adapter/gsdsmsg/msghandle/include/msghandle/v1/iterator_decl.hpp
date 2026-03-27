//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2016 MIZUKI Hirata
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_V1_ITERATOR_DECL_HPP
#define MSGHANDLE_V1_ITERATOR_DECL_HPP
#if !defined(MSGHANDLE_USE_CPP03)

#include "msghandle/object_fwd.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

msghandle::object_kv* begin(msghandle::object_map &map);
const msghandle::object_kv* begin(const msghandle::object_map &map);
msghandle::object_kv* end(msghandle::object_map &map);
const msghandle::object_kv* end(const msghandle::object_map &map);

msghandle::object* begin(msghandle::object_array &array);
const msghandle::object* begin(const msghandle::object_array &array);
msghandle::object* end(msghandle::object_array &array);
const msghandle::object* end(const msghandle::object_array &array);

/// @cond
}
/// @endcond

}

#endif // !defined(MSGHANDLE_USE_CPP03)
#endif // MSGHANDLE_V1_ITERATOR_DECL_HPP
