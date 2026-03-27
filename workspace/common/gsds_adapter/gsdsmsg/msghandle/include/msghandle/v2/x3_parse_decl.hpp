//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2017 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_X3_PARSE_DECL_HPP
#define MSGHANDLE_V2_X3_PARSE_DECL_HPP

#if defined(MSGHANDLE_USE_X3_PARSE)

#include "msghandle/versioning.hpp"

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v2) {
/// @endcond


template <typename Iterator, typename Visitor>
bool parse(Iterator&& begin, Iterator&& end, Visitor&& vis);

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msghandle


#endif // defined(MSGHANDLE_USE_X3_PARSE)

#endif // MSGHANDLE_V2_X3_PARSE_DECL_HPP
