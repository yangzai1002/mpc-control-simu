//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V2_DEFINE_DECL_HPP
#define MSGHANDLE_V2_DEFINE_DECL_HPP

#include "msghandle/cpp_config.hpp"

#if defined(MSGHANDLE_USE_CPP03)
#include "msghandle/v2/adaptor/detail/cpp03_define_array_decl.hpp"
#include "msghandle/v2/adaptor/detail/cpp03_define_map_decl.hpp"
#else  // MSGHANDLE_USE_CPP03
#include "msghandle/v2/adaptor/detail/cpp11_define_array_decl.hpp"
#include "msghandle/v2/adaptor/detail/cpp11_define_map_decl.hpp"
#endif // MSGHANDLE_USE_CPP03

#endif // MSGHANDLE_V2_DEFINE_DECL_HPP
