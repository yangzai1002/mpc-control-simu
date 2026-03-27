/*
 * MessagePack for C++ version information
 *
 * Copyright (C) 2008-2013 FURUHASHI Sadayuki and Takatoshi Kondo
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef MSGHANDLE_VERSION_HPP
#define MSGHANDLE_VERSION_HPP

#include "version_master.hpp"

#define MSGHANDLE_STR(v) #v
#define MSGHANDLE_VERSION_I(maj, min, rev) MSGHANDLE_STR(maj) "." MSGHANDLE_STR(min) "." MSGHANDLE_STR(rev)

#define MSGHANDLE_VERSION MSGHANDLE_VERSION_I(MSGHANDLE_VERSION_MAJOR, MSGHANDLE_VERSION_MINOR, MSGHANDLE_VERSION_REVISION)

inline const char* msghandle_version(void) {
    return MSGHANDLE_VERSION;
}

inline int msghandle_version_major(void) {
    return MSGHANDLE_VERSION_MAJOR;
}

inline int msghandle_version_minor(void) {
    return MSGHANDLE_VERSION_MINOR;
}

inline int msghandle_version_revision(void) {
    return MSGHANDLE_VERSION_REVISION;
}

#endif /* msghandle/version.hpp */
