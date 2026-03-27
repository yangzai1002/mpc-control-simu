#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <vector>

#define MAX_CAMERA_NUM 16

namespace avos
{
    namespace idpware
    {
        enum EAp2GsdsPubPlatformIOType
        {
            IOCTL_AP2GSDSPUB_PUBLISH,
        };

        struct SAp2GsdsPublishParam
        {
            std::string topic;
            std::string type;
            void* msg;
        };

    } // namespace idpware
} // namespace avos
