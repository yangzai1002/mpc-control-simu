#ifndef __XCG1230_DRIVER_BY_UDP__
#define __XCG1230_DRIVER_BY_UDP__

#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

#include <condition_variable>
#include <functional>
#include <iomanip>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "CGI230UdpProtocol.hpp"
#include "DatetimeUtil.hpp"
#include "HexUtils.hpp"
#include "LocationInfoHandler.hpp"
#include "SafeQueue.hpp"
#include "SimpleSocketBuffer.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "config_loader/custom_stack.h"
#include "geometry_tools.hpp"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"

namespace CGI230DriverByUdp {
    class CGI230Ec {
    public:
        CGI230Ec() : _last_lat(0), _last_lon(0), _last_speed(0), _last_time(0), _last_frame_time_ms(0) {
        }

        std::uint32_t ec_add(const std::string& ec) {
            std::lock_guard<std::mutex> lock(_mtx);
            auto                        ptr = _ec_map.find(ec);
            if (ptr == _ec_map.end()) {
                _ec_map[ec] = 0;
            }
            ptr = _ec_map.find(ec);
            if (ptr == _ec_map.end()) {
                return 0;
            }

            ++ptr->second;
            if (ptr->second % 20 == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(ec);
            }
            return ptr->second;
        }

        void ec_remove(const std::string& ec) {
            std::lock_guard<std::mutex> lock(_mtx);
            if (_ec_map.find(ec) != _ec_map.end()) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
                _ec_map.erase(ec);
            }
        }

        void ec_clear() {
            std::lock_guard<std::mutex> lock(_mtx);
            for (const auto& ec : _ec_map) {
                FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
            }
            _ec_map.clear();
        }

        // void check_gps_steady(const std::string& ec, double lat, double lon, double speed, double time) {
        //     if (_last_lon != 0 && _last_lat != 0 && _last_speed != 0) {
        //         auto distance = app_common::::gps_real_distance(lat, lon, _last_lat, _last_lon);
        //         auto estimated_distance = _last_speed * (time - _last_time) * 5;
        //         ApInfo("cgi230") << "distance: " << distance << ", estimated_distance: " << estimated_distance
        //               << ", last_speed: " << speed << ", last_pos: (" << _last_lat << ", " << _last_lon << ")"
        //               << ", last_time: " << _last_time << ", this_pos(lat, lon): (" << lat << ", " << lon << ")"
        //               << ", this_time: " << time;
        //         if (distance > estimated_distance) {
        //             ec_add(ec);
        //         } else {
        //             ec_remove(ec);
        //         }
        //     }
        //     _last_lat = lat;
        //     _last_lon = lon;
        //     _last_speed = speed;
        //     _last_time = time;
        // }

        // void check_freq(const std::string& ec, std::uint32_t interval_ms) {
        //     auto now = app_common::::DatetimeUtil::current_ms();
        //     if (_last_frame_time_ms != 0) {
        //         if (now - _last_frame_time_ms > interval_ms) {
        //             ec_add(ec);
        //         } else {
        //             ec_remove(ec);
        //         }
        //     }
        //     _last_frame_time_ms = now;
        // }

    private:
        std::mutex                           _mtx;
        std::map<std::string, std::uint32_t> _ec_map;
        double                               _last_lat;
        double                               _last_lon;
        double                               _last_speed;
        double                               _last_time;
        std::uint64_t                        _last_frame_time_ms;
    };

    class CGI230UdpRecvData {
    public:
        CGI230UdpRecvData(std::int32_t port) : _port(port), _fd(-1), _read_buffer(2048) {
            _queue_free_gnss = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafGnssInfo>>>();
            _queue_data_gnss = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafGnssInfo>>>();
            for (int i = 0; i < 10; ++i) {
                auto p = std::make_shared<ara::adsfi::MsgHafGnssInfo>();
                p->header.frameID = "cgi230_can_driver";
                _queue_free_gnss->push(std::move(p));
            }

            _queue_free_imu = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafIMU>>>();
            _queue_data_imu = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafIMU>>>();
            for (int i = 0; i < 10; ++i) {
                auto p = std::make_shared<ara::adsfi::MsgHafIMU>();
                p->imuHeader.frameID = "cgi230_can_driver";
                _queue_free_imu->push(std::move(p));
            }

            _queue_free_fusion = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafLocation>>>();
            _queue_data_fusion = std::make_shared<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafLocation>>>();
            for (int i = 0; i < 10; ++i) {
                auto p = std::make_shared<ara::adsfi::MsgHafLocation>();
                p->header.frameID = "cgi230_can_driver";
                _queue_free_fusion->push(std::move(p));
            }
        }

        ~CGI230UdpRecvData() {
        }

        void start() {
            std::thread([this]() { listen_and_read(); }).detach();
        }

        int GetLocationInfo(std::shared_ptr<ara::adsfi::MsgHafLocation>& location) {
            std::shared_ptr<ara::adsfi::MsgHafLocation> ptr_fusion;
            if (_queue_data_fusion->try_move_pop(ptr_fusion) == false) {
                ApError("cgi230") << "cannot get gps";
                return -1;
            }
            *location = *ptr_fusion;
            _queue_free_fusion->push(std::move(ptr_fusion));
            return 0;
        }

        int GetSensorGps(std::shared_ptr<ara::adsfi::MsgHafGnssInfo>& gps) {
            std::shared_ptr<ara::adsfi::MsgHafGnssInfo> ptr_gnss;
            if (_queue_data_gnss->try_move_pop(ptr_gnss) == false) {
                ApError("cgi230") << "cannot get location";
                return -1;
            }
            *gps = *ptr_gnss;
            _queue_free_gnss->push(std::move(ptr_gnss));
            return 0;
        }

        int GetImu(std::shared_ptr<ara::adsfi::MsgHafIMU>& imu) {
            std::shared_ptr<ara::adsfi::MsgHafIMU> ptr_imu;
            if (_queue_data_imu->try_move_pop(ptr_imu) == false) {
                ApError("cgi230") << "cannot get imu";
                return -1;
            }
            *imu = *ptr_imu;
            _queue_free_imu->push(std::move(ptr_imu));
            return 0;
        }

    private:
        void listener_create() {
            std::string  error;
            sockaddr_in  _this{};
            socklen_t    clen = sizeof(_this);
            std::int32_t port;
            std::int32_t enabled = 1;

            _fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_fd < 0) {
                error = std::string("create socket failed: ") + strerror(errno);
                goto error1;
            }

            if (fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK) < 0) {
                error = std::string("set nonblock failed: ") + strerror(errno);
                goto error2;
            }

            if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0) {
                error = std::string("set reuseaddr failed: ") + strerror(errno);
                goto error2;
            }

            _this.sin_family = AF_INET;
            _this.sin_port = htons(_port);
            _this.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(_fd, reinterpret_cast<sockaddr*>(&_this), sizeof(_this)) < 0) {
                error = std::string("bind failed: ") + strerror(errno);
                goto error2;
            }

            memset(&_this, 0, sizeof(_this));
            if (getsockname(_fd, (sockaddr*)&_this, &clen) < 0) {
                error = std::string("getsockname error with ") + strerror(errno);
                goto error2;
            }
            port = ntohs(_this.sin_port);
            ApInfo("cgi230") << "cgi230 udp port: " << port;
            return;

        error2:
            close(_fd);
            _fd = -1;

        error1:
            throw std::runtime_error(error);
        }

        void listener_close() {
            if (_fd > 0) {
                close(_fd);
                _fd = -1;
            }
        }

        void process_gnss(const std::uint8_t* d, std::size_t s) {
            HCRAWGNSSPVATB gnss{};
            if (s != sizeof(gnss)) {
                ApError("cgi230") << "gnss ignored, data length error: " << s << ", " << sizeof(gnss);
                _ec.ec_add(_ERRORCODE_DATA_CONTENT);
                return;
            }
            _ec.ec_remove(_ERRORCODE_DATA_CONTENT);

            memcpy(&gnss, d, sizeof(gnss));

            std::shared_ptr<ara::adsfi::MsgHafGnssInfo> data = nullptr;
            if (!_queue_free_gnss->try_move_pop(data)) {
                _ec.ec_add(_ERRORCODE_BUFFER_EXHAUSTED);
                ApError("cgi230") << "gnss ignored, no free item";
                return;
            }
            _ec.ec_remove(_ERRORCODE_BUFFER_EXHAUSTED);

            data->header.seq = _seq_gnss++;
            data->second = gps_time_2_utc_time(gnss.header.gps_week, gnss.header.gps_second / 1000.0);
            data->header.timestamp.sec = static_cast<decltype(data->header.timestamp.sec)>(data->second);
            data->header.timestamp.nsec =
                static_cast<decltype(data->header.timestamp.nsec)>((data->second - data->header.timestamp.sec) * 1e9);
            data->longitude = gnss.lon;
            data->latitude = gnss.lat;
            data->elevation = gnss.alt;
            GeoPointGPS  gps(gnss.lon, gnss.lat);
            GeoPointGCCS gccs;
            GeometryTool::GPS2GCCS(gps, gccs);

            data->utmPosition.x = gccs.xg;
            data->utmPosition.y = gccs.yg;
            data->utmPosition.z = gnss.alt;
            data->utmZoneNum = GeometryTool::GetLongZone(gnss.lon);
            // data->utmZoneChar = data->utmZoneNum;

            data->attitude = {};

            data->sdPosition.x = gnss.std_lat;
            data->sdPosition.y = gnss.std_lon;
            data->sdPosition.z = gnss.std_alt;

            data->sdVelocity.x = gnss.std_ve;
            data->sdVelocity.y = gnss.std_vn;
            data->sdVelocity.z = gnss.std_vu;

            data->satUseNum = gnss.nsused;
            data->satInViewNum = gnss.ns;

            data->linearVelocity.x = gnss.ve;
            data->linearVelocity.y = gnss.vn;
            data->linearVelocity.z = gnss.vu;

            data->hDop = gnss.hdop;
            switch (gnss.pos_type) {
                case 16:
                    data->solutionStatus = 1;
                    break;
                case 17:
                    data->solutionStatus = 2;
                    break;
                case 32:
                case 33:
                case 34:
                case 49:
                    data->solutionStatus = 5;
                    break;
                case 48:
                case 50:
                    data->solutionStatus = 4;
                case 0:
                case 18:
                case 19:
                default:
                    data->solutionStatus = 0;
                    break;
            }

            _queue_data_gnss->push(std::move(data));
            _leap_s = gnss.leaps;

            ApInfo("cgi230") << "gnss leap_s: " << _leap_s << ", week: " << gnss.header.gps_week
                             << ", second: " << gnss.header.gps_second;

            _num_satellite = gnss.nsused;
        }

        void process_imu(const std::uint8_t* d, std::size_t s) {
            HCRAWIMUVB imu{};
            if (s != sizeof(imu)) {
                ApError("cgi230") << "imu ignored, data length error: " << s << ", " << sizeof(imu);
                _ec.ec_add(_ERRORCODE_DATA_CONTENT);
                return;
            }
            _ec.ec_remove(_ERRORCODE_DATA_CONTENT);

            memcpy(&imu, d, sizeof(imu));

            std::shared_ptr<ara::adsfi::MsgHafIMU> data = nullptr;
            if (!_queue_free_imu->try_move_pop(data)) {
                _ec.ec_add(_ERRORCODE_BUFFER_EXHAUSTED);
                ApError("cgi230") << "imu ignored, no free item";
                return;
            }
            _ec.ec_remove(_ERRORCODE_BUFFER_EXHAUSTED);

            data->imuHeader.seq = _seq_imu++;
            double t = gps_time_2_utc_time(imu.week, imu.second);
            data->imuHeader.timestamp.sec = static_cast<decltype(data->imuHeader.timestamp.sec)>(t);
            data->imuHeader.timestamp.nsec =
                static_cast<decltype(data->imuHeader.timestamp.nsec)>((t - data->imuHeader.timestamp.sec) * 1e9);

            data->angularVelocity.x = imu.gyro_x;
            data->angularVelocity.y = imu.gyro_y;
            data->angularVelocity.z = imu.gyro_z;

            data->linearAcceleration.x = imu.acc_x;
            data->linearAcceleration.y = imu.acc_y;
            data->linearAcceleration.z = imu.acc_z;

            std::uint8_t tmp_status {};
            static_assert(sizeof(tmp_status)==sizeof(imu.ErrStatus), "size mismatch");
            memcpy(&tmp_status, &imu.ErrStatus, sizeof(imu.ErrStatus));
            data->imuStatus = tmp_status;
            data->temperature = imu.temp;

            _queue_data_imu->push(std::move(data));
        }

        void process_fusion(const std::uint8_t* d, std::size_t s) {
            HCINSPVATB fusion{};
            if (s != sizeof(fusion)) {
                ApInfo("cgi230") << "fusion ignored, data length error: " << s << ", " << sizeof(fusion);
                _ec.ec_add(_ERRORCODE_DATA_CONTENT);
                return;
            }

            memcpy(&fusion, d, s);

            std::shared_ptr<ara::adsfi::MsgHafLocation> data = nullptr;
            if (!_queue_free_fusion->try_move_pop(data)) {
                _ec.ec_add(_ERRORCODE_BUFFER_EXHAUSTED);
                ApError("cgi230") << "fusion ignored, no free item";
                return;
            }
            _ec.ec_remove(_ERRORCODE_BUFFER_EXHAUSTED);

            static long long loc_fusion_seq = 0;
            data->header.seq = loc_fusion_seq++;
            // ApInfo("cgi230") << "fusion, week: " << fusion.week << ", second: " << fusion.second;
            double t = gps_time_2_utc_time(fusion.week, (std::uint32_t)fusion.second);
            data->header.timestamp.sec = static_cast<decltype(data->header.timestamp.sec)>(t);
            data->header.timestamp.nsec =
                static_cast<decltype(data->header.timestamp.nsec)>((t - data->header.timestamp.sec) * 1e9);

            data->locationState = fusion.stat.state_ins;

            data->llh.x = fusion.lon;
            data->llh.y = fusion.lat;
            data->llh.z = fusion.alt;

            GeoPointGPS  gps(fusion.lon, fusion.lat);
            GeoPointGCCS gccs;
            GeometryTool::GPS2GCCS(gps, gccs);
            data->pose.pose.position.x = gccs.xg;
            data->pose.pose.position.y = gccs.yg;
            data->pose.pose.position.z = fusion.alt;

            data->pose.pose.orientation.x = fusion.roll;
            data->pose.pose.orientation.y = fusion.pitch;
            auto cur_yaw = 90 + fusion.yaw;
            if (cur_yaw > 180)
                cur_yaw -= 360;
            if (cur_yaw < -180)
                cur_yaw += 360;
            ApInfo("cgi230") << fusion.yaw << ", " << cur_yaw;
            // data->pose.pose_euler.rotation.yaw = cur_yaw;
            data->pose.pose.orientation.z = cur_yaw;

            data->velocity.twist.linear.x = fusion.vn;
            data->velocity.twist.linear.y = fusion.ve;
            data->velocity.twist.linear.z = fusion.vu;

            data->velocity.twist.angular.x = fusion.vgyro_x;
            data->velocity.twist.angular.y = fusion.vgyro_y;
            data->velocity.twist.angular.z = fusion.vgyro_z;

            data->acceleration.accel.linear.x = fusion.vacc_x;
            data->acceleration.accel.linear.y = fusion.vacc_y;
            data->acceleration.accel.linear.z = fusion.vacc_z;

            // data->header.seq = _seq_fusion++;
            // data->header.time = gps_time_2_utc_time(fusion.week, fusion.second);

            // timeval tv{};
            // gettimeofday(&tv, nullptr);
            // auto sys = tv.tv_sec + tv.tv_usec / 1000000.0;
            // auto diff = fabs(sys - data->header.time);
            // if (diff > 30.0) {
            //     _ec.ec_add(_ERRORCODE_TIME_DIFF_LARGE);
            // } else {
            //     _ec.ec_remove(_ERRORCODE_TIME_DIFF_LARGE);
            // }

            // if (diff >= 0.050) { // TODO: 152 联调时专门为 3 号车加的，后续要去掉
            //     timeval stv{};
            //     stv.tv_sec = (time_t)data->header.time;
            //     stv.tv_usec = (data->header.time - stv.tv_sec) * 1000000;
            //     settimeofday(&stv, nullptr);
            //     ApInfo("cgi230") << "settimeofday, gps: " << std::setprecision(20) << data->header.time << ", sys: " << sys;
            // }

            // data->llh.lat = fusion.lat;
            // data->llh.lon = fusion.lon;
            // data->llh.height = fusion.alt;

            // if (std::isnan(fusion.lon) == false && std::isnan(fusion.lat) == false &&
            //     std::isnan(fusion.heading) == false) {
            //     GeoPointGPS  temp_point_gps(fusion.lon, fusion.lat, fusion.heading);
            //     GeoPointGCCS temp_point_gccs;
            //     GeometryTool::GPS2GCCS(temp_point_gps, temp_point_gccs);
            //     data->pose.pose_euler.position.x = temp_point_gccs.xg;
            //     data->pose.pose_euler.position.y = temp_point_gccs.yg;
            //     data->pose.pose_euler.position.z = fusion.alt;
            // }
            // data->pose.pose_euler.rotation.roll = fusion.roll;
            // data->pose.pose_euler.rotation.pitch = fusion.pitch;
            // auto cur_yaw = 90 + fusion.yaw;
            // if (cur_yaw > 180)
            //     cur_yaw -= 360;
            // if (cur_yaw < -180)
            //     cur_yaw += 360;
            // ApInfo("cgi230") << std::setprecision(20) << fusion.yaw << ", " << cur_yaw;
            // data->pose.pose_euler.rotation.yaw = cur_yaw;

            // data->twist.velocity.x = fusion.vn;
            // data->twist.velocity.y = fusion.ve;
            // data->twist.velocity.z = fusion.vu;
            // data->twist.angular_velocity.x = fusion.vgyro_x;
            // data->twist.angular_velocity.y = fusion.vgyro_y;
            // data->twist.angular_velocity.z = fusion.vgyro_z;

            // data->acceleration.x = fusion.vacc_x;
            // data->acceleration.y = fusion.vacc_y;
            // data->acceleration.z = fusion.vacc_z;

            // data->v = fusion.speed;

            // data->nav_status.utc_time = fusion.second;
            // /*
            // GNSS状态:
            // 0:不定位不定向;
            // 1:单点定位定向;
            // 2:伪距差分定位定向;
            // 3:组合推算;
            // 4:RTK 稳定解定位定向;
            // 5:RTK浮点解定位定向;
            // 6:单点定位不定向;
            // 7:伪距差分定位不定向;
            // 8:RTK稳定解定位不定向;
            // 9:RTK浮点解定位不定向
            // */
            // // 与我们的定义差不多，直接使用
            // data->nav_status.gps_status = fusion.stat.state_gnss;
            // data->nav_status.pos_status = fusion.stat.state_gnss;
            // data->nav_status.init_status = fusion.stat.state_ins;
            // data->nav_status.sate_num = _num_satellite;

            if (fusion.stat.state_gnss != 4) {
                _ec.ec_add(_ERRORCODE_NOT_FIX);
            } else {
                _ec.ec_remove(_ERRORCODE_NOT_FIX);
            }

            /*
            组合状态:
            0:初始化;
            1:卫导模式;
            2:组合导航模式;
            3:纯惯导模式
            */
            if (fusion.stat.state_ins != 2) {
                _ec.ec_add(_ERRORCODE_NOT_INTEGRATED_NAVIGATION);
            } else {
                _ec.ec_remove(_ERRORCODE_NOT_INTEGRATED_NAVIGATION);
            }

            // _ec.check_gps_steady(_ERRORCODE_POS_JUMP, fusion.lat, fusion.lon, fusion.speed, data->header.time);
            // _ec.check_freq(_ERRORCODE_FREQ_ABNORMAL, 15);

            // data->dr.week = fusion.week;
            // data->dr.utc_time = fusion.second;
            // data->dr.dr_time = 1000.0; // refer  asensing

            _queue_data_fusion->push(std::move(data));
        }

        void listen_and_read() {
            while (true) {
                // 创建设备
                if (_fd < 0) {
                    try {
                        listener_create();
                        _ec.ec_remove(_ERRORCODE_DEVICE_ERROR);
                    } catch (const std::runtime_error& e) {
                        _ec.ec_add(_ERRORCODE_DEVICE_ERROR);
                        ApError("cgi230") << e.what();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        continue;
                    }
                }

                try {
                    auto n = _read_buffer.poll_and_read(_fd, 200);
                    _ec.ec_remove(_ERRORCODE_READ);
                } catch (const std::runtime_error& e) {
                    _ec.ec_add(_ERRORCODE_READ);
                    ApError("cgi230") << e.what();
                    listener_close();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }

                auto untouched = _read_buffer.untouched_data();
                while (untouched.second >= sizeof(HCHeader)) {
                    auto header = reinterpret_cast<const HCHeader*>(untouched.first);
                    if (0 != memcmp(header->sync, "\xAA\xCCHC", sizeof(header->sync))) {
                        ApError("cgi230") << "sync code error: ";
                        _ec.ec_add(_ERRORCODE_DATA_CONTENT);
                        _read_buffer.reset();
                        break;
                    }
                    _ec.ec_remove(_ERRORCODE_DATA_CONTENT);

                    // 消息实际长度，不包含头和校验
                    auto explen = header->length + sizeof(HCHeader) + 4;
                    if (untouched.second < explen) {
                        break;
                    }
                    // TODO : 校验 CRC

                    if (0x0101 == header->msgid) {
                        process_gnss(untouched.first, explen);
                    } else if (0x0108 == header->msgid) {
                        process_imu(untouched.first, explen);
                    } else if (0x0201 == header->msgid) {
                        process_fusion(untouched.first, explen);
                    } else {
                        ApInfo("cgi230") << "receive other msgid " << header->msgid;
                    }
                    _read_buffer.touch(explen);
                    untouched = _read_buffer.untouched_data();
                }
            }
        }

    private:
        static const constexpr char* _ERRORCODE_DEVICE_ERROR = "1011001";  // 打开设备失败
        static const constexpr char* _ERRORCODE_READ = "1011002";          // 读取设备失败
        static const constexpr char* _ERRORCODE_MISS_KEY_DATA = "1011003"; // 缺少关键数据
        static const constexpr char* _ERRORCODE_DATA_CONTENT = "1011004"; // 数据内容错误（如CRC校验不通过等）
        static const constexpr char* _ERRORCODE_NOT_FIX = "1011005";      // 非固定解
        static const constexpr char* _ERRORCODE_POS_JUMP = "1011006";                        // 定位跳跃
        static const constexpr char* _ERRORCODE_MAIN_ANTENNA_OPEN_CIRCUIT = "1011007";       // 主天线断路
        static const constexpr char* _ERRORCODE_MAIN_ANTENNA_CLOSE_CIRCUIT = "1011008";      // 主天线短路
        static const constexpr char* _ERRORCODE_AUXILIARY_ANTENNA_OPEN_CIRCUIT = "1011009";  // 辅天线断路
        static const constexpr char* _ERRORCODE_AUXILIARY_ANTENNA_CLOSE_CIRCUIT = "1011010"; // 辅天线短路
        static const constexpr char* _ERRORCODE_TEMPERATURE_HIGH = "1011011";                // 温度过高
        static const constexpr char* _ERRORCODE_TEMPERATURE_LOW = "1011012";                 // 温度过低
        static const constexpr char* _ERRORCODE_NOT_INTEGRATED_NAVIGATION = "1011013";       // 非组合导航模式
        static const constexpr char* _ERRORCODE_FREQ_ABNORMAL = "1011014";                   // 数据频率异常
        static const constexpr char* _ERRORCODE_BUFFER_EXHAUSTED = "1012015";                // 数据缓存耗尽
        static const constexpr char* _ERRORCODE_TIME_DIFF_LARGE = "1012016"; // 数据时间与系统时间相差太大

        const std::int32_t _port;

        std::int32_t _fd;

        app_common::SimpleSocketBuffer _read_buffer;

        CGI230Ec _ec;

    public:
        std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafGnssInfo>>> _queue_free_gnss;
        std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafGnssInfo>>> _queue_data_gnss;

        std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafIMU>>> _queue_free_imu;
        std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafIMU>>> _queue_data_imu;

        std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafLocation>>> _queue_free_fusion;
        std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgHafLocation>>> _queue_data_fusion;

    private:
        static constexpr const std::int32_t HCFDRAWGNSSPVAT = 0x385;
        static constexpr const std::int32_t HCFDRAWIMUVB = 0x387;
        static constexpr const std::int32_t HCFDINSPVATB = 0x388;
        static constexpr const std::int32_t HCFDERRINFOB = 0x38c;

        std::int32_t _num_satellite = 0;
        std::int32_t _leap_s = 0; // 闰秒

        std::int64_t _last_recv_385;
        std::int64_t _last_recv_387;
        std::int64_t _last_recv_388;
        std::int64_t _last_recv_38c;

        long long _seq_gnss = 0;
        long long _seq_imu = 0;
        long long _seq_fusion = 0;

        double gps_time_2_utc_time(double gps_week, double gps_time) {
            const double WEEKS = 7 * 24 * 3600.0;
            // GPS时间起始点为1980年1月6日，UTC时间为1980年1月6日 0时
            return (315964800 - _leap_s) + gps_week * WEEKS + gps_time;
        }

        double gps_time_2_utc_time(double gps_week, std::uint32_t gps_time) {
            auto   micro = app_common::DatetimeUtil::current_millisecond();
            double a = gps_time * 1000.0 + micro % 1000;
            return gps_time_2_utc_time(gps_week, a / 1000.0);
        }
    };
} // namespace CGI230DriverByUdp

#endif
