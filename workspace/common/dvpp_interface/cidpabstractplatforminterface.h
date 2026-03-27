#pragma once

#include <dlfcn.h>

#include <iostream>
#include <string>

#include "cidpabstractplatform.h"

namespace avos {
    namespace idpware {

        typedef CIdpAbstractPlatform *(*CreateIdpAbstractPlatformFunc)();
        typedef void (*DestroyIdpAbstractPlatformFunc)(CIdpAbstractPlatform *);

        class CIdpAbstractPlatformInterface {
        public:
            CIdpAbstractPlatformInterface() {
            }
            virtual ~CIdpAbstractPlatformInterface() {
            }

            int open(std::string so_name, int mode) {
                void *handle = dlopen(so_name.c_str(), mode);
                if (!handle) {
                    std::cerr << "Error opening library: " << dlerror() << std::endl;
                    return -1;
                }

                m_createinstance_func = (CreateIdpAbstractPlatformFunc)dlsym(handle, "CreateIdpAbstractPlatform");
                const char *error = dlerror();
                if (error) {
                    std::cerr << "Error loading CreateIdpAbstractPlatform function: " << error << std::endl;
                    dlclose(handle);
                    return -1;
                }

                m_destroyinstance_func = (DestroyIdpAbstractPlatformFunc)dlsym(handle, "DestroyIdpAbstractPlatform");
                error = dlerror();
                if (error) {
                    std::cerr << "Error loading DestroyIdpAbstractPlatform function: " << error << std::endl;
                    dlclose(handle);
                    return -1;
                }

                m_hal_platform = m_createinstance_func();
                if (!m_hal_platform) {
                    std::cerr << "Failed to create m_hal_platform object" << std::endl;
                    dlclose(handle);
                    return -1;
                }

                if (m_hal_platform->open() != 0) {
                    std::cerr << "Failed to initialize m_hal_platform object" << std::endl;
                    DestroyIdpAbstractPlatformFunc(m_hal_platform);
                    dlclose(handle);
                    return -1;
                }

                return 0; // Success
            }

            int ioctl(int request, void *argp) {
                if (m_hal_platform) {
                    return m_hal_platform->ioctl(request, argp);
                } else {
                    return -1;
                }
            }

            int read(char **ptr, size_t &len) {
                if (m_hal_platform) {
                    return m_hal_platform->read(ptr, len);
                } else {
                    return -1;
                }
            }
            int write(const char *data, size_t len) {
                if (m_hal_platform) {
                    return m_hal_platform->write(data, len);
                } else {
                    return -1;
                }
            }

        private:
            CIdpAbstractPlatform *         m_hal_platform = NULL;
            CreateIdpAbstractPlatformFunc  m_createinstance_func;
            DestroyIdpAbstractPlatformFunc m_destroyinstance_func;
        };

        struct PerDataInterface {
            char *    m_ptr;
            u_int64_t m_ptr_data_len;
        };

    } // namespace idpware
} // namespace avos
