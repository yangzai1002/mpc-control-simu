#ifndef AVOS_THREAD_NAME_H
#define AVOS_THREAD_NAME_H

#include <thread>

namespace avos
{

class AvosThreadName
{
public:
    static void set(const std::string &name)
    {
        if (name.empty())
        {
            return;
        }
        pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
    }
};

} // namespace avos

#endif
