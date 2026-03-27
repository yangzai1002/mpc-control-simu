//
// MessagePack for C++ FILE* buffer adaptor
//
// Copyright (C) 2013 Vladimir Volodko
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_V1_FBUFFER_HPP
#define MSGHANDLE_V1_FBUFFER_HPP

#include "msghandle/v1/fbuffer_decl.hpp"
#include "msghandle/assert.hpp"

#include <cstdio>
#include <stdexcept>

namespace msghandle {

/// @cond
MSGHANDLE_API_VERSION_NAMESPACE(v1) {
/// @endcond

class fbuffer {
public:
    explicit fbuffer(FILE* file) : m_file(file) { }

public:
    void write(const char* buf, unsigned int len)
    {
        MSGHANDLE_ASSERT(buf || len == 0);
        if (!buf) return;
        if (1 != fwrite(buf, len, 1, m_file)) {
            throw std::runtime_error("fwrite() failed");
        }
    }

    FILE* file() const
    {
        return m_file;
    }

#if defined(MSGHANDLE_USE_CPP03)
private:
    fbuffer(const fbuffer&);
    fbuffer& operator=(const fbuffer&);
#else  // defined(MSGHANDLE_USE_CPP03)
    fbuffer(const fbuffer&) = delete;
    fbuffer& operator=(const fbuffer&) = delete;
#endif // defined(MSGHANDLE_USE_CPP03)

private:
    FILE* m_file;
};

/// @cond
}  // MSGHANDLE_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msghandle

#endif // MSGHANDLE_V1_FBUFFER_HPP
