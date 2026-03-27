//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2016 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_MSGHANDLE_TUPLE_DECL_HPP
#define MSGHANDLE_V1_MSGHANDLE_TUPLE_DECL_HPP

#include "msghandle/cpp_config.hpp"

#if defined(MSGHANDLE_USE_CPP03)
#include "msghandle/v1/adaptor/detail/cpp03_msghandle_tuple_decl.hpp"
#else  // MSGHANDLE_USE_CPP03
#include "msghandle/v1/adaptor/detail/cpp11_msghandle_tuple_decl.hpp"
#endif // MSGHANDLE_USE_CPP03

#endif // MSGHANDLE_V1_MSGHANDLE_TUPLE_DECL_HPP
