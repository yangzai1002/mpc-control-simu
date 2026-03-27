#pragma once

#include "cgsdsabstractplatform.h"
#include <string>
#include <dlfcn.h>
#include <iostream>

namespace avos {
namespace idpware {

typedef CGsdsAbstractPlatform* (*CreateGsdsAbstractPlatformFunc)();
typedef void (*DestroyGsdsAbstractPlatformFunc)(CGsdsAbstractPlatform*);
typedef void (*FuncPtr)();
typedef FuncPtr (*CreateFunc)();

class CGsdsAbstractPlatformInterface {
public:
    CGsdsAbstractPlatformInterface() {}
    virtual ~CGsdsAbstractPlatformInterface() {
        if (m_hal_platform) {
            m_destroyinstance_func(m_hal_platform);
        }
        if (m_handle) {
            dlclose(m_handle);
        }
    }

    int open(const std::string& so_name, int mode) {
        m_handle = dlopen(so_name.c_str(), mode);
        if (!m_handle) {
            std::cerr << "Error opening library: " << dlerror() << std::endl;
            return -1;
        }

        m_createinstance_func = (CreateGsdsAbstractPlatformFunc)dlsym(m_handle, "CreateGsdsAbstractPlatform");
        const char* error = dlerror();
        if (error) {
            std::cerr << "Error loading CreateGsdsAbstractPlatform function: " << error << std::endl;
            dlclose(m_handle);
            return -1;
        }

        m_destroyinstance_func = (DestroyGsdsAbstractPlatformFunc)dlsym(m_handle, "DestroyGsdsAbstractPlatform");
        error = dlerror();
        if (error) {
            std::cerr << "Error loading DestroyGsdsAbstractPlatform function: " << error << std::endl;
            dlclose(m_handle);
            return -1;
        }

        m_hal_platform = m_createinstance_func();
        if (!m_hal_platform) {
            std::cerr << "Failed to create m_hal_platform object" << std::endl;
            dlclose(m_handle);
            return -1;
        }

        if (m_hal_platform->open() != 0) {
            std::cerr << "Failed to initialize m_hal_platform object" << std::endl;
            m_destroyinstance_func(m_hal_platform);
            dlclose(m_handle);
            return -1;
        }

        m_createfunc = (CreateFunc)dlsym(m_handle, "CreateGSDSSoaFunction");
        error = dlerror();
        if (error) {
            std::cerr << "Error retrieving function: " << error << std::endl;
            dlclose(m_handle);
            return -1;
        }

        m_func = m_createfunc();
        if (!m_func) {
            std::cerr << "Error creating function pointer" << std::endl;
            dlclose(m_handle);
            return -1;
        }

        return 0; // Success
    }

    void callFunction() {
        if (m_func) {
            m_func();
        } else {
            std::cerr << "Function pointer is not valid" << std::endl;
        }
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
    CGsdsAbstractPlatform* m_hal_platform = nullptr;
    CreateGsdsAbstractPlatformFunc m_createinstance_func = nullptr;
    DestroyGsdsAbstractPlatformFunc m_destroyinstance_func = nullptr;
    CreateFunc m_createfunc = nullptr;
    FuncPtr m_func = nullptr;
    void* m_handle = nullptr;
};

} // namespace idpware
} // namespace avos
