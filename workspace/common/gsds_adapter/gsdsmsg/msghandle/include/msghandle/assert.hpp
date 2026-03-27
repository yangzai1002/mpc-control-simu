//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2022 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MSGHANDLE_ASSERT_HPP
#define MSGHANDLE_ASSERT_HPP

#if defined(MSGHANDLE_NO_BOOST)

#include <cassert>
#define MSGHANDLE_ASSERT assert

#else  // defined(MSGHANDLE_NO_BOOST)

#include <boost/assert.hpp>
#define MSGHANDLE_ASSERT BOOST_ASSERT

#endif // defined(MSGHANDLE_NO_BOOST)

#endif // MSGHANDLE_ASSERT_HPP
