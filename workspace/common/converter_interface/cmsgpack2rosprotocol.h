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
        enum EMsgpack2RosPlatformIOType
        {
            IOCTL_INIT_PRARM,
            IOCTL_MSGPACK2ROS_PUBLISH,
            IOCTL_MSGPACK2ROS_INITBAG,
            IOCTL_MSGPACK2ROS_WRITEBAG,
            IOCTL_MSGPACK2ROS_CLOSEBAG,
            IOCTL_MSGPACK2SELF_INITBAG,
            IOCTL_MSGPACK2SELF_WRITEBAG,
            IOCTL_MSGPACK2SELF_CLOSEBAG,
        };


        // struct SIdpString
        // {
        //     char str_name[128];
        //     int str_name_len;
        // };

        struct SInitParam
        {
            std::string node_name;
        };
        
        struct SInitBagParam
        {
            std::string path_name;
            std::string bag_name;
        };

        struct SPublishParam
        {
            std::string topic;
            std::string type;
            std::string content;
        };

        struct SWriteGsdsBagParam
        {
            std::string topic;
            std::string type;
            std::string content;
            double pubtime;
            double subtime;
        };

        struct SWriteRosBagParam
        {
            std::string topic;
            std::string type;
            std::string content;
            double pubtime;
            double subtime;
        };
    } // namespace idpware
} // namespace avos
