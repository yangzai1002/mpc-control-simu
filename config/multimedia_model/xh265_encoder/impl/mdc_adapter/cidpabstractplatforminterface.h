#pragma once

#include <dlfcn.h>

#include <iostream>
#include <string>

#include "cidpabstractplatform.h"
#include "log/log.h"

namespace avos {
    namespace idpware {
        typedef CIdpAbstractPlatform* (*CreateIdpAbstractPlatformFunc)();
        typedef void (*DestroyIdpAbstractPlatformFunc)(CIdpAbstractPlatform*);

        class CIdpAbstractPlatformInterface {
        public:
            CIdpAbstractPlatformInterface() {
            }
            virtual ~CIdpAbstractPlatformInterface() {
                if (m_destroyinstance_func) {
                    m_destroyinstance_func(m_hal_platform);
                    m_hal_platform = nullptr;
                }
            }

            int open(std::string so_name, int mode) {
                void* handle = dlopen(so_name.c_str(), mode);
                if (!handle) {
                    AERROR << "Error opening library: " << dlerror();
                    return -1;
                }

                m_createinstance_func = (CreateIdpAbstractPlatformFunc)dlsym(handle, "CreateIdpAbstractPlatform");
                const char* error = dlerror();
                if (error) {
                    AERROR << "Error loading CreateIdpAbstractPlatform function: " << error;
                    dlclose(handle);
                    return -1;
                }

                m_destroyinstance_func = (DestroyIdpAbstractPlatformFunc)dlsym(handle, "DestroyIdpAbstractPlatform");
                error = dlerror();
                if (error) {
                    AERROR << "Error loading DestroyIdpAbstractPlatform function: " << error;
                    dlclose(handle);
                    return -1;
                }

                m_hal_platform = m_createinstance_func();
                if (!m_hal_platform) {
                    AERROR << "Failed to create m_hal_platform object";
                    dlclose(handle);
                    return -1;
                }

                if (m_hal_platform->open() != 0) {
                    AERROR << "Failed to initialize m_hal_platform object";
                    m_destroyinstance_func(m_hal_platform);
                    dlclose(handle);
                    return -1;
                }

                return 0; // Success
            }

            int ioctl(int request, void* argp) {
                if (m_hal_platform) {
                    return m_hal_platform->ioctl(request, argp);
                } else {
                    return -1;
                }
            }

            int read(char** ptr, size_t& len) {
                if (m_hal_platform) {
                    return m_hal_platform->read(ptr, len);
                } else {
                    return -1;
                }
            }
            int write(const char* data, size_t len) {
                if (m_hal_platform) {
                    return m_hal_platform->write(data, len);
                } else {
                    return -1;
                }
            }

        private:
            CIdpAbstractPlatform*          m_hal_platform = NULL;
            CreateIdpAbstractPlatformFunc  m_createinstance_func;
            DestroyIdpAbstractPlatformFunc m_destroyinstance_func;
        };
    } // namespace idpware

} // namespace avos
