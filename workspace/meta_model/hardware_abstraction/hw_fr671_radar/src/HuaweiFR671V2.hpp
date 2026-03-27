#ifndef __HUAWEI_FR671_V2_H__
#define __HUAWEI_FR671_V2_H__

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
#include "udp_client.h"
#include "vehicle_culculate.h"

class FR671DataInteract {
private:
    FR671DataInteract() {
        last_frame = 0;
        cur_frame = 0;
        radar_packets.msop_packet.reserve(250);
        bak_radar_packets.msop_packet.reserve(250);
        radar_points.objs.reserve(50000);
        bak_radar_points.objs.reserve(50000);
    }

public:
    int GetShiftPosition()
    {
        return shift_position_.load();
    }

    void SetShiftPosition(int shift_position)
    {
        shift_position_.store(shift_position);
    }

    void GetWaitImu(int &shift_position, ara::adsfi::MsgHafLocation &imu_info, int debug=0)
    {
        std::unique_lock<std::mutex> locker(cv_mutex_imu_);
        cv_imu_.wait(locker, [&]() -> bool { return (imu_set_cnt_ != imu_get_cnt_); });
        shift_position = shift_position_.load();
        imu_info = imu_info_;
        imu_get_cnt_ = imu_set_cnt_;
        if (debug & 2)
        {
            printf("   GetWaitImu success, cnt: %llu!\n", imu_get_cnt_);
        }
    }

    void SetNotifyImu(ara::adsfi::MsgHafLocation &imu_info)
    {
        std::unique_lock<std::mutex> locker(cv_mutex_imu_);
        imu_info_ = imu_info;
        imu_set_cnt_++;
        cv_imu_.notify_one();
    }

    uint16_t GetPacketSize() {
        std::unique_lock<std::mutex> locker(mutex_);
        uint16_t                     size = radar_packets.msop_packet.size();
        return size;
    }

    void AddLidarPackets(const ara::adsfi::CommonUdpPacket &data) {
        std::unique_lock<std::mutex> locker(mutex_);
        radar_packets.msop_packet.push_back(data);
    }

    void AddLidarPoints(const ara::adsfi::SensorRadarPointCloud &packet_point_cloud, int debug = 0) {
        std::unique_lock<std::mutex> locker(mutex_);
        radar_points.header.time = packet_point_cloud.header.time;
        radar_points.objs.insert(radar_points.objs.end(), packet_point_cloud.objs.begin(),
                                 packet_point_cloud.objs.end());
        if (debug & 8)
        {
            printf("   AddLidarPoints, all_point size: %d, cuur_udp_point size: %d\n", radar_points.objs.size(), packet_point_cloud.objs.size());
        }
    }
    void SetLidarDfiop(const ara::adsfi::CommonUdpPacket &data) {
        std::unique_lock<std::mutex> locker(mutex_);
        radar_packets.difop_packet = data;
    }
    void NotifyRadarData(int pub_logic = 0) {
        double ts_tmp = 0;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            // data_valid.store(true);
            // bak_radar_packets.msop_packet.clear();
            // bak_radar_points.objs.clear();
            bak_radar_packets = radar_packets;
            // bak_radar_packets.msop_packet.swap(radar_packets.msop_packet);
            // bak_radar_packets.difop_packet = radar_packets.difop_packet;
            bak_radar_points =  radar_points;

            timeval tv{};
            gettimeofday(&tv, nullptr);
            ts_tmp = tv.tv_sec + tv.tv_usec / 1000000.0;
            // ts_tmp = ADSFI_TO_SEC(radar_points.header.time.sec, radar_points.header.time.nsec);
            // bak_radar_points.objs.swap(radar_points.objs);
            printf("   NotifyRadarData[pub_logic=%d], pointcloud size: %d, all udp bag size: %d, ts: %.3f\n",
                   pub_logic, bak_radar_points.objs.size(), bak_radar_packets.msop_packet.size(), ts_tmp); // todo
            radar_packets.msop_packet.clear();
            radar_points.objs.clear();
        }

        std::unique_lock<std::mutex> locker(cv_mutex_);
        cur_frame = ts_tmp;
        packets_cur_frame = ts_tmp;
        cv_.notify_one();
        packets_cv_.notify_one();
    }

    void GetRadarData(ara::adsfi::SensorRadarPointCloud &points) {
        static long long radar_seq = 0;

        // AINFO << "wait for data ..";
        {
            std::unique_lock<std::mutex> locker(cv_mutex_);
            cv_.wait(locker, [&]() -> bool { return (fabs(cur_frame - last_frame) > 0.01); });

            points = bak_radar_points;
            last_frame = cur_frame;
            points.header.time.sec = ADSFI_GET_SEC(cur_frame);
            points.header.time.nsec = ADSFI_GET_NSEC(cur_frame);
        }

        points.header.seq = radar_seq++;
        points.header.module_name = "radar_driver";
        points.header.data_name = "radar_point";
        points.header.frame_id = "/base_link";
    }


    void GetPacketsData(ara::adsfi::SensorRadarPacket &packets) {
        static long long packets_seq = 0;

        // AINFO << "wait for data ..";
        {
            std::unique_lock<std::mutex> locker(cv_mutex_);
            packets_cv_.wait(locker, [&]() -> bool { return (fabs(packets_cur_frame - packets_last_frame) > 0.01); });

            packets = bak_radar_packets;
            packets_last_frame = packets_cur_frame;
            packets.header.time.sec = ADSFI_GET_SEC(packets_cur_frame);
            packets.header.time.nsec = ADSFI_GET_NSEC(packets_cur_frame);
        }
        // AINFO << "wait done";

        packets.header.seq = packets_seq++;
        packets.header.module_name = "radar_driver";
        packets.header.data_name = "radar_packet";
        packets.header.frame_id = "/base_link";

    }

private:
    ara::adsfi::SensorRadarPacket     radar_packets;
    ara::adsfi::SensorRadarPointCloud radar_points;

    ara::adsfi::SensorRadarPacket     bak_radar_packets;
    ara::adsfi::SensorRadarPointCloud bak_radar_points;

    ara::adsfi::MsgHafLocation imu_info_;

private:
    // std::atomic<bool> data_valid;
    double                  last_frame;
    double                  cur_frame;
    double                  packets_last_frame = 0;
    double                  packets_cur_frame = 0;
    std::mutex              mutex_;
    std::mutex              cv_mutex_;
    std::condition_variable cv_;
    std::condition_variable packets_cv_;
    std::atomic<int> shift_position_ = 0;
    std::mutex              cv_mutex_imu_;
    std::condition_variable cv_imu_;
    uint64_t imu_set_cnt_ = 0;
    uint64_t imu_get_cnt_ = 0;

public:
    static FR671DataInteract *Instance() {            // 静态公有方法，用于获取唯一实例
        static FR671DataInteract *instance = nullptr; // 静态成员变量，用于存储唯一实例
        if (instance == nullptr) {
            instance = new FR671DataInteract();
        }
        return instance;
    }
};

class FR671 {
public:
    FR671() {
        msop_buf.udp_packet.resize(UDP_BUFFER_SIZE);
        split_angle = 180;
    }

    void ec_add(const std::string &ec, const std::string &extar_info, const std::string &step="init", uint8_t threshold=1) {
        decoder.ec_add(ec, extar_info, step, threshold);
    }

    void ec_remove(const std::string &ec, uint8_t threshold=1) {
        decoder.ec_remove(ec, threshold);
    }

    void UpdateVehicleInfo(int shift_position, const ara::adsfi::MsgHafLocation& loc_fusion) {
        std::lock_guard<std::mutex> guard(can_send_mutex_);
        PrivatePoint cur_point(loc_fusion.pose.pose.position.x, loc_fusion.pose.pose.position.y);
        calculator_ptr_->updateState(loc_fusion.v, loc_fusion.velocity.twist.angular.z);

        vehicle_info_.vehSpd = loc_fusion.v;
        // 车辆运动方向  0:静止 1:向前运动  2:向后运动
        // 1:倒档 2:空档 3:前进档 0:P档
        if (std::fabs(vehicle_info_.vehSpd) < 0.01)
        {
            vehicle_info_.veh_spd_dir = 0;
            vehicle_info_.gear = 0x1;
        }
        else
        {
            if (3 == shift_position) // 前进档
            {
                vehicle_info_.veh_spd_dir = 1;
                vehicle_info_.gear = 0x4;
            }
            else if (1 == shift_position) // 倒档
            {
                vehicle_info_.veh_spd_dir = 2;
                vehicle_info_.gear = 0x3;
            }
            else
            {
                vehicle_info_.veh_spd_dir = 0;
                vehicle_info_.gear = 0x1;
            }
        }
        vehicle_info_.yawrate = calculator_ptr_->GetYawRate() ;
        vehicle_info_.longitudeAcc = calculator_ptr_->GetLongitudinalAcc();
        vehicle_info_.lateralAcc = calculator_ptr_->GetLateralAcc();
        vehicle_info_.steer_angle = calculator_ptr_->GetSteerAngle();
        vehicle_info_.steeringVelocity = calculator_ptr_->GetSteeringRate();
        // if(1 == vehicle_info_.veh_spd_dir) //向前运动
        // {
        //  vehicle_info_.longitudeAcc = calculator_ptr_->GetLongitudinalAcc();
        // }
        // else
        // {
        //  vehicle_info_.longitudeAcc = -calculator_ptr_->GetLongitudinalAcc();
        // }

        if (driver_cfg.debug & 32)
        {
            struct timeval current_time;
            gettimeofday(&current_time, NULL);
            long long ts_ms = current_time.tv_sec * 1000 + current_time.tv_usec / 1000;
            printf("   UpdateVehicleInfo: veh_spd_dir: %d, vehSpd: %.5f, steer_angle: %.5f, steeringVelocity: %.5f,"
                   " yawrate: %.5f, longitudeAcc: %.5f, lateralAcc: %.5f,gear: %d, ts: %.3f s\n",
                   vehicle_info_.veh_spd_dir, vehicle_info_.vehSpd, vehicle_info_.steer_angle, vehicle_info_.steeringVelocity,
                   vehicle_info_.yawrate, vehicle_info_.longitudeAcc, vehicle_info_.lateralAcc, vehicle_info_.gear, ts_ms / 1000.0);
        }

    }

    void Init(int sensor_id, int interval_ms) {
        while (true)
        {
            bool ret =  decoder.isIpReachable(driver_cfg.native_ip, 1);
            if (false == ret)
            {
                printf("init: 192.168.201.101 is unreachale\n");
                ec_add("1072024", "ping radar failed", "init", 20);
            }
            else
            {
                printf("init: 192.168.201.101 is reachale\n");
                ec_remove("1072024");
                break;
            }
            usleep(1000 * 500);
        }

        std::thread([&]() {
            while (true) {
                bool ret = decoder.isIpReachable(driver_cfg.native_ip);
                if (false == ret) {
                    printf("192.168.201.101 is unreachale\n");
                    ec_add("1072024", "ping radar failed", "input", 3);
                } else {
                    ec_remove("1072024", 3);
                }
                usleep(1000 * 1000);
            }
            return 0;
        }).detach();

        decoder.Init(radar_cali_config, sensor_id, interval_ms);
        calculator_ptr_ = std::make_shared<VehicleMotionCalculator>(radar_cali_config.wheel_base_length, radar_cali_config.reduce_ratio);
        // send heartbeet
        std::thread([&]()
        {
            avos::driver::HeartBeatMsg heart_beat_msg;
            heart_beat_msg.FillCheckSum();

            avos::SocketConfig heart_beat_config;
            heart_beat_config.ip = "192.168.201.101";
            heart_beat_config.port = 52374;
            heart_beat_config.native_ip = driver_cfg.native_ip;
            heart_beat_config.native_port = driver_cfg.heart_port;

            avos::UdpClient hear_beat_client;
            int ret = hear_beat_client.Init(heart_beat_config);
            if (ret < 0)
            {
                // AERROR << "hear_beat_client Init failed";
                ec_add("1072003", "hear_beat_client Init failed", "init");
                // exit(0);
            }
            else
            {
                ec_remove("1072003");
                while (true) {
                    heart_beat_msg.FillCheckSum();
                    hear_beat_client.SendData((const char *)&heart_beat_msg, sizeof(avos::driver::HeartBeatMsg));
                    usleep(900 * 1000);
                }
            }
            // return 0;
        }).detach();

        // send vehicle info
        std::thread([&]()
        {
            avos::driver::VehicleInfoMsg vehicle_msg;
            vehicle_msg.FillCheckSum();

            avos::SocketConfig vehicle_config;
            vehicle_config.ip = "192.168.201.101";
            vehicle_config.port = 52374;

            vehicle_config.native_ip = driver_cfg.native_ip;
            vehicle_config.native_port = driver_cfg.vehicle_port;

            avos::UdpClient vehicle_client;
            int ret = vehicle_client.Init(vehicle_config);
            if (ret < 0)
            {
                // AERROR << "vehicle_client Init failed";
                ec_add("1072003", "vehicle_client Init failed", "init");
                // exit(0);
            }
            else
            {
                ec_remove("1072003");
                while (true) {
                    {
                        std::lock_guard<std::mutex> guard(can_send_mutex_);
                        vehicle_msg.SetData(vehicle_info_);
                    }
                    vehicle_msg.FillCheckSum();
                    std::cout << "send vehile info" << std::endl;
                    vehicle_client.SendData((const char *)&vehicle_msg, sizeof(avos::driver::VehicleInfoMsg));
                    usleep(20 * 1000);
                }
            }
            // return 0;
        }).detach();

        std::thread([&]()
        {
            avos::SocketConfig data_config;
            data_config.ip = "239.255.201.1";
            data_config.port = 52371;
            data_config.native_ip = driver_cfg.native_ip;
            data_config.native_port = driver_cfg.data_port;

            avos::MulticastUDP multicase_udp;
            if (multicase_udp.Init(data_config) != 0)
            {
                // AERROR << "multicase_udp Init failed";
                ec_add("1072003", "multicase_udp Init failed", "init");
                // exit(-1);
            }
            else
            {
                ec_remove("1072003");


                int socket_fd = multicase_udp.GetSocketFd();
                int curRcvBufSize = -1;
                socklen_t optlen = sizeof(curRcvBufSize);
                if (getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &curRcvBufSize, &optlen) < 0)
                {
                    printf("getsockopt fail, err_no:%d err_msg: %s\n", errno, strerror(errno));
                }
                printf("------------------ default udp receive buf size is %d ------------------\n", curRcvBufSize);
                if (driver_cfg.udp_buf_size > 0)
                {
                    int rcvBufSize = driver_cfg.udp_buf_size;
                    optlen = sizeof(rcvBufSize);
                    printf("------------------ wish to set udp receive buf size as %d ------------------\n", rcvBufSize);
                    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &rcvBufSize, optlen) < 0)
                    {
                        printf("setsockopt fail, err_no:%d err_msg: %s\n", errno, strerror(errno));
                    }
                    curRcvBufSize = -1;
                    optlen = sizeof(curRcvBufSize);
                    if (getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &curRcvBufSize, &optlen) < 0)
                    {
                        printf("getsockopt fail, err_no:%d err_msg: %s\n", errno, strerror(errno));
                    }
                    printf("------------------ now udp receive buf size is %d ------------------\n", curRcvBufSize);
                    if (curRcvBufSize != rcvBufSize * 2)
                    {
                        printf("------------------ set udp receive buf size fail ------------------\n");
                    }
                }

                while (true)
                {
                    memset(&msop_buf.udp_packet[0], 0, UDP_BUFFER_SIZE);
                    int rev_num = multicase_udp.RecieveData((char *)&msop_buf.udp_packet[0], UDP_BUFFER_SIZE);
                    if (driver_cfg.debug & 4)
                    {
                        printf("   recv udp, data size: %d\n", rev_num);
                    }
                    if(rev_num>0)
                    {
                        udp_recv_msop_queue.push(msop_buf);
                        ec_remove("1072004", 5);
                    }
                    else
                    {
                        ec_add("1072004", "recv udp data failed", "input", 5);
                    }

                }
            }
            // return 0;
        }).detach();

        // recv err_code
        std::thread([&]()
		{
            avos::SocketConfig data_config;
            data_config.ip = "239.255.201.1";
            data_config.port = 52373;
            data_config.native_ip = driver_cfg.native_ip;
            data_config.native_port = driver_cfg.data_port; //unused

            avos::MulticastUDP multicase_udp;
            if (multicase_udp.Init(data_config) != 0)
            {
                // AERROR << "err_code multicase_udp Init failed";
                ec_add("1072003", "err_code multicase_udp Init failed", "init");
                // exit(-1);
            }
            else
            {
                ec_remove("1072003");
                char err_code_buf[UDP_BUFFER_SIZE];
                timeval tv{};
                double cur_ts = 0;
                uint64_t cnt = 0;
                char msg[2048];
                std::string err_info;
                while (true)
                {
                    memset(err_code_buf, 0, UDP_BUFFER_SIZE);
                    int rev_num = multicase_udp.RecieveData((char *)&err_code_buf[0], UDP_BUFFER_SIZE);
                    if(rev_num<=0)
                    {
                        continue;
                    }
                    avos::driver::ErrCodeMsg  *err_code = (avos::driver::ErrCodeMsg *)(&(err_code_buf[0]));
                    if((driver_cfg.debug & 1024)&&(0==(cnt%1000)))
                    {
                        gettimeofday(&tv, nullptr);
                        cur_ts = tv.tv_sec + tv.tv_usec / 1000000.0;
                        memset(msg,0,sizeof(msg));
                        sprintf(msg, "recv udp, data size: %d, ts: %.3f, detail:\n"
                                "   \tnvmErrorSt is: %d\n"
                                "   \tfatalErrorSt is: %d\n"
                                "   \tgeneralErrorSt is: %d\n"
                                "   \ttempAlarmSt is: %d\n"
                                "   \tvoltageAlarmSt is: %d\n"
                                "   \tinterferenceSt is: %d\n"
                                "   \tblockAlarmSt is: %d\n"
                                "   \tvelInfoSt is: %d\n"
                                "   \teleAxisErrSt is: %d\n"
                                "   \tcfgAbnormalSt is: %d\n"
                                "   \tunCalAlarm is: %d\n"
                                "   \tTsnSynSt is: %d\n"
                                "   \tTriggerLogUpload is: %d\n",
                                rev_num, cur_ts,
                                err_code->nvmErrorSt,
                                err_code->fatalErrorSt,
                                err_code->generalErrorSt,
                                err_code->tempAlarmSt,
                                err_code->voltageAlarmSt,
                                err_code->interferenceSt,
                                err_code->blockAlarmSt,
                                err_code->velInfoSt,
                                err_code->eleAxisErrSt,
                                err_code->cfgAbnormalSt,
                                err_code->unCalAlarm,
                                err_code->TsnSynSt,
                                err_code->TriggerLogUpload);
                        ApInfo("parse") << msg;
                    }
                    cnt++;

                    // upload error_code
                    if(err_code->nvmErrorSt)
                    {
                        ec_add("1072012", "nvmErrorSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072012", 3);
                    }

                    if(err_code->fatalErrorSt)
                    {
                        ec_add("1072013", "fatalErrorSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072013", 3);
                    }

                    if(err_code->generalErrorSt)
                    {
                        ec_add("1072014", "generalErrorSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072014", 3);
                    }

                    if(err_code->tempAlarmSt)
                    {
                        err_info = (1 == err_code->tempAlarmSt)?"Minor":"Major";
                        ec_add("1072015", "tempAlarmSt check failed, level "+err_info, "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072015", 3);
                    }

                    if(err_code->voltageAlarmSt)
                    {
                        err_info = (1 == err_code->voltageAlarmSt)?"Minor":"Major";
                        ec_add("1072016", "voltageAlarmSt check failed, level "+err_info, "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072016", 3);
                    }

                    if(err_code->interferenceSt)
                    {
                        ec_add("1072017", "interferenceSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072017", 3);
                    }

                    if(err_code->blockAlarmSt)
                    {
                        err_info = (1 == err_code->blockAlarmSt)?"partional block":"full block";
                        ec_add("1072018", "blockAlarmSt check failed, level "+err_info, "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072018", 3);
                    }

                    if(err_code->velInfoSt)
                    {
                        ec_add("1072019", "velInfoSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072019", 3);
                    }

                    if(err_code->eleAxisErrSt)
                    {
                        err_info = (1 == err_code->eleAxisErrSt)?"Minor":"Major";
                        ec_add("1072020", "eleAxisErrSt check failed, level "+err_info, "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072020", 3);
                    }

                    if(err_code->cfgAbnormalSt)
                    {
                        ec_add("1072021", "cfgAbnormalSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072021", 3);
                    }

                    if(err_code->unCalAlarm)
                    {
                        ec_add("1072022", "unCalAlarm check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072022", 3);
                    }

                    if(err_code->TsnSynSt)
                    {
                        ec_add("1072023", "TsnSynSt check failed", "parse", 3);
                    }
                    else
                    {
                        ec_remove("1072023", 3);
                    }

                    // if(err_code->TriggerLogUpload)
                    // {
                    //     ec_add("1072024", "TriggerLogUpload check failed", 3);
                    // }
                    // else
                    // {
                    //     ec_remove("1072024", 3);
                    // }
                }
            }
            // return 0;
        }).detach();

        std::thread([&]() {
            auto                  ptr = CustomStack::Instance();
            bool                  need_push = false;
            ara::adsfi::CommonUdpPacket       msop_buf_prepare;
            ara::adsfi::SensorRadarPointCloud       packet_point_cloud;

            uint64_t first_pkg_ts;
            int pub_logic;
            while (1) {
                while (udp_recv_msop_queue.size() != 0) {
                    if (need_push) {
                        FR671DataInteract::Instance()->AddLidarPoints(packet_point_cloud, driver_cfg.debug);
                        FR671DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                        need_push = false;
                    }

                    msop_buf_prepare = udp_recv_msop_queue.popFront();
                    packet_point_cloud.objs.clear();

                    decoder.ConvertPacktsToPoint(msop_buf_prepare, packet_point_cloud, start_time_stamp, pub_logic);

                    //pub_logic: 0 not fill not pub; 1 first pub then fill cloud poind; 2 only fill cloud poind; 3 fill cloud poind then pub
                    if (3 == pub_logic) // current udp is last, should fill then notify
                    {

                        FR671DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                        FR671DataInteract::Instance()->AddLidarPoints(packet_point_cloud, driver_cfg.debug);

                        FR671DataInteract::Instance()->NotifyRadarData(pub_logic);

                    } else if (2 == pub_logic) // current udp is not last, should fill but no notify.
                    {
                        FR671DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                        FR671DataInteract::Instance()->AddLidarPoints(packet_point_cloud, driver_cfg.debug);

                    } else if (1 == pub_logic) // lost udp packet, this is a new frame. first notiry, then save data.
                    {
                        need_push = true;
                        // FR671DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                        FR671DataInteract::Instance()->NotifyRadarData(pub_logic);
                        printf("   pub_logic=1, lost udp packet, first pub, then save data \n");
                    } else if (0 == pub_logic) // add header packet
                    {
                        FR671DataInteract::Instance()->AddLidarPackets(msop_buf_prepare);
                    }
                }
                if (driver_cfg.debug & 16)
                {
                    printf("   udp_recv_msop_queue.size() is: %d\n", udp_recv_msop_queue.size());
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }).detach();

        std::thread([&]() {
            int shift_position = 0;
            ara::adsfi::MsgHafLocation imu_info;
            while (1) {
                FR671DataInteract::Instance()->GetWaitImu(shift_position, imu_info, driver_cfg.debug);
                UpdateVehicleInfo(shift_position, imu_info);
            }
        }).detach();
    }

public:
    avos::SocketConfig                  difop_config;
    avos::SocketConfig                  msop_config;
    avos::driver::DecoderConfig   radar_cali_config;
    avos::driver::HWRadarDriverConfig     driver_cfg;
    uint64_t   start_time_stamp = 0; //ms
    uint64_t   end_time_stamp = 0;
    uint64_t   cloud_time_stamp = 0;
    int        sensor_id_ = 0;
    avos::driver::HWDecoder decoder;
    std::mutex can_send_mutex_;
    avos::driver::VehicleInfo vehicle_info_;
    std::shared_ptr<VehicleMotionCalculator> calculator_ptr_;
private:
    double      split_angle;
    ara::adsfi::CommonUdpPacket            msop_buf;
    UserLockQueue<ara::adsfi::CommonUdpPacket> udp_recv_msop_queue;
};



//     exchange_ptr->GetRadarData(packets, points, driver_ptr->driver_cfg.debug);

#endif
