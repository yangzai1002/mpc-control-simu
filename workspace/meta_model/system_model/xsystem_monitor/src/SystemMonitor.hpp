#ifndef _SYSTEM_MONITOR_HPP_
#define _SYSTEM_MONITOR_HPP_

#include <sys/sysinfo.h>

#include <atomic>
#include <string>
#include <thread>

#include "IPCStats.hpp"
#include "ProcNetDevStats.hpp"
#include "ap_log/ap_log_interface.h"
#include "dvpp_interface/dvpp_protocol/cusermdc610funcinterface.h"
#include "faulthandle_api.hpp"

class SystemMonitor {
public:
    static constexpr const char* _ERRORCODE_RAM_USAGE = "2021001"; //	板卡RAM占用率高	占用率大于90%
    static constexpr const char* _ERRORCODE_DISK_SYS_USAGE =
        "2021002"; //	板卡系统硬盘可使用空间过小	硬盘剩余空间小于10%
    static constexpr const char* _ERRORCODE_DISK_USER_USAGE =
        "2021003"; //	板卡用户硬盘可使用空间过小	硬盘剩余空间小于10%
    static constexpr const char* _ERRORCODE_CPU_TEMPERATURE = "2021004"; //	板卡CPU温度过高	CPU温度大于８０℃
    static constexpr const char* _ERRORCODE_GPU_TEMPERATURE = "2021005"; //	板卡GPU温度过高	GPU温度大于８０℃
    static constexpr const char* _ERRORCODE_NET_TX_BANDWIDTH =
        "2021006"; // 板卡网卡Tx带宽占用过高，发送带宽大于８００Ｍｂ/s
    static constexpr const char* _ERRORCODE_NET_RX_BANDWIDTH =
        "2021007"; // 板卡网卡Rx带宽占用过高，接收带宽大于８００Ｍｂ/s
    static constexpr const char* _ERRORCODE_INTERNET_DOWN = "2021008"; //	板卡公网连接异常
    static constexpr const char* _ERRORCODE_SHMEM_USAGE = "2021009";   //	共享内存使用过多	同判断标准描述
    static constexpr const char* _ERRORCODE_SEMPHORA_USAGE = "2021010"; //	信号量使用过多	同判断标准描述
    static constexpr const char* _ERRORCODE_FD_USAGE = "2021011";   //	文件资源打开过多	同判断标准描述
    static constexpr const char* _ERRORCODE_CPU_LOAD = "2021012";   //	CPU负载高	同判断标准描述
    static constexpr const char* _ERRORCODE_BOARD_LOAD = "2021013"; //	板卡负载高	同判断标准描述

    static constexpr const char* _ERRORCODE_CONFIG = "2022001";  //	配置参数读取失败
    static constexpr const char* _ERRORCODE_CONNECT = "2022002"; //	设备连接失败

    class Ec {
    public:
        Ec() = default;

        std::uint32_t ec_add(const std::string& ec, const std::string& info = "") {
            std::lock_guard<std::mutex> lock(_mtx);
            auto                        ptr = _ec_map.find(ec);
            if (ptr == _ec_map.end()) {
                auto r = _ec_map.insert({ec, 0});
                if (r.second) {
                    ptr = r.first;
                } else {
                    return 0;
                }
            }

            ++ptr->second;
            if (ptr->second % 20 == 1) {
                FaultHandle::FaultApi::Instance()->SetFaultCode(ec, info);
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

    private:
        std::mutex                           _mtx;
        std::map<std::string, std::uint32_t> _ec_map;
    };

public:
    SystemMonitor();

    ~SystemMonitor();

private:
    void work();

    void refresh_sysinfo(); // 刷新 sysinfo 缓存

    void check_cpu_per_load_high(double threshold);

    void check_cpu_total_load_high(double threshold);

    void check_ram_usage_high(double threshold);

    void check_disk_space_usage_high(const std::string& path, double threshold, const std::string& ec);

    void check_temperature_high(const std::string& path, double threshold_upper, double threshold_lower,
                                const std::string& ec);

    void check_net_tx_bandwidth_high(double threshold_mbps);

    void check_net_rx_bandwidth_high(double threshold_mbps);

    // void check_internet_connection_down();2501项目暂时移除这个公网检测逻辑

    void check_shared_memory_usage_high(size_t threshold_usage);

    void check_semaphore_usage_high(size_t threshold_usage);

    void check_open_files_too_many(size_t threshold_usage);

private:
    bool is_mdc610() {
        return "mdc610" == _board_type;
    }
    void check_mdc610_board_temperature_high(double threshold_upper, double threshold_lower);

private:
    std::string _board_type;
    std::string _ping_ip;
    std::string _ifname;
    double      _threshold_cpu_per_load;
    double      _threshold_cpu_total_load;
    double      _threshold_usage_ram;
    std::string _check_disk_sys_path;
    std::string _check_disk_user_path;
    double      _threshold_usage_disk_sys;
    double      _threshold_usage_disk_user;
    double      _threshold_cpu_temp_upper;
    double      _threshold_cpu_temp_lower;
    double      _threshold_gpu_temp_upper;
    double      _threshold_gpu_temp_lower;
    double      _threshold_board_temp_upper;
    double      _threshold_board_temp_lower;
    double      _threshold_tx_bandwidth;
    double      _threshold_rx_bandwidth;
    double      _threshold_usage_shm;
    double      _threshold_usage_sem;

private:
    struct sysinfo _cached_sysinfo_;

    std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> _mdc610_info;

    rtnl_link_stats64 _last_link_stat;
    rtnl_link_stats64 _this_link_stat;

    IPCUsage _ipc_usage;

private:
    Ec               _ec;
    std::atomic_bool _stopped;
    std::thread      _thread;
};

#endif