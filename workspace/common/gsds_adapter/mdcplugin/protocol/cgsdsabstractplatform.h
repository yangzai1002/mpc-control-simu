#pragma once

#include "stddef.h"

namespace avos
{
    namespace idpware
    {
        class CGsdsAbstractPlatform {
        public:
            virtual ~CGsdsAbstractPlatform() {}
            virtual int open() = 0;
            virtual int write(const char* data, size_t len) = 0;
            virtual int read(char** ptr, size_t& len) = 0;
            virtual int ioctl(int request, void* argp) = 0;
            virtual int close() = 0;
        };

    } // namespace idpware
} // namespace avos

