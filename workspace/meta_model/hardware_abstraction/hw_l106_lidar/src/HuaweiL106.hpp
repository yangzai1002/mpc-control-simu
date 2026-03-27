#ifndef __HUAWEI_L106_HPP__
#define __HUAWEI_L106_HPP__
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "config_loader/custom_stack.h"
// #include "log/log.h"
#include "ap_log/ap_log_interface.h"

#include "UserLockQueue.h"
#include "data_pool.h"
#include "huawei_decode.h"
#include "multicast_udp.h"

class L106DataInteract {
private:
    L106DataInteract() {
        data_valid.store(false);
        last_frame = 0;
        cur_frame = 0;
        lidar_packets.msop_packet.reserve(250);
        bak_lidar_packets.msop_packet.reserve(250);
        lidar_points.pointCloud.reserve(50000);
        bak_lidar_points.pointCloud.reserve(50000);
    }

public:
    uint16_t GetPacketSize() {
        std::unique_lock<std::mutex> locker(mutex_);
        uint16_t                     size = lidar_packets.msop_packet.size();
        return size;
    }

    void AddLidarPackets(const ara::adsfi::CommonUdpPacket &data) {
        std::unique_lock<std::mutex> locker(mutex_);
        lidar_packets.msop_packet.push_back(data);
    }

    void AddLidarPoints(const ara::adsfi::MsgLidarFrame &packet_point_cloud, int debug = 0) {
        std::unique_lock<std::mutex> locker(mutex_);
        lidar_points.pointCloud.insert(lidar_points.pointCloud.end(), packet_point_cloud.pointCloud.begin(),
                                   packet_point_cloud.pointCloud.end());
        if (debug) {
            printf("AddLidarPoints, all_point size: %d, cuur_udp_point size: %d\n", lidar_points.pointCloud.size(),
                   packet_point_cloud.pointCloud.size());
        }
    }
    void SetLidarDfiop(const ara::adsfi::CommonUdpPacket &data) {
        std::unique_lock<std::mutex> locker(mutex_);
        lidar_packets.difop_packet = data;
    }
    void NotifyLidarData(double ts, int debug = 0) { //ts: us
        {
            std::unique_lock<std::mutex> locker(mutex_);
            cur_frame = ts;
            packets_cur_frame = ts;
            lidar_points.timestamp.sec = ADSFI_GET_SEC(ts);
            lidar_points.timestamp.nsec = ADSFI_GET_NSEC(ts);
            bak_lidar_packets = lidar_packets;
            bak_lidar_points = lidar_points;
            timeval tv{};
            gettimeofday(&tv, nullptr);

            // cur_frame = ADSFI_TO_SEC(bak_lidar_points.timestamp.sec, bak_lidar_points.timestamp.nsec);
            double diff = tv.tv_sec + tv.tv_usec / 1000000.0 - cur_frame;
            
            ApInfo("notyfi")<< "pointcloud size:"<< bak_lidar_points.pointCloud.size() <<  ",udp bag size:" << bak_lidar_packets.msop_packet.size() <<
             ", pointcloud ts: "<<cur_frame<< 
             ", diff with sys_ts: "<< diff<<" s\n";
                   
            lidar_packets.msop_packet.clear();
            lidar_points.pointCloud.clear();
        }

        std::unique_lock<std::mutex> locker(cv_mutex_);

        cv_.notify_one();
        packets_cv_.notify_one();
        if (debug) {
            printf("NotifyLidarData once\n");
        }
    }

    void GetLidarData(ara::adsfi::MsgLidarFrame &points) {
        static long long lidar_seq = 0;

        // AINFO << "wait for data ..";
        {
            std::unique_lock<std::mutex> locker(cv_mutex_);
            cv_.wait(locker, [&]() -> bool { return (fabs(cur_frame - last_frame) > 0.01); });

            points = bak_lidar_points;
            last_frame = cur_frame;
        }

        points.seq = lidar_seq++;
        points.frameID = "/base_link";
    }


    void GetPacketsData(ara::adsfi::SensorLidarPacket &packets) {
        static long long packets_seq = 0;

        // AINFO << "wait for data ..";
        {
            std::unique_lock<std::mutex> locker(cv_mutex_);
            packets_cv_.wait(locker, [&]() -> bool { return (fabs(packets_cur_frame - packets_last_frame) > 0.01); });

            packets = bak_lidar_packets;
            packets_last_frame = packets_cur_frame;

            packets.header.time.sec = ADSFI_GET_SEC(packets_cur_frame);
            packets.header.time.nsec = ADSFI_GET_NSEC(packets_cur_frame);
        }
        // AINFO << "wait done";

        packets.header.seq = packets_seq++;
        packets.header.module_name = "lidar_driver";
        packets.header.data_name = "lidar_point";
        packets.header.frame_id = "/base_link";

    }

private:
    ara::adsfi::SensorLidarPacket     lidar_packets;
    ara::adsfi::MsgLidarFrame lidar_points;

    ara::adsfi::SensorLidarPacket     bak_lidar_packets;
    ara::adsfi::MsgLidarFrame bak_lidar_points;

private:
    std::atomic<bool>       data_valid;
    double                  last_frame;
    double                  cur_frame;
    double                  packets_last_frame = 0;
    double                  packets_cur_frame = 0;
    std::mutex              mutex_;
    std::mutex              cv_mutex_;
    std::condition_variable cv_;
    std::condition_variable packets_cv_;

public:
    static L106DataInteract *Instance() {            // 静态公有方法，用于获取唯一实例
        static L106DataInteract *instance = nullptr; // 静态成员变量，用于存储唯一实例
        if (instance == nullptr) {
            instance = new L106DataInteract();
        }
        return instance;
    }
};

class L106Driver {
public:
    L106Driver() {
        difop_buf.udp_packet.resize(sizeof(avos::driver::SupplementaryData));
        msop_buf.udp_packet.resize(UDP_BUFFER_SIZE);
        split_angle = 180;
        dif_init.store(false);
    }

    void ec_add(const std::string &ec, const std::string &extar_info, const std::string &step="init", uint8_t threshold=1) {
        decoder.ec_add(ec, extar_info, step, threshold);
    }

    void ec_remove(const std::string &ec, uint8_t threshold=1) {
        decoder.ec_remove(ec, threshold);
    }

    void Init() {
        while (true) {
            bool ret = decoder.isIpReachable(difop_config.native_ip, 1);
            if (false == ret) {
                printf("init: 192.168.1.201 is unreachale\n");
                ec_add("1022036", "ping lidar failed", "init", 20);
            } else {
                printf("init: 192.168.1.201 is reachale\n");
                ec_remove("1022036");
                break;
            }
            usleep(1000 * 500);
        }

        std::thread([&]() {
            while (true) {
                bool ret = decoder.isIpReachable(difop_config.native_ip);
                if (false == ret) {
                    printf("192.168.1.201 is unreachale\n");
                    ec_add("1022036", "ping lidar failed", "input", 3);
                } else {
                    ec_remove("1022036", 3);
                }
                usleep(1000 * 1000);
            }
            return 0;
        }).detach();

        decoder.Init(lidar_cali_config, sensor_id_);
        int ret = decoder.SetDifopConfig(difop_config);
        if (ret < 0) {
            // AERROR << "supple_info_client Init failed";
            ec_add("1022003", "supple_info_client init failed", "init");
            // exit(0);
            // printf("supple_info_client Init failed\n");
        } else {
            ec_remove("1022003");
        }

        difop_config.Print("difop_config");
        msop_config.Print("msop_config");
        if (ret >= 0) {
            std::thread([&]() {
                usleep(1000 * 10000);
                for (int i = 0; i < 5; i++) {
                    decoder.SendLidarConfigMsg(driver_cfg.phase_lock);
                    usleep(500 * 1000);
                }
                while (!decoder.IsSupplementaryInformatFull()) {
                    memset(supplementary_buf, 0, SUPPLE_BUFFER_SIZE);
                    decoder.SendAllSupplementaryInformation();
                }
                decoder.GetSupplementaryData(difop_buf);
                dif_init.store(true);
            }).detach();
        }

        std::thread([&]() {
        avos::SocketConfig  errcode_config;
        errcode_config.ip = "239.255.0.1";
        errcode_config.native_ip = msop_config.native_ip;
        errcode_config.port = 2369;
        // errcode_config.native_port = 2369;
        errcode_config.native_port = difop_config.native_port+37;
        errcode_config.Print("err_code_config");
        avos::MulticastUDP multicase_udp_err_code;
        if (multicase_udp_err_code.Init(errcode_config) != 0) {
            ec_add("1022003", "multicase_udp_err_code init failed", "init");
            // AERROR << "multicase_udp_err_code Init failed";
            // printf("multicase_udp_err_code Init failed\n");
            // exit(0);
        } else {
            ec_remove("1022003");
            uint64_t cnt = 0;
                printf("---------------------------errcode_info_client establish parse thread, error_debug: %d---------------------------\n", lidar_cali_config.error_debug);
                ////////////////////////// parse err_code
                char buf[128];
                while (true) {

                    // memset(&(decoder.lidar_err_code), 0, decoder.lidar_err_code.size());
                    memset(buf, 0, 128);
                    // int recv_size = decoder.errcode_info_client_.RecieveData((char*)&(decoder.lidar_err_code), decoder.lidar_err_code.size(),lidar_cali_config.error_debug);
                    int recv_size = multicase_udp_err_code.RecieveData(buf, 128, lidar_cali_config.error_debug);
                    memcpy(&(decoder.lidar_err_code), buf, decoder.lidar_err_code.size());
                    if((lidar_cali_config.error_debug&8) && (0==(cnt%1000)))
                    {
                        ApInfo("parse") <<"recv err_code msg size: " << recv_size;
                        ApInfo("parse") <<"EthPhyErrorState is: " << (int)decoder.lidar_err_code.pay_load.EthPhyErrorState;
                        ApInfo("parse") <<"SocCtrlErrorState is: " << (int)decoder.lidar_err_code.pay_load.SocCtrlErrorState;
                        ApInfo("parse") <<"ReceiveModuleErrorState is: " << (int)decoder.lidar_err_code.pay_load.ReceiveModuleErrorState;
                        ApInfo("parse") <<"SendModuleErrorState is: " << (int)decoder.lidar_err_code.pay_load.SendModuleErrorState;
                        ApInfo("parse") <<"InnerModulePowerState is: " << (int)decoder.lidar_err_code.pay_load.InnerModulePowerState;
                        ApInfo("parse") <<"ElectricMotorErrorState is: " << (int)decoder.lidar_err_code.pay_load.ElectricMotorErrorState;
                        ApInfo("parse") <<"WindowsHeatErrorState is: " << (int)decoder.lidar_err_code.pay_load.WindowsHeatErrorState;
                        ApInfo("parse") <<"EthCRCErrorState is: " << (int)decoder.lidar_err_code.pay_load.EthCRCErrorState;
                        ApInfo("parse") <<"SQIInadequateState is: " << (int)decoder.lidar_err_code.pay_load.SQIInadequateState;
                        ApInfo("parse") <<"SoftwareErrorState is: " << (int)decoder.lidar_err_code.pay_load.SoftwareErrorState;
                        ApInfo("parse") <<"HighTemperatureErrorState is: " << (int)decoder.lidar_err_code.pay_load.HighTemperatureErrorState;
                        ApInfo("parse") <<"LowTemperatureErrorState is: " << (int)decoder.lidar_err_code.pay_load.LowTemperatureErrorState;
                        ApInfo("parse") <<"HighVoltageErrorState is: " << (int)decoder.lidar_err_code.pay_load.HighVoltageErrorState;
                        ApInfo("parse") <<"LowVoltageErrorState is: " << (int)decoder.lidar_err_code.pay_load.LowVoltageErrorState;
                        ApInfo("parse") <<"CfgErrorState is: " << (int)decoder.lidar_err_code.pay_load.CfgErrorState;
                        ApInfo("parse") <<"TimeSynLossState is: " << (int)decoder.lidar_err_code.pay_load.TimeSynLossState;
                        ApInfo("parse") <<"TimeSynJumpState is: " << (int)decoder.lidar_err_code.pay_load.TimeSynJumpState;
                        ApInfo("parse") <<"DeviceCertiState is: " << (int)decoder.lidar_err_code.pay_load.DeviceCertiState;
                        ApInfo("parse") <<"BlockState is: " << (int)decoder.lidar_err_code.pay_load.BlockState;
                        ApInfo("parse") <<"CommunicationLossState is: " << (int)decoder.lidar_err_code.pay_load.CommunicationLossState;
                        ApInfo("parse") <<"CommunicationDataInvalidState is: " << (int)decoder.lidar_err_code.pay_load.CommunicationDataInvalidState;
                        ApInfo("parse") <<"RangingDegradationState is: " << (int)decoder.lidar_err_code.pay_load.RangingDegradationState;
                        ApInfo("parse") <<"RanginggenerallyDegradationState is: " << (int)decoder.lidar_err_code.pay_load.RanginggenerallyDegradationState;
                        ApInfo("parse") <<"LogUploadState is: " << (int)decoder.lidar_err_code.pay_load.LogUploadState;
                        ApInfo("parse") <<"LidarHeatStatus is: " << (int)decoder.lidar_err_code.pay_load.LidarHeatStatus;
                        ApInfo("parse") <<"BlindType is: " << (int)decoder.lidar_err_code.pay_load.BlindType;
                        ApInfo("parse") <<"BlindLevel1 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel1;
                        ApInfo("parse") <<"ConfidenceLevel1 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel1;
                        ApInfo("parse") <<"BlindLevel2 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel2;
                        ApInfo("parse") <<"ConfidenceLevel2 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel2;
                        ApInfo("parse") <<"BlindLevel3 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel3;
                        ApInfo("parse") <<"ConfidenceLevel3 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel3;
                        ApInfo("parse") <<"BlindLevel4 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel4;
                        ApInfo("parse") <<"ConfidenceLevel4 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel4;
                        ApInfo("parse") <<"BlindLevel5 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel5;
                        ApInfo("parse") <<"ConfidenceLevel5 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel5;
                        ApInfo("parse") <<"BlindLevel6 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel6;
                        ApInfo("parse") <<"ConfidenceLevel6 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel6;
                        ApInfo("parse") <<"BlindLevel7 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel7;
                        ApInfo("parse") <<"ConfidenceLevel7 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel7;
                        ApInfo("parse") <<"BlindLevel8 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel8;
                        ApInfo("parse") <<"ConfidenceLevel8 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel8;
                        ApInfo("parse") <<"BlindLevel9 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel9;
                        ApInfo("parse") <<"ConfidenceLevel9 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel9;
                        ApInfo("parse") <<"BlindLevel10 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel10;
                        ApInfo("parse") <<"ConfidenceLevel10 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel10;
                        ApInfo("parse") <<"BlindLevel11 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel11;
                        ApInfo("parse") <<"ConfidenceLevel11 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel11;
                        ApInfo("parse") <<"BlindLevel12 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel12;
                        ApInfo("parse") <<"ConfidenceLevel12 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel12;
                        ApInfo("parse") <<"BlindLevel13 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel13;
                        ApInfo("parse") <<"ConfidenceLevel13 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel13;
                        ApInfo("parse") <<"BlindLevel14 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel14;
                        ApInfo("parse") <<"ConfidenceLevel14 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel14;
                        ApInfo("parse") <<"BlindLevel15 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel15;
                        ApInfo("parse") <<"ConfidenceLevel15 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel15;
                        ApInfo("parse") <<"BlindLevel16 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel16;
                        ApInfo("parse") <<"ConfidenceLevel16 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel16;
                        ApInfo("parse") <<"BlindLevel17 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel17;
                        ApInfo("parse") <<"ConfidenceLevel17 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel17;
                        ApInfo("parse") <<"BlindLevel18 is: " << (int)decoder.lidar_err_code.pay_load.BlindLevel18;
                        ApInfo("parse") <<"ConfidenceLevel18 is: " << (int)decoder.lidar_err_code.pay_load.ConfidenceLevel18;
                    }
                    cnt++;

                    // upload error_code
                    if (decoder.lidar_err_code.pay_load.EthPhyErrorState) {
                        ec_add("1022012", "EthPhyErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022012", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.SocCtrlErrorState) {
                        ec_add("1022013", "SocCtrlErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022013", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.ReceiveModuleErrorState) {
                        ec_add("1022014", "ReceiveModuleErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022014", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.SendModuleErrorState) {
                        ec_add("1022015", "SendModuleErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022015", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.InnerModulePowerState) {
                        ec_add("1022016", "InnerModulePowerState check failed", "parse", 3);
                    } else {
                        ec_remove("1022016", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.ElectricMotorErrorState) {
                        ec_add("1022017", "ElectricMotorErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022017", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.WindowsHeatErrorState) {
                        ec_add("1022018", "WindowsHeatErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022018", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.EthCRCErrorState) {
                        ec_add("1022019", "EthCRCErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022019", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.SQIInadequateState) {
                        ec_add("1022020", "SQIInadequateState check failed", "parse", 3);
                    } else {
                        ec_remove("1022020", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.SoftwareErrorState) {
                        ec_add("1022021", "SoftwareErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022021", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.HighTemperatureErrorState) {
                        ec_add("1022022", "HighTemperatureErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022022", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.LowTemperatureErrorState) {
                        ec_add("1022023", "LowTemperatureErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022023", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.HighVoltageErrorState) {
                        ec_add("1022024", "HighVoltageErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022024", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.LowVoltageErrorState) {
                        ec_add("1022025", "LowVoltageErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022025", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.CfgErrorState) {
                        ec_add("1022026", "CfgErrorState check failed", "parse", 3);
                    } else {
                        ec_remove("1022026", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.TimeSynLossState) {
                        ec_add("1022027", "TimeSynLossState check failed", "parse", 3);
                    } else {
                        ec_remove("1022027", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.TimeSynJumpState) {
                        ec_add("1022028", "TimeSynJumpState check failed", "parse", 3);
                    } else {
                        ec_remove("1022028", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.DeviceCertiState) {
                        ec_add("1022029", "DeviceCertiState check failed", "parse", 3);
                    } else {
                        ec_remove("1022029", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.BlockState) {
                        std::string block_info;
                        if (0 == decoder.lidar_err_code.pay_load.BlindType) {
                            block_info = "无法判断";
                        } else if (1 == decoder.lidar_err_code.pay_load.BlindType) {
                            block_info = "建议加热类";
                        } else if (2 == decoder.lidar_err_code.pay_load.BlindType) {
                            block_info = "建议清洗类";
                        } else if (3 == decoder.lidar_err_code.pay_load.BlindType) {
                            block_info = "禁止操作类(破损、塑料带等)";
                        } else if (4 == decoder.lidar_err_code.pay_load.BlindType) {
                            block_info = "盲区有目标";
                        }

                        ec_add("1022030", "Lidar block, type " + block_info, "parse", 3);
                    } else {
                        ec_remove("1022030", 3);
                    }

                    // yixingwang, 2025-12-27, 都没有实现这个功能，所以不报这个故障
                    // if (decoder.lidar_err_code.pay_load.CommunicationLossState) {
                    //     ec_add("1022031", "CommunicationLossState check failed", 3);
                    // } else {
                    //     ec_remove("1022031", 3);
                    // }

                    if (decoder.lidar_err_code.pay_load.CommunicationDataInvalidState) {
                        ec_add("1022032", "CommunicationDataInvalidState check failed", "parse", 3);
                    } else {
                        ec_remove("1022032", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.RangingDegradationState) {
                        ec_add("1022033", "RangingDegradationState check failed", "parse", 3);
                    } else {
                        ec_remove("1022033", 3);
                    }

                    if (decoder.lidar_err_code.pay_load.RanginggenerallyDegradationState) {
                        ec_add("1022034", "RanginggenerallyDegradationState check failed", "parse", 3);
                    } else {
                        ec_remove("1022034", 3);
                    }

                    if (3 == decoder.lidar_err_code.pay_load.LidarHeatStatus) {
                        ec_add("1022035", "LidarHeatStatus check failed[加热异常]", "parse", 3);
                    } else {
                        ec_remove("1022035", 3);
                    }

                    // cat err.log2| awk '{printf("if(%s)\n{\n\tec_add(\"%s\", \"%s check failed\",
                    // 3);\n}\nelse\n{\n\tec_remove(\"%s\", 3);\n}\n\n"),$2,$3,$1,$3}'
                    // if(decoder.lidar_err_code.pay_load.LogUploadState)
                    // {
                    //     ec_add("1022035", "LogUploadState check failed", 3);
                    // }
                    // else
                    // {
                    //     ec_remove("1022035", 3);
                    // }
                }
            }
        }).detach();

        std::thread([&]() {
            avos::MulticastUDP multicase_udp;
            if (multicase_udp.Init(msop_config) != 0) {
                ec_add("1022003", "multicase_udp init failed", "init");
                // AERROR << "multicase_udp Init failed";
                // printf("multicase_udp Init failed\n");
                // exit(0);
            } else {
                ec_remove("1022003");
                while (true) {
                    memset(&msop_buf.udp_packet[0], 0, UDP_BUFFER_SIZE);
                    int rev_num = multicase_udp.RecieveData((char *)&msop_buf.udp_packet[0], UDP_BUFFER_SIZE);

                    if (driver_cfg.debug) {
                        printf("recv udp, data size: %d\n", rev_num);
                    }
                    if (rev_num > 0)
                        udp_recv_msop_queue.push(msop_buf);
                }
            }

        }).detach();

        std::thread([&]() {
            auto                  ptr = CustomStack::Instance();
            bool                  need_push = false;
            ara::adsfi::CommonUdpPacket       msop_buf_prepare;
            ara::adsfi::MsgLidarFrame packet_point_cloud;

            int      pub_logic;
            while (true) {
                while (udp_recv_msop_queue.size() != 0) {
                    if (false == dif_init.load()) {
                        udp_recv_msop_queue.clear();
                        continue;
                    }
                    if (need_push) {
                        L106DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                        L106DataInteract::Instance()->AddLidarPoints(packet_point_cloud, driver_cfg.debug);
                        need_push = false;
                    }


                    msop_buf_prepare = udp_recv_msop_queue.popFront();
                    packet_point_cloud.pointCloud.clear();

                    decoder.ConvertPacktsToPoint(msop_buf_prepare, packet_point_cloud, new_frame_ts, pub_logic);
                    if(fabs(new_frame_ts - last_frame_ts)>0.01)// new frame
                    {
                        int interval = fabs(new_frame_ts - last_frame_ts)*1000;//ms
                        if (driver_cfg.debug) {
                            printf("get new_frame_ts %.3f\n", new_frame_ts);
                        }
                        if ((interval > (driver_cfg.interval_ms * 1.5)) && (0.01 < last_frame_ts)) {
                            printf("lidar lost frame, last ts %.3f, current ts %.3f, interval_ms %d\n",
                                   last_frame_ts, new_frame_ts, interval);
                        }
                        if (new_frame_ts < last_frame_ts) {
                            printf("lidar time back to old, last ts %.3f, current ts %.3f, interval_ms %d\n",
                                   last_frame_ts, new_frame_ts, -interval);
                            ec_add("1022008", "lidar time back to old", "input");
                        } else {
                            ec_remove("1022008");
                        }
                        last_frame_ts = new_frame_ts;
                    }

                    // pub_logic: 0 not fill not pub; 1 first pub then fill cloud poind; 2 only fill cloud poind; 3 fill
                    // cloud poind then pub
                    if (3 == pub_logic) // current udp is last, should fill then notify
                    {
                        int ten_ms = ADSFI_GET_NSEC(new_frame_ts) % 100000000;
                        if (!(ten_ms > driver_cfg.ten_ms_filter_l && ten_ms < driver_cfg.ten_ms_filter_r)) {
                            L106DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                            L106DataInteract::Instance()->AddLidarPoints(packet_point_cloud, driver_cfg.debug);

                            L106DataInteract::Instance()->SetLidarDfiop(difop_buf);
                            L106DataInteract::Instance()->NotifyLidarData(frame_pub_ts, driver_cfg.debug_ts);
                        }
                        // is_first = false;

                    } else if (2 == pub_logic) // current udp is not last, should fill but no notify.
                    {
                        int ten_ms = ADSFI_GET_NSEC(new_frame_ts) % 100000000;
                        if (!(ten_ms > driver_cfg.ten_ms_filter_l && ten_ms < driver_cfg.ten_ms_filter_r)) {
                            L106DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                            L106DataInteract::Instance()->AddLidarPoints(packet_point_cloud, driver_cfg.debug);

                            if(fabs(new_frame_ts - frame_pub_ts)>0.01)
                            {
                                frame_pub_ts = new_frame_ts;
                            }
                        }

                    } else if (1 == pub_logic) // lost udp packet, this is a new frame. first notiry, then save data.
                    {
                        int ten_ms = ADSFI_GET_NSEC(new_frame_ts) % 100000000; // current frame ts
                        if (!(ten_ms > driver_cfg.ten_ms_filter_l &&
                              ten_ms < driver_cfg.ten_ms_filter_r)) // fill to next frame
                        {
                            need_push = true;
                        }
                        if (L106DataInteract::Instance()->GetPacketSize() > 0) {
                            L106DataInteract::Instance()->SetLidarDfiop(difop_buf);
                            printf("pub_logic=1, lost udp packet, first pub, then save data \n");
                            L106DataInteract::Instance()->NotifyLidarData(frame_pub_ts, driver_cfg.debug_ts);
                        }
                        frame_pub_ts = new_frame_ts;
                    }
                }
                if (driver_cfg.debug) {
                    printf("udp_recv_msop_queue.size() is: %d, dif_init is: %d\n", udp_recv_msop_queue.size(),
                           dif_init.load());
                }
                if (false == dif_init.load()) {
                    while (udp_recv_msop_queue.size() > 0) {
                        udp_recv_msop_queue.popFront();
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }).detach();
    }

public:
    avos::SocketConfig                difop_config;
    avos::SocketConfig                msop_config;
    avos::driver::DecoderConfig       lidar_cali_config;
    avos::driver::HWLidarDriverConfig driver_cfg;
    char                              supplementary_buf[SUPPLE_BUFFER_SIZE];
    avos::driver::PacketData          udp_recv_data;
    bool                              keep_alive_ = true;
    std::atomic<bool>                 dif_init;
    double                          new_frame_ts = 0; // ms
    double                          last_frame_ts = 0;
    double                          frame_pub_ts     = 0;
    avos::driver::HWDecoder           decoder;
    int                               sensor_id_ = 0;

private:
    double                     split_angle;
    ara::adsfi::CommonUdpPacket            msop_buf, difop_buf;
    UserLockQueue<ara::adsfi::CommonUdpPacket> udp_recv_msop_queue;
    UserLockQueue<ara::adsfi::CommonUdpPacket> udp_recv_difop_queue;
};


#endif

