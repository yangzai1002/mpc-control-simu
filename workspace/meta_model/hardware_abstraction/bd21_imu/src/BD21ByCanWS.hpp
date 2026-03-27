#ifndef __BD21_BY_CAN_WS_HPP__
#define __BD21_BY_CAN_WS_HPP__


#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <cmath>
#include <condition_variable>

#include "config_loader/custom_stack.h"
#include "monitor/faulthandle_api.hpp"
// #include "log/log.h"
#include "ap_log/ap_log_interface.h"


#include "UserLockQueue.h"
#include "SocketCan.hpp"
#include "can_recv_protocol.h"
#include "geometry_tools.hpp"
#include "ymdhms2GpsTs.hpp"




#define UUDT  (315964800 - 18)
#define WEEKS  (7 * 24 * 3600.0)

inline double NavboxUtc2Unix(unsigned short utc_w, double utc_s) {
    return UUDT + utc_w * WEEKS + utc_s;
}



class BD21DataInteract {
private:
    BD21DataInteract() { // 构造函数私有化
                         // 初始化操作
    }

public:
    void SetLocFusion(const avos::BD21::LocFusionData &loc) {
        std::unique_lock<std::mutex> lock(locfusion_mtx);
        loc_fusion_data = loc;
         m_locfusion_cond.notify_one();
    }

    bool GetLocFusion(avos::BD21::LocFusionData &loc) {
        std::unique_lock<std::mutex> lock(locfusion_mtx);
        m_locfusion_cond.wait(lock);
        if (loc.ins_cnt == loc_fusion_data.ins_cnt) {
            return false;
        }
        loc = loc_fusion_data;
        return true;
    }

    void SetGpsData(const avos::BD21::GnssData &gps_data) {
        std::unique_lock<std::mutex> lock(gnss_mtx);
        gnss_data = gps_data;
        m_gnss_cond.notify_one();
    }

    bool GetGpsData(avos::BD21::GnssData &gps_data) {
        std::unique_lock<std::mutex> lock(gnss_mtx);
        m_gnss_cond.wait(lock);
        if (gps_data.gnss_cnt == gnss_data.gnss_cnt) {
            return false;
        }
        gps_data = gnss_data;
        return true;
    }

    void SetImuData(const avos::BD21::SensorImuData &imu) {
        std::unique_lock<std::mutex> lock(imu_mtx);
        sensor_imu_data = imu;
        m_imu_cond.notify_one();
    }

    bool GetImuData(avos::BD21::SensorImuData &imu, bool sync_data=false) {
        std::unique_lock<std::mutex> lock(imu_mtx);
        if(sync_data)
        {
            imu = sensor_imu_data;
            return true;
        }
        m_imu_cond.wait(lock);
        if (imu.imu_cnt == sensor_imu_data.imu_cnt) {
            return false;
        }
        imu = sensor_imu_data;
        return true;
    }

    void SetWheelSpeedData(const avos::BD21::WheelSpeedData &wheel_speed) {
        std::unique_lock<std::mutex> lock(wheel_speed_mtx);
        wheel_speed_data = wheel_speed;

        m_wheel_speed_cond.notify_one();
    }

    bool GetWheelSpeedData(avos::BD21::WheelSpeedData &wheel_speed) {
        std::unique_lock<std::mutex> lock(wheel_speed_mtx);
        m_wheel_speed_cond.wait(lock);
        if (wheel_speed.wheel_speed_cnt == wheel_speed_data.wheel_speed_cnt) {
            return false;
        }
        wheel_speed = wheel_speed_data;
        return true;
    }

private:
    std::mutex locfusion_mtx;
    std::mutex imu_mtx;
    std::mutex gnss_mtx;
    std::mutex wheel_speed_mtx;

    avos::BD21::LocFusionData loc_fusion_data;
    avos::BD21::SensorImuData sensor_imu_data;
    avos::BD21::GnssData      gnss_data;
    avos::BD21::WheelSpeedData wheel_speed_data;

    std::condition_variable m_locfusion_cond;
    std::condition_variable m_imu_cond;
    std::condition_variable m_gnss_cond;
    std::condition_variable m_wheel_speed_cond;

public:
    static BD21DataInteract *Instance() {            // 静态公有方法，用于获取唯一实例
        static BD21DataInteract *instance = nullptr; // 静态成员变量，用于存储唯一实例
        if (instance == nullptr) {
            instance = new BD21DataInteract();
        }
        return instance;
    }
};


class BD21Driver {
public:
    BD21Driver() {
        loc_fusion_parse_.lat = 720;
        loc_fusion_parse_.lon = 720;
        loc_fusion_parse_.v = -2;
        gnss_parse_.lat = 720;
        gnss_parse_.lon = 720;
    }

    // 角度转弧度
    double deg2rad(double deg) {
        return deg * M_PI / 180.0;
    }

    // 计算两点经纬度距离（Haversine公式）
    // lat1/lon1: 点1经纬度（角度）
    // lat2/lon2: 点2经纬度（角度）
    // 返回值：距离（单位：km）
    double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double radLat1 = deg2rad(lat1);
        double radLat2 = deg2rad(lat2);
        double radLon1 = deg2rad(lon1);
        double radLon2 = deg2rad(lon2);

        double deltaLat = radLat2 - radLat1;
        double deltaLon = radLon2 - radLon1;

        double a = pow(sin(deltaLat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(deltaLon / 2), 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        double distance = EARTH_RADIUS * c;

        return distance;
    }

    void Init() {
        // recv thread
        std::thread([&]() {
            std::vector<struct can_filter> rfilter;
            struct can_filter              temp_filter;

            temp_filter.can_id = 0x02020001;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020002;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020003;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020004;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020005;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020006;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020007;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020008;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02020009;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02030001;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02030002;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02030003;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02030004;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02040001;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02040002;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02010001;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);
            temp_filter.can_id = 0x02010002;
            temp_filter.can_mask = CAN_EFF_MASK;
            rfilter.push_back(temp_filter);

            SocketCanFrame can_frame_rev;
            auto           socket_can_transmitter_ptr = std::make_shared<SocketcanTransmitter>(can_config_, rfilter);
            if (socket_can_transmitter_ptr->get_sock_fd() < 0) {
                ec_add("1011001", "establish can_sock failed" , "init");
                // exit(0);
            } else {
                ec_remove("1011001");
                static bool nedd_check_remove = false;
                static int  must_check_remove_cnt = 0;
                while (true) {
                    memset(&can_frame_rev, 0, sizeof(can_frame_rev));
                    if (socket_can_transmitter_ptr->ReadCanFrame(can_frame_rev) == -1)
                    {
			            nedd_check_remove = ec_add("1011002", "ReadCanFrame error", "input", 3);
                        must_check_remove_cnt = 0;
                    }
                    else
                    {

			            if (nedd_check_remove || must_check_remove_cnt < 3) {
                            must_check_remove_cnt++;
                            ec_remove("1011002", 3);
                        }
                        if (debug_ & 1) {
                            printf("recv can_msg id: 0x%x\n", can_frame_rev.id & CAN_EFF_MASK);
                        }
                        can_msg_q_.push(can_frame_rev);
                    }
                }
            }
        }).detach();

        // recv wheel_speed thread
        std::thread([&]() {
            std::vector<struct can_filter> rfilter;
            struct can_filter              temp_filter;

            temp_filter.can_id = 0x289;
	        temp_filter.can_mask = CAN_SFF_MASK;

            rfilter.push_back(temp_filter);

            SocketCanFrame can_frame_rev;
            auto           socket_can_transmitter_ptr = std::make_shared<SocketcanTransmitter>(can_config_ws_, rfilter);
            if (socket_can_transmitter_ptr->get_sock_fd() < 0) {
                ec_add("1011001", "establish wheel_speed can_sock failed", "init");
                // exit(0);
            } else {
                ec_remove("1011001");
                static bool nedd_check_remove = false;
                static int  must_check_remove_cnt = 0;
                while (true) {
                    memset(&can_frame_rev, 0, sizeof(can_frame_rev));
                    if (socket_can_transmitter_ptr->ReadCanFrame(can_frame_rev) == -1)
                    {
			            nedd_check_remove = ec_add("1011002", "ReadCanFrame of wheel_speed error", "input", 3);
                        must_check_remove_cnt = 0;
                    }
                    else
                    {
			            if (nedd_check_remove || must_check_remove_cnt < 3) {
                            must_check_remove_cnt++;
                            ec_remove("1011002", 3);
                        }
                        if (debug_ & 1) {
                            printf("recv wheel_speed can_msg id: 0x%x\n", can_frame_rev.id);
                        }
                        can_msg_q_.push(can_frame_rev);
                    }
                }
            }
        }).detach();

        // parse thread
        std::thread([&]() {
            auto           ptr = CustomStack::Instance();
            auto           change_ptr = BD21DataInteract::Instance();
            SocketCanFrame can_frame_prepare;
            GPSTime        gps_ts = {0, 0.0};
            TimeWithMillis cur_ms = {};
            timeval        tv = {};
            timeval        tv_data{};
            gettimeofday(&tv_data, nullptr);
            double start_ts = tv_data.tv_sec + tv_data.tv_usec / 1000000.0;
            double check_ts = 0;
            double cur_ts_s = 0.0;
            double diff = 0;

	    static bool     nedd_check_remove_1011005 = false;
            static uint64_t must_check_remove_cnt_1011005 = 0;
            static bool     nedd_check_remove_1011013 = false;
            static uint64_t must_check_remove_cnt_1011013 = 0;
            static bool     nedd_check_remove_1011015 = false;
            static uint64_t must_check_remove_cnt_1011015 = 0;
            static bool     nedd_check_remove_1011016 = false;
            static uint64_t must_check_remove_cnt_1011016 = 0;
            static bool     nedd_check_remove_1011017 = false;
            static uint64_t must_check_remove_cnt_1011017 = 0;
            static bool     nedd_check_remove_1011018 = false;
            static uint64_t must_check_remove_cnt_1011018 = 0;
            static bool     nedd_check_remove_1011019 = false;
            static uint64_t must_check_remove_cnt_1011019 = 0;
            static bool     nedd_check_remove_1011020 = false;
            static uint64_t must_check_remove_cnt_1011020 = 0;
            static bool     nedd_check_remove_1011021 = false;
            static uint64_t must_check_remove_cnt_1011021 = 0;
            static bool     nedd_check_remove_1011022 = false;
            static uint64_t must_check_remove_cnt_1011022 = 0;
            static bool     nedd_check_remove_1011003 = false;
            static uint64_t must_check_remove_cnt_1011003 = 0;
            static bool     nedd_check_remove_1011006 = false;
            static uint64_t must_check_remove_cnt_1011006 = 0;
            static bool     nedd_check_remove_1011004 = false;
            static uint64_t must_check_remove_cnt_1011004 = 0;

            while (1) {
                while (can_msg_q_.size() != 0) {
                    gettimeofday(&tv_data, nullptr);
                    start_ts = tv_data.tv_sec + tv_data.tv_usec / 1000000.0;
                    if (nedd_check_remove_1011003 || must_check_remove_cnt_1011003 < 1) {
                        must_check_remove_cnt_1011003++;
                    	ec_remove("1011003");
                    }

                    can_frame_prepare = can_msg_q_.popFront();
                    switch (can_frame_prepare.id & CAN_EFF_MASK) {
                        case 0x02020001:
                            ins_tx1.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx1.Print("ins_tx1");
                            }
                            if (false == use_raw_data_) {
                                sensor_imu_parse_.acce_x = ins_tx1.ins_acc_x;
                                sensor_imu_parse_.acce_y = ins_tx1.ins_acc_y;
                                sensor_imu_parse_.acce_z = ins_tx1.ins_acc_z;
                            }
                            if (debug_ & 32) {
                                ins_tx1.Print("ins_tx1");
                            }
                            gnss_parse_.status = ins_tx1.inss_gnssflag_pos;
                            loc_fusion_parse_.gps_status = ins_tx1.inss_gnssflag_pos;
                            gnss_parse_.heading_stat = ins_tx1.inss_gnssflag_heading;
                            // if (!((4 == gnss_parse_.status) || (5 == gnss_parse_.status))) {
                            if (gnss_parse_.status < 4) {
                                nedd_check_remove_1011005 = ec_add("1011005", "location status < 4", "parse", 3);
                                must_check_remove_cnt_1011005 = 0;
                            } else {
                                if (nedd_check_remove_1011005 || must_check_remove_cnt_1011005 < 3) {
                                    must_check_remove_cnt_1011005++;
                               	    ec_remove("1011005", 3);
                            	}
                            }

                            break;
                        case 0x02020002:
                            ins_tx2.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx2.Print("ins_tx2");
                            }
                            if (use_raw_data_) {
                                sensor_imu_parse_.acce_x = ins_tx2.raw_acc_x;
                                sensor_imu_parse_.acce_y = ins_tx2.raw_acc_y;
                                sensor_imu_parse_.acce_z = ins_tx2.raw_acc_z;
                            }
                            gnss_parse_.diff_age = ins_tx2.ins_gnss_age;
                            gnss_parse_.satenum = ins_tx2.ins_num_sv;

                            break;

                        case 0x02020003:
                            ins_tx3.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);

                            if (debug_ & 2) {
                                ins_tx3.Print("ins_tx3");
                            }
                            if (debug_ & 32) {
                                ins_tx3.Print("ins_tx3");
                            }
                            if (false == use_raw_data_) {
                                sensor_imu_parse_.gyro_x = ins_tx3.ins_gyro_x;
                                sensor_imu_parse_.gyro_y = ins_tx3.ins_gyro_y;
                                sensor_imu_parse_.gyro_z = ins_tx3.ins_gyro_z;
                                if (use_sys_time_) {
                                    cur_ms = Ymdhms2GpsTs::getCurrentTimeWithMillis();
                                    gps_ts = Ymdhms2GpsTs::utcToGps(cur_ms);
                                    sensor_imu_parse_.week = gps_ts.week;
                                    sensor_imu_parse_.utc_time = gps_ts.tow;
                                } else {
                                    sensor_imu_parse_.week = ins_tx11.gnss_week;
                                    sensor_imu_parse_.utc_time = ins_tx6.ins_time / 1000.0; // ms to s
                                }

                                if (debug_ & 4) {
                                    printf("sensor_imu_parse's week is: %d, utc_time is: %.3f\n",
                                           sensor_imu_parse_.week, sensor_imu_parse_.utc_time);
                                }

                                sensor_imu_parse_.imu_cnt++;
                                change_ptr->SetImuData(sensor_imu_parse_);
                                if (debug_ & 512) {
                                    printf("get imu ts: %.3f\n", start_ts);
                                }
                            }
                            loc_fusion_parse_.status = ins_tx3.inss_status;
                            if(!((loc_fusion_parse_.status == 4)||(loc_fusion_parse_.status == 5)||(loc_fusion_parse_.status == 6)))
                            {
				                nedd_check_remove_1011013 = ec_add("1011013", "not ins loction", "parse", 3);
                                must_check_remove_cnt_1011013 = 0;
                            } else {
                                if (nedd_check_remove_1011013 || must_check_remove_cnt_1011013 < 3) {
                                    must_check_remove_cnt_1011013++;
                                    ec_remove("1011013", 3);
				}
                            }

                            break;
                        case 0x02020004:
                            ins_tx4.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx4.Print("ins_tx4");
                            }
                            if (use_raw_data_) {
                                sensor_imu_parse_.gyro_x = ins_tx4.raw_gyro_x;
                                sensor_imu_parse_.gyro_y = ins_tx4.raw_gyro_y;
                                sensor_imu_parse_.gyro_z = ins_tx4.raw_gyro_z;
                                if (use_sys_time_) {
                                    cur_ms = Ymdhms2GpsTs::getCurrentTimeWithMillis();
                                    gps_ts = Ymdhms2GpsTs::utcToGps(cur_ms);
                                    sensor_imu_parse_.week = gps_ts.week;
                                    sensor_imu_parse_.utc_time = gps_ts.tow;
                                } else {
                                    sensor_imu_parse_.week = ins_tx11.gnss_week;
                                    sensor_imu_parse_.utc_time = ins_tx6.ins_time / 1000.0;
                                }

                                if (debug_ & 4) {
                                    printf("sensor_imu_parse's week is: %d, utc_time is: %.3f\n",
                                           sensor_imu_parse_.week, sensor_imu_parse_.utc_time);
                                }

                                sensor_imu_parse_.imu_cnt++;
                                change_ptr->SetImuData(sensor_imu_parse_);
                                if (debug_ & 512) {
                                    printf("get raw imu ts: %.3f\n", start_ts);
                                }
                            }
                            loc_fusion_parse_.err_status = ins_tx4.inss_error_status;
                            if(ins_tx4.inss_error_status&(1<<0))
                            {
				                nedd_check_remove_1011015 = ec_add("1011015", "gyro_x error", "parse", 3);
                                must_check_remove_cnt_1011015 = 0;
                            } else {
                                if (nedd_check_remove_1011015 || must_check_remove_cnt_1011015 < 3) {
                                    must_check_remove_cnt_1011015++;
                                    ec_remove("1011015", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<1))
                            {
				                nedd_check_remove_1011016 = ec_add("1011016", "gyro_y error", "parse", 3);
                                must_check_remove_cnt_1011016 = 0;
                            } else {
                                if (nedd_check_remove_1011016 || must_check_remove_cnt_1011016 < 3) {
                                    must_check_remove_cnt_1011016++;
                                    ec_remove("1011016", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<2))
                            {
				                nedd_check_remove_1011017 = ec_add("1011017", "gyro_z error", "parse", 3);
                                must_check_remove_cnt_1011017 = 0;
                            } else {
                                if (nedd_check_remove_1011017 || must_check_remove_cnt_1011017 < 3) {
                                    must_check_remove_cnt_1011017++;
                                    ec_remove("1011017", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<3))
                            {
				                nedd_check_remove_1011018 = ec_add("1011018", "acc_x error", "parse", 3);
                                must_check_remove_cnt_1011018 = 0;
                            } else {
                                if (nedd_check_remove_1011018 || must_check_remove_cnt_1011018 < 3) {
                                    must_check_remove_cnt_1011018++;
                                    ec_remove("1011018", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<4))
                            {
				                nedd_check_remove_1011019 = ec_add("1011019", "acc_y error", "parse", 3);
                                must_check_remove_cnt_1011019 = 0;
                            } else {
                                if (nedd_check_remove_1011019 || must_check_remove_cnt_1011019 < 3) {
                                    must_check_remove_cnt_1011019++;
                                    ec_remove("1011019", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<5))
                            {
				                nedd_check_remove_1011020 = ec_add("1011020", "acc_z error", "parse", 3);
                                must_check_remove_cnt_1011020 = 0;
                            } else {
                                if (nedd_check_remove_1011020 || must_check_remove_cnt_1011020 < 3) {
                                    must_check_remove_cnt_1011020++;
                                    ec_remove("1011020", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<6))
                            {
				                nedd_check_remove_1011021 = ec_add("1011021", "can not recv gps heading data", "parse", 3);
                                must_check_remove_cnt_1011021 = 0;
                            } else {
                                if (nedd_check_remove_1011021 || must_check_remove_cnt_1011021 < 3) {
                                    must_check_remove_cnt_1011021++;
                                    ec_remove("1011021", 3);
				}
                            }
                            if(ins_tx4.inss_error_status&(1<<7))
                            {
				                nedd_check_remove_1011022 = ec_add("1011022", "other imu error", "parse", 3);
                                must_check_remove_cnt_1011022 = 0;
                            } else {
                                if (nedd_check_remove_1011022 || must_check_remove_cnt_1011022 < 3) {
                                    must_check_remove_cnt_1011022++;
                                    ec_remove("1011022", 3);
				}
                            }
                            // wheel_speed
                            // if(ins_tx4.inss_error_status&(1<<8))
                            // {
                            //     ec_add("1141015");
                            // } else {
                            //     ec_remove("1141015");
                            // }

                            break;
                        case 0x02020005:
                            ins_tx5.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx5.Print("ins_tx5");
                            }
                            loc_fusion_parse_.yaw = ins_tx5.ins_heading_angle;
                            loc_fusion_parse_.pitch = ins_tx5.ins_pitch_angle;
                            loc_fusion_parse_.roll = ins_tx5.ins_roll_angle;

                            break;
                        case 0x02020006:
                            ins_tx6.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx6.Print("ins_tx6");
                            }
                            if (debug_ & 32) {
                                ins_tx6.Print("ins_tx6");
                            }
                            loc_fusion_parse_.height = ins_tx6.ins_locat_height;
                            loc_fusion_parse_.inss_ts_s = ins_tx6.ins_time;

                            loc_fusion_parse_.week = sensor_imu_parse_.week;
                            loc_fusion_parse_.utc_time = sensor_imu_parse_.utc_time;
                            if (debug_ & 4) {
                                printf("loc_fusion_parse's week is: %d, utc_time is: %.3f, ins_time is: %ld\n",
                                       gps_ts.week, gps_ts.tow, ins_tx6.ins_time);
                            }

                            break;
                        case 0x02020007:
                            ins_tx7.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx7.Print("ins_tx7");
                            }
                            if (debug_ & 32) {
                                ins_tx7.Print("ins_tx7");
                            }
                            if(loc_fusion_parse_.lat<360 && loc_fusion_parse_.lon<360 && loc_fusion_parse_.v>-1)
                            {
                                //double calculateDistance(double lat1, double lon1, double lat2, double lon2)
                                diff = calculateDistance(loc_fusion_parse_.lat, loc_fusion_parse_.lon, ins_tx7.ins_latitude, ins_tx7.ins_longitude);
                                if(loc_fusion_parse_.v>0.01)
                                {
                                    if(diff > loc_fusion_parse_.v*0.01*2) //moving, loc diff > v*0.01*2
                                    {
                                        nedd_check_remove_1011006 = ec_add("1011006", "ins loc fluctuation", "parse", 3);
                                        must_check_remove_cnt_1011006 = 0;
                                    }
                                    else
                                    {
                                        if (nedd_check_remove_1011006 || must_check_remove_cnt_1011006 < 3) {
                                            must_check_remove_cnt_1011006++;
                                            ec_remove("1011006", 3);
                                        }
                                    }
                                }
                                else
                                {
                                    if(diff > 0.3) // motionless, loc diff > 30cm
                                    {
                                        nedd_check_remove_1011006 = ec_add("1011006", "ins loc fluctuation", "parse", 3);
                                        must_check_remove_cnt_1011006 = 0;
                                    }
                                    else
                                    {
                                        if (nedd_check_remove_1011006 || must_check_remove_cnt_1011006 < 3) {
                                            must_check_remove_cnt_1011006++;
                                            ec_remove("1011006", 3);
                                        }
                                    }
                                }

                            }
                            loc_fusion_parse_.lat = ins_tx7.ins_latitude;
                            loc_fusion_parse_.lon = ins_tx7.ins_longitude;

                            break;
                        case 0x02020008:
                            ins_tx8.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx8.Print("ins_tx8");
                            }
                            if (debug_ & 32) {
                                ins_tx8.Print("ins_tx8");
                            }
                            loc_fusion_parse_.east_speed = ins_tx8.ins_east_speed;
                            loc_fusion_parse_.north_speed = ins_tx8.ins_north_speed;
                            loc_fusion_parse_.up_speed = ins_tx8.ins_ground_speed;
                            loc_fusion_parse_.v =
                                sqrt(ins_tx8.ins_east_speed * ins_tx8.ins_east_speed +
                                     ins_tx8.ins_north_speed * ins_tx8.ins_north_speed +
                                     ins_tx8.ins_ground_speed * ins_tx8.ins_ground_speed); // ins_tx8.ins_ground_speed;

                            break;
                        case 0x02020009:
                            ins_tx9.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx9.Print("ins_tx9");
                            }
                            loc_fusion_parse_.yaw_std = ins_tx9.ins_std_heading;
                            loc_fusion_parse_.lat_std = ins_tx9.ins_std_lat;
                            loc_fusion_parse_.height_std = ins_tx9.ins_std_height;
                            loc_fusion_parse_.lon_std = ins_tx9.ins_std_lon;

                            loc_fusion_parse_.ins_cnt++; // recv all ins's can_msg
                            change_ptr->SetLocFusion(loc_fusion_parse_);
                            if (debug_ & 512) {
                                printf("get ins ts: %.3f\n", start_ts);
                            }

                            break;
                        // gnss
                        case 0x02030001:
                            ins_tx10.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx10.Print("ins_tx10");
                            }
                            gnss_parse_.recv_stat = ins_tx10.gnss_state;
                            gnss_parse_.heading = ins_tx10.gnss_heading;
                            gnss_parse_.pitch = ins_tx10.gnss_pitch;
                            gnss_parse_.week = 1;
                            // if (0 == ins_tx10.gnss_state) {
                            //     ec_add("1011003", "can not recv beidou data", 3);
                            // } else {
                            //     ec_remove("1011003", 3);
                            // }

                            break;
                        case 0x02030002:
                            ins_tx11.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx11.Print("ins_tx11");
                            }
                            gnss_parse_.height = ins_tx11.gnss_locate_height;
                            // gnss_parse_.week = ins_tx11.gnss_week;

                            break;
                        case 0x02030003:
                            ins_tx12.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx12.Print("ins_tx12");
                            }
                            if (debug_ & 32) {
                                ins_tx12.Print("ins_tx12");
                            }
                            if(loc_fusion_parse_.lat<360 && loc_fusion_parse_.lon<360 && gnss_parse_.lat<360 && gnss_parse_.lon<360)
                            diff = calculateDistance(loc_fusion_parse_.lat, loc_fusion_parse_.lon, ins_tx12.gnss_latitude, ins_tx12.gnss_longitude);

                            if(diff > 0.5) // motionless, loc_loc-gps_loc diff > 50cm
                            {
                                nedd_check_remove_1011004 = ec_add("1011004", "ins_loc&gps_loc's differ too large", "parse");
                                must_check_remove_cnt_1011004 = 0;
                            }
                            else
                            {
                                if (nedd_check_remove_1011004 || must_check_remove_cnt_1011004 < 1) {
                                    must_check_remove_cnt_1011004++;
                                    ec_remove("1011004");
                                }
                            }

                            gnss_parse_.lat = ins_tx12.gnss_latitude;
                            gnss_parse_.lon = ins_tx12.gnss_longitude;

                            change_ptr->SetGpsData(gnss_parse_);
                            gnss_parse_.week = 0;
                            if (debug_ & 512) {
                                printf("get gnss ts: %.3f\n", start_ts);
                            }
                            if (debug_ & 4) {
                                ins_tx13.Print("ins_tx13");
                                gps_ts =
                                    Ymdhms2GpsTs::utcToGps(ins_tx13.utc_year, ins_tx13.utc_month, ins_tx13.utc_day, ins_tx13.utc_hour,
                                             ins_tx13.utc_min, ins_tx13.utc_sec, ins_tx13.utc_msec);
                                gettimeofday(&tv, nullptr);
                                cur_ts_s = tv.tv_sec + tv.tv_usec / 1000000.0;
                                diff_with_sys_ms_ = (cur_ts_s - NavboxUtc2Unix(gps_ts.week, gps_ts.tow)) * 1000;
                                printf(
                                    "gnss_parse's week is: %d, after Ymdhms2GpsTs::utcToGps convert, week is: %d, utc_time is: %.3f, "
                                    "cur_ts is: %.3f, diff_with_sys is: %d ms\n",
                                    gnss_parse_.week, gps_ts.week, gnss_parse_.utc_time, cur_ts_s, diff_with_sys_ms_);
                            }


                            break;
                        case 0x02030004:
                            ins_tx13.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx13.Print("ins_tx13");
                            }
                            if (debug_ & 32) {
                                ins_tx13.Print("ins_tx13");
                            }

                            if (use_sys_time_) {
                                cur_ms = Ymdhms2GpsTs::getCurrentTimeWithMillis();
                                gps_ts = Ymdhms2GpsTs::utcToGps(cur_ms);
                                // gnss_parse_.week = gps_ts.week;

                                gettimeofday(&tv, nullptr);
                                cur_ts_s = tv.tv_sec + tv.tv_usec / 1000000.0;
                                gnss_parse_.utc_time = cur_ts_s;// gps_ts.tow;
                            } else {
                                // gps_ts = Ymdhms2GpsTs::utcToGps(ins_tx13.utc_year, ins_tx13.utc_month, ins_tx13.utc_day,
                                // ins_tx13.utc_hour, ins_tx13.utc_min, ins_tx13.utc_sec, ins_tx13.utc_msec);
                                // gnss_parse_.week = ins_tx11.gnss_week;
                                gnss_parse_.utc_time = sensor_imu_parse_.utc_time;
                                // gnss_parse_.week = gps_ts.week;
                                // gnss_parse_.utc_time = gps_ts.tow;
                            }

                            gnss_parse_.gnss_cnt++; // recv all ins's can_msg

                            break;
                        case 0x02040001:
                            ins_tx14.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx14.Print("ins_tx14");
                            }
                            gnss_parse_.hdop = ins_tx14.gnss_hdop;
                            gnss_parse_.gdop = ins_tx14.gnss_gdop;
                            gnss_parse_.pdop = ins_tx14.gnss_pdop;
                            gnss_parse_.track_angle = ins_tx14.gnss_track_true;

                            break;
                        case 0x02040002:
                            ins_tx15.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 2) {
                                ins_tx15.Print("ins_tx15");
                            }
                            gnss_parse_.horizontal_spd = ins_tx15.gnss_hor_speed;
                            gnss_parse_.vertical_spd = ins_tx15.gnss_ver_speed;
                            gnss_parse_.mileage = ins_tx15.mileage;

                            break;

                        // cablic para
                        case 0x02010001:
                            ins_tx16.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 8) {
                                ins_tx16.Print("ins_tx16");
                            }
                            break;
                        case 0x02010002:
                            ins_tx17.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 8) {
                                ins_tx17.Print("ins_tx17");
                            }
                            break;

                        // wheel_speed
                        case 0x289:
                            wheel_speed.GetFromCanFrame(can_frame_prepare.data, can_frame_prepare.data_len);
                            if (debug_ & 1024) {
                                wheel_speed.Print("wheel_speed");
                            }
                            wheel_speed_parse_.wheel_speed_cnt++; // recv all ins's can_msg
                            //左前和左后填充左
                            wheel_speed_parse_.left_rear_speed = wheel_speed.m1_speed;
                            wheel_speed_parse_.left_front_speed = wheel_speed.m1_speed;
                            //右前和右后 填充右
                            wheel_speed_parse_.right_rear_speed = wheel_speed.m2_speed;
                            wheel_speed_parse_.right_front_speed = wheel_speed.m2_speed;

                            wheel_speed_parse_.week = sensor_imu_parse_.week;
                            wheel_speed_parse_.utc_time = sensor_imu_parse_.utc_time;

                            change_ptr->SetWheelSpeedData(wheel_speed_parse_);
                            break;
                    }
                }
                gettimeofday(&tv_data, nullptr);
                check_ts = tv_data.tv_sec + tv_data.tv_usec / 1000000.0;
                if ((check_ts - start_ts) > 3.0) {
                    nedd_check_remove_1011003 = ec_add("1011003", "lost data exceed 3s", "parse");
                    must_check_remove_cnt_1011003 = 0;
                }

                if (debug_ & 16) {
                    printf("can_msg_q_.size() is: %d\n", can_msg_q_.size());
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
        }).detach();
    }

public:
    // 地球平均半径6371.0（单位：km），如需米则改为 6371000
    const double EARTH_RADIUS = 6371000;

    SocketCanConfig can_config_;
    SocketCanConfig can_config_ws_;
    int             debug_ = 0;
    bool            record_data_ = false;
    bool            use_sys_time_ = false;
    bool            use_raw_data_ = false;
    uint64_t        start_time_stamp = 0; // ms
    uint64_t        end_time_stamp = 0;
    uint64_t        cloud_time_stamp = 0;
    int             diff_with_sys_ms_ = 0; // bd21 gnss_data's ts diff with sys_ts when recv, sys-gnss
private:
    UserLockQueue<SocketCanFrame> can_msg_q_;
    avos::BD21::InsTx1        ins_tx1;
    avos::BD21::InsTx2        ins_tx2;
    avos::BD21::InsTx3        ins_tx3;
    avos::BD21::InsTx4        ins_tx4;
    avos::BD21::InsTx5        ins_tx5;
    avos::BD21::InsTx6        ins_tx6;
    avos::BD21::InsTx7        ins_tx7;
    avos::BD21::InsTx8        ins_tx8;
    avos::BD21::InsTx9        ins_tx9;
    avos::BD21::InsTx10       ins_tx10;
    avos::BD21::InsTx11       ins_tx11;
    avos::BD21::InsTx12       ins_tx12;
    avos::BD21::WheelSpeed289 wheel_speed;

public:
    avos::BD21::InsTx13 ins_tx13;
    avos::BD21::InsTx14 ins_tx14;
    avos::BD21::InsTx15 ins_tx15;
    avos::BD21::InsTx16 ins_tx16;
    avos::BD21::InsTx17 ins_tx17;

private:
    avos::BD21::LocFusionData loc_fusion_parse_;
    avos::BD21::SensorImuData sensor_imu_parse_;
    avos::BD21::GnssData      gnss_parse_;
    avos::BD21::WheelSpeedData wheel_speed_parse_;

public:
    int sensor_id_ = 0;

private:
    typedef struct ErrCodeState
    {
        uint8_t normal_cnt;
        uint8_t abnormal_cnt;
        uint64_t normal_ts;//ms
        uint64_t abnormal_ts;//ms
        bool     last_state_is_normal;
        bool     last_state_is_abnormal;
        ErrCodeState()
        {
            normal_cnt = 0;
            abnormal_cnt = 0;
            normal_ts = 0;
            abnormal_ts = 0;
            last_state_is_normal = false;
            last_state_is_abnormal = false;
        }
    }ErrCodeState;
    std::map<std::string, ErrCodeState> ec_map; //<ecc_code, state>

public:
    bool ec_add(const std::string &ec, const std::string &extar_info, const std::string &step="init", uint8_t threshold=1) {
        static timeval tv = {};
        gettimeofday(&tv, nullptr);

        auto ptr = ec_map.find(ec);
        if (ptr == ec_map.end()) {
            ec_map[ec] = ErrCodeState();
        }
        ec_map[ec].normal_cnt = 0;
        ec_map[ec].abnormal_cnt++;
        ec_map[ec].abnormal_ts = tv.tv_sec*1000 + tv.tv_usec / 1000.0;
        if((ec_map[ec].abnormal_cnt >= threshold)&&(!ec_map[ec].last_state_is_abnormal))
        {
            ec_map[ec].last_state_is_normal =  false;
            ec_map[ec].last_state_is_abnormal = true;

            std::string info = std::to_string(ec_map[ec].abnormal_ts)+" ms: "+extar_info;
            ApError(step) << "ec_add: " << ec << ", extra_info: " << info;
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, info);
        }
        return ec_map[ec].last_state_is_abnormal; // true: need check if remove; false: no need check
    }

    void ec_remove(const std::string &ec, uint8_t threshold=1) {
        static timeval tv = {};
        gettimeofday(&tv, nullptr);

        auto ptr = ec_map.find(ec);
        if (ptr == ec_map.end()) {
            ec_map[ec] = ErrCodeState();
        }
        ec_map[ec].abnormal_cnt = 0;
        ec_map[ec].normal_cnt++;
        ec_map[ec].normal_ts = tv.tv_sec*1000 + tv.tv_usec / 1000.0;
        if((ec_map[ec].normal_cnt >= threshold)&&(!ec_map[ec].last_state_is_normal))
        {
            ec_map[ec].last_state_is_normal =  true;
            ec_map[ec].last_state_is_abnormal = false;

            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
        }
    }

    void ec_clear() {
        for (const auto &ec : ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        ec_map.clear();
    }
};

#endif
