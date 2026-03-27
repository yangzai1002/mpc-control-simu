//
// MessagePack for C++
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#include "msghandle/object.hpp"
#include "msghandle/iterator.hpp"
#include "msghandle/zone.hpp"
#include "msghandle/pack.hpp"
#include "msghandle/null_visitor.hpp"
#include "msghandle/parse.hpp"
#include "msghandle/unpack.hpp"
#include "msghandle/x3_parse.hpp"
#include "msghandle/x3_unpack.hpp"
#include "msghandle/sbuffer.hpp"
#include "msghandle/vrefbuffer.hpp"
#include "msghandle/version.hpp"
#include "msghandle/type.hpp"


#define MSG_IS_CONTINUOUS_MEM(ret) MSGHANDLE_IS_CONTINUOUS_MEM(ret)
#define MSG_DEFINE MSGHANDLE_DEFINE
#define MSG_PACK(buf,value) \
        SMsgHandleInfo info_##value; \
        msghandle::v1::pack(buf, value,info_##value);
#define MSG_UNPACK(data_ptr,data_len) \
        msghandle::unpack(data_ptr, data_len);
#define MSG_OBJECT_HANDLE  msghandle::object_handle
#define MSG_OBJECT  msghandle::object