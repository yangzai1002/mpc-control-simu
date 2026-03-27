#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>

namespace avos
{
    namespace idpware
    {
        namespace systeminfo
        {
            enum EIdpsysteminfoPlatformIOType
            {
                IOCTL_GET_DPCN, //获取数据域时间
                IOCTL_GET_MPCN,//获取管理域时间
                IOCTL_GET_BOARDINFO,//获取板级设备信息
            };

            enum EIdpTimeType
            {
                SECONDS_TYPE,
                MILLSECONDS_TYPE,
                MICROSECONDS_TYPE,
                NANOSECONDS_TYPE,
            };
            

            enum EIdpBoardStrInfoType
            {
                BOARD_TEMPERATURE_TYPE,  //板级温度
                BOARD_FANDUTY_TYPE,//获取板级风扇转速
                BOARD_VOLTAGE_TYPE,//获取板级设备电压

            };

            struct SSysTimeInfoData
            {
                EIdpTimeType time_type;
                u_int64_t stamp;
            };

            struct SSysBoardStrinfoData
            {
                EIdpBoardStrInfoType board_str_type;
                char info[409600];
                u_int32_t info_len;
            };

        } //systeminfo
    } // namespace idpware
} // namespace avos
