//
// MessagePack for C++ memory pool
//
// Copyright (C) 2008-2014 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_ZONE_HPP
#define MSGHANDLE_V1_ZONE_HPP

#include "msghandle/cpp_config.hpp"

#if defined(MSGHANDLE_USE_CPP03)
#include "msghandle/v1/detail/cpp03_zone.hpp"
#else  // MSGHANDLE_USE_CPP03
#include "msghandle/v1/detail/cpp11_zone.hpp"
#endif // MSGHANDLE_USE_CPP03

#endif // MSGHANDLE_V1_ZONE_HPP
