#ifndef _PROC_NET_DEV_STATS_HPP_
#define _PROC_NET_DEV_STATS_HPP_

#include <linux/if_link.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class ProcNetDevStats {
public:
    ProcNetDevStats() = delete;

    // 获取某个接口的 RX/TX 字节数
    static rtnl_link_stats64 getStats(const std::string& ifname) {
        std::ifstream infile("/proc/net/dev");
        if (!infile.is_open()) {
            throw std::runtime_error("Failed to open /proc/net/dev\n");
        }

        std::string line;
        // Skip first 2 header lines
        std::getline(infile, line);
        std::getline(infile, line);

        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string        iface;
            std::getline(iss, iface, ':');
            trim(iface);

            if (iface != ifname) {
                continue;
            }

            rtnl_link_stats64 stat{};

            iss >> stat.rx_bytes >> stat.rx_packets >> stat.rx_errors >> stat.rx_dropped >> stat.rx_fifo_errors >>
                stat.rx_frame_errors >> stat.rx_compressed >> stat.multicast >> stat.tx_bytes >> stat.tx_packets >>
                stat.tx_errors >> stat.tx_dropped >> stat.tx_fifo_errors >> stat.collisions >> stat.tx_carrier_errors >>
                stat.tx_compressed;

            return stat;
        }

        throw std::runtime_error("Cannot find interfifname");
    }

private:
    static void trim(std::string& s) {
        size_t start = s.find_first_not_of(" ");
        size_t end = s.find_last_not_of(" ");
        if (start == std::string::npos || end == std::string::npos) {
            s = "";
        } else {
            s = s.substr(start, end - start + 1);
        }
    }
};

#endif
