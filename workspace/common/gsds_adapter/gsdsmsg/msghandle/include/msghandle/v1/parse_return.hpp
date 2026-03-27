//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_PARSE_RETURN_HPP
#define MSGHANDLE_V1_PARSE_RETURN_HPP

#include "msghandle/versioning.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond


// for internal use
typedef enum {
    PARSE_SUCCESS              =  2,
    PARSE_EXTRA_BYTES          =  1,
    PARSE_CONTINUE             =  0,
    PARSE_PARSE_ERROR          = -1
} parse_return;

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_PARSE_RETURN_HPP
