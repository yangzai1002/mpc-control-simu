#include <arpa/inet.h>
#include <dirent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

#include "JsonUtils.hpp"
#include "StringUtils.hpp"
#include "SystemMonitor.hpp"
#include "custom_stack.h"
#include "ap_log/ap_log_interface.h"

SystemMonitor::SystemMonitor() : _stopped(false) {
    auto ptr = CustomStack::Instance();

    std::string prefix;
    auto        full_path = prefix + "resource/monitor/board_type";
    if (!ptr->GetProjectConfigValue(full_path, _board_type) || _board_type.empty()) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    _board_type = app_common::StringUtils::to_lower(_board_type);
    ApInfo("system_monitor") << full_path << ": " << _board_type;

    full_path = prefix + "resource/monitor/ping_ip";
    if (!ptr->GetProjectConfigValue(full_path, _ping_ip) || _ping_ip.empty()) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _ping_ip;

    full_path = prefix + "resource/monitor/threshold/threshold_cpu_per_load";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_cpu_per_load) || _threshold_cpu_per_load <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_cpu_per_load;

    full_path = prefix + "resource/monitor/threshold/threshold_cpu_total_load";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_cpu_total_load) || _threshold_cpu_total_load <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_cpu_total_load;

    full_path = prefix + "resource/monitor/threshold/threshold_usage_ram";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_usage_ram) || _threshold_usage_ram <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_usage_ram;

    full_path = prefix + "resource/monitor/threshold/check_disk_sys_path";
    if (!ptr->GetProjectConfigValue(full_path, _check_disk_sys_path) || _check_disk_sys_path.empty()) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _check_disk_sys_path;

    full_path = prefix + "resource/monitor/threshold/threshold_usage_disk_sys";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_usage_disk_sys) || _threshold_usage_disk_sys <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_usage_disk_sys;

    full_path = prefix + "resource/monitor/threshold/check_disk_user_path";
    if (!ptr->GetProjectConfigValue(full_path, _check_disk_user_path) || _check_disk_user_path.empty()) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _check_disk_user_path;

    full_path = prefix + "resource/monitor/threshold/threshold_usage_disk_user";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_usage_disk_user) || _threshold_usage_disk_user <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_usage_disk_user;

    full_path = prefix + "resource/monitor/threshold/threshold_cpu_temp_upper";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_cpu_temp_upper)) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_cpu_temp_upper;

    full_path = prefix + "resource/monitor/threshold/threshold_cpu_temp_lower";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_cpu_temp_lower) ||
        _threshold_cpu_temp_lower >= _threshold_cpu_temp_upper) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_cpu_temp_lower;

    full_path = prefix + "resource/monitor/threshold/threshold_gpu_temp_upper";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_gpu_temp_upper)) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_gpu_temp_upper;

    full_path = prefix + "resource/monitor/threshold/threshold_gpu_temp_lower";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_gpu_temp_lower) ||
        _threshold_gpu_temp_lower >= _threshold_gpu_temp_upper) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_gpu_temp_lower;

    full_path = prefix + "resource/monitor/threshold/threshold_board_temp_upper";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_board_temp_upper)) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_board_temp_upper;

    full_path = prefix + "resource/monitor/threshold/threshold_board_temp_lower";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_board_temp_lower) ||
        _threshold_board_temp_lower >= _threshold_board_temp_upper) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_board_temp_lower;

    full_path = prefix + "resource/monitor/threshold/ifname";
    if (!ptr->GetProjectConfigValue(full_path, _ifname) || _ifname.empty()) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _ifname;

    full_path = prefix + "resource/monitor/threshold/threshold_tx_bandwidth";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_tx_bandwidth) || _threshold_tx_bandwidth <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_tx_bandwidth;

    full_path = prefix + "resource/monitor/threshold/threshold_rx_bandwidth";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_rx_bandwidth) || _threshold_rx_bandwidth <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_rx_bandwidth;

    full_path = prefix + "resource/monitor/threshold/threshold_usage_shm";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_usage_shm) || _threshold_usage_shm <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_usage_shm;

    full_path = prefix + "resource/monitor/threshold/threshold_usage_sem";
    if (!ptr->GetProjectConfigValue(full_path, _threshold_usage_sem) || _threshold_usage_sem <= 0) {
        _ec.ec_add(_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " error");
    }
    ApInfo("system_monitor") << full_path << ": " << _threshold_usage_sem;

    _ec.ec_remove(_ERRORCODE_CONFIG);

    if (is_mdc610()) {
        _mdc610_info = avos::idpware::CUserMdc610FuncInterface::Instance();
    }

    refresh_sysinfo();
    _last_link_stat = ProcNetDevStats::getStats(_ifname);
    _this_link_stat = ProcNetDevStats::getStats(_ifname);

    _thread = std::thread([this]() { work(); });
}

SystemMonitor::~SystemMonitor() {
    _stopped = true;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void SystemMonitor::work() {
    pthread_setname_np(pthread_self(), "monitor");
    ApInfo("system_monitor") << "work begin ...";
    while (!_stopped) {
        try {
            refresh_sysinfo();
            check_cpu_per_load_high(_threshold_cpu_per_load);

            check_cpu_total_load_high(_threshold_cpu_total_load);

            check_ram_usage_high(_threshold_usage_ram);

            check_disk_space_usage_high(_check_disk_sys_path, _threshold_usage_disk_sys, _ERRORCODE_DISK_SYS_USAGE);
            check_disk_space_usage_high(_check_disk_user_path, _threshold_usage_disk_user, _ERRORCODE_DISK_USER_USAGE);
        } catch (const std::exception& e) {
            ApError("system_monitor") << "ERROR: " << e.what();
        }

        try {
            if (is_mdc610()) {
                check_mdc610_board_temperature_high(_threshold_board_temp_upper, _threshold_board_temp_lower);
            } else {
                check_temperature_high("/sys/class/thermal/thermal_zone0/temp", _threshold_cpu_temp_upper,
                                       _threshold_cpu_temp_lower, _ERRORCODE_CPU_TEMPERATURE);

                check_temperature_high("/sys/class/thermal/thermal_zone1/temp", _threshold_gpu_temp_upper,
                                       _threshold_gpu_temp_lower, _ERRORCODE_GPU_TEMPERATURE);
            }
        } catch (const std::exception& e) {
            ApError("system_monitor") << "ERROR: " << e.what();
        }

        try {
            _last_link_stat = ProcNetDevStats::getStats(_ifname);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            _this_link_stat = ProcNetDevStats::getStats(_ifname);
            check_net_tx_bandwidth_high(_threshold_tx_bandwidth);
            check_net_rx_bandwidth_high(_threshold_rx_bandwidth);
            // check_internet_connection_down(); 2501项目暂时移除这个公网检测逻辑
            _ec.ec_remove(_ERRORCODE_CONNECT);
        } catch (const std::exception& e) {
            _ec.ec_add(_ERRORCODE_CONNECT, _ifname);
            ApError("system_monitor") << "ERROR: " << e.what();
        }

        try {
            _ipc_usage = IPCStats::getUsage();
            check_shared_memory_usage_high(_threshold_usage_shm);
            check_semaphore_usage_high(_threshold_usage_sem);
        } catch (const std::exception& e) {
            ApError("system_monitor") << "ERROR: " << e.what();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    ApInfo("system_monitor") << "work end ...";
}

// 刷新 sysinfo 缓存
void SystemMonitor::refresh_sysinfo() {
    sysinfo(&_cached_sysinfo_);
}

void SystemMonitor::check_cpu_per_load_high(double threshold) {
    int    ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    double load1 = _cached_sysinfo_.loads[0] / 65536.0; // sysinfo loads 是放大 2^16 的整数
    ApInfo("system_monitor") << "CPU per load: " << (load1 / ncpu) << ", threshold: " << threshold;
    if ((load1 / ncpu) > threshold) {
        _ec.ec_add(_ERRORCODE_CPU_LOAD, std::to_string(load1 / ncpu));
    } else {
        _ec.ec_remove(_ERRORCODE_CPU_LOAD);
    }
}

void SystemMonitor::check_cpu_total_load_high(double threshold) {
    ApInfo("system_monitor") << "CPU total load: " << _cached_sysinfo_.loads[0] / 65536.0
                             << ", threshold: " << threshold;
    auto v = _cached_sysinfo_.loads[0] / 65536.0;
    if (v > threshold) {
        _ec.ec_add(_ERRORCODE_BOARD_LOAD, std::to_string(v));
    } else {
        _ec.ec_remove(_ERRORCODE_BOARD_LOAD);
    }
}

// RAM 占用率高
void SystemMonitor::check_ram_usage_high(double threshold) {
    double used = (double)(_cached_sysinfo_.totalram - _cached_sysinfo_.freeram) / _cached_sysinfo_.totalram;
    ApInfo("system_monitor") << "RAM used: " << used * 100.0 << "%, threshold: " << threshold;
    if ((used * 100.0) > threshold) {
        _ec.ec_add(_ERRORCODE_RAM_USAGE, std::to_string(used));
    } else {
        _ec.ec_remove(_ERRORCODE_RAM_USAGE);
    }
}

// 硬盘可用空间过小
void SystemMonitor::check_disk_space_usage_high(const std::string& path, double threshold, const std::string& ec) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        throw std::runtime_error("statvfs error");
    }
    double free = (double)stat.f_bavail * stat.f_frsize;
    double total = (double)stat.f_blocks * stat.f_frsize;
    double ratio = 1.0 - free / total;

    ApInfo("system_monitor") << "Disk usage: " << ratio * 100.0 << "%, threshold: " << threshold;
    if ((ratio * 100.0) > threshold) {
        _ec.ec_add(ec, std::to_string(ratio));
    } else {
        _ec.ec_remove(ec);
    }
}

void SystemMonitor::check_temperature_high(const std::string& path, double threshold_upper, double threshold_lower,
                                           const std::string& ec) {
    std::ifstream fin(path);
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open temperature file: " + path);
    }
    int temp_milli = 0;
    fin >> temp_milli;
    double temp = temp_milli / 1000.0;
    ApInfo("system_monitor") << path << " temperature: " << temp << ", threshold_upper: " << threshold_upper
                             << ", threshold_lower: " << threshold_lower;
    if (temp > threshold_upper || temp < threshold_lower) {
        _ec.ec_add(ec, std::to_string(temp));
    } else {
        _ec.ec_remove(ec);
    }
}

// 网卡 Tx 带宽占用过高（Mbps）
void SystemMonitor::check_net_tx_bandwidth_high(double threshold_mbps) {
    auto diff = _this_link_stat.tx_bytes - _last_link_stat.tx_bytes;
    auto mbps = diff * 8.0 / 1024 / 1024;
    ApInfo("system_monitor") << "Net Tx: " << mbps << " Mbps, threshold: " << threshold_mbps;
    if (mbps > threshold_mbps) {
        _ec.ec_add(_ERRORCODE_NET_TX_BANDWIDTH, std::to_string(mbps));
    } else {
        _ec.ec_remove(_ERRORCODE_NET_TX_BANDWIDTH);
    }
}

// 网卡 Rx 带宽占用过高（Mbps）
void SystemMonitor::check_net_rx_bandwidth_high(double threshold_mbps) {
    auto diff = _this_link_stat.rx_bytes - _last_link_stat.rx_bytes;
    auto mbps = diff * 8.0 / 1024 / 1024;
    ApInfo("system_monitor") << "Net Rx: " << mbps << " Mbps, threshold: " << threshold_mbps;
    if (mbps > threshold_mbps) {
        _ec.ec_add(_ERRORCODE_NET_RX_BANDWIDTH, std::to_string(mbps));
    } else {
        _ec.ec_remove(_ERRORCODE_NET_RX_BANDWIDTH);
    }
}

// 公网连接异常（ping 8.8.8.8）2501项目暂时移除这个公网检测逻辑
// void SystemMonitor::check_internet_connection_down() {
// auto cmd = "ping -c 1 -W 1 " + _ping_ip + " > /dev/null 2>&1";
// int  ret = system(cmd.c_str());
// if (ret != 0) {
//     _ec.ec_add(_ERRORCODE_INTERNET_DOWN);
// } else {
//     _ec.ec_remove(_ERRORCODE_INTERNET_DOWN);
// }
// }

// 共享内存使用过多（统计 /dev/shm 占用）
void SystemMonitor::check_shared_memory_usage_high(size_t threshold_percentage) {
    auto percent = [](size_t used, size_t total) { return total == 0 ? 0.0 : 100.0 * used / total; };
    auto usage = percent(_ipc_usage.shm_used, _ipc_usage.shm_limit);
    ApInfo("system_monitor") << "Shared memory usage: " << usage << "%, threshold: " << threshold_percentage;
    if (usage > threshold_percentage) {
        _ec.ec_add(_ERRORCODE_SHMEM_USAGE, std::to_string(usage));
    } else {
        _ec.ec_remove(_ERRORCODE_SHMEM_USAGE);
    }
}

// 信号量使用过多（统计 ipcs -s 输出行数）
void SystemMonitor::check_semaphore_usage_high(size_t threshold_percentage) {
    auto percent = [](size_t used, size_t total) { return total == 0 ? 0.0 : 100.0 * used / total; };
    auto usage = percent(_ipc_usage.sem_used, _ipc_usage.sem_limit);
    ApInfo("system_monitor") << "Semaphore usage: " << usage << "%, threshold: " << threshold_percentage;
    if (usage > threshold_percentage) {
        _ec.ec_add(_ERRORCODE_SEMPHORA_USAGE, std::to_string(usage));
    } else {
        _ec.ec_remove(_ERRORCODE_SEMPHORA_USAGE);
    }
}

// 文件资源打开过多（/proc/sys/fs/file-nr）
void SystemMonitor::check_open_files_too_many(size_t threshold_count) {
    std::ifstream fin("/proc/sys/fs/file-nr");
    if (!fin.is_open()) {
        return;
    }
    size_t allocated, unused, max;
    fin >> allocated >> unused >> max;
    if (allocated > threshold_count) {
        _ec.ec_add(_ERRORCODE_FD_USAGE, std::to_string(allocated));
    } else {
        _ec.ec_remove(_ERRORCODE_FD_USAGE);
    }
}

void SystemMonitor::check_mdc610_board_temperature_high(double threshold_upper, double threshold_lower) {
    if (_mdc610_info != nullptr) {
        auto str_temperature = _mdc610_info->idpGetBoardTemperature();
        ApInfo("system_monitor") << "temperature string: " << str_temperature;

        try {
            auto        json_obj = nlohmann::json::parse(str_temperature);
            std::string str_soc_temp;
            app_common::JsonUtils::valueFromJson(str_soc_temp, json_obj, "Soc", true);
            auto d_temp = (double)strtold(str_soc_temp.c_str(), nullptr);
            ApInfo("system_monitor") << "temperature: " << d_temp << ", threshold_upper: " << threshold_upper
                                     << ", threshold_lower: " << threshold_lower;
            if (d_temp > threshold_upper || d_temp < threshold_lower) {
                _ec.ec_add(_ERRORCODE_CPU_TEMPERATURE, std::to_string(d_temp));
            } else {
                _ec.ec_remove(_ERRORCODE_CPU_TEMPERATURE);
            }
        } catch (const std::exception& e) {
            ApError("system_monitor") << "temperature string parse error: " << e.what();
        }
    }
}