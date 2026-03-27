//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015-2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_TYPE_BOOST_MSGHANDLE_VARIANT_DECL_HPP
#define MSGHANDLE_V1_TYPE_BOOST_MSGHANDLE_VARIANT_DECL_HPP

#include "msghandle/versioning.hpp"
#include "msghandle/adaptor/adaptor_base.hpp"
#include "msghandle/object.hpp"
#include "msghandle/adaptor/check_container_size.hpp"
#include "msghandle/adaptor/boost/string_ref.hpp"
#include "msghandle/adaptor/ext.hpp"
#include "msghandle/adaptor/raw.hpp"

#include <string>
#include <vector>
namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {

template <typename STR, typename BIN, typename EXT>
struct basic_variant;

template <typename STR, typename BIN, typename EXT>
bool operator<(basic_variant<STR, BIN, EXT> const& lhs, basic_variant<STR, BIN, EXT> const& rhs);

template <typename STR, typename BIN, typename EXT>
bool operator==(basic_variant<STR, BIN, EXT> const& lhs, basic_variant<STR, BIN, EXT> const& rhs);

template <typename STR, typename BIN, typename EXT>
bool operator!=(basic_variant<STR, BIN, EXT> const& lhs, basic_variant<STR, BIN, EXT> const& rhs);

typedef basic_variant<std::string, std::vector<char>, msghandle::type::ext> variant;
typedef basic_variant<
#if (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 53
    boost::string_ref,
#else  // (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 53
    std::string,
#endif // (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 53
    msghandle::type::raw_ref, msghandle::type::ext_ref> variant_ref;

} // namespace type

/// @cond
} // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msghandle

#endif // MSGHANDLE_V1_TYPE_BOOST_MSGHANDLE_VARIANT_DECL_HPP
