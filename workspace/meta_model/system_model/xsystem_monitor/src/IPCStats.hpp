#ifndef _IPC_STATS_HPP_
#define _IPC_STATS_HPP_
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct IPCUsage {
    size_t shm_used = 0;
    size_t shm_limit = 0;

    size_t sem_used = 0;
    size_t sem_limit = 0;

    size_t msg_used = 0;
    size_t msg_limit = 0;
};

class IPCStats {
public:
    IPCStats() = default;

public:
    static IPCUsage getUsage() {
        IPCUsage usage;

        usage.shm_used = countLines("/proc/sysvipc/shm");
        usage.sem_used = countLines("/proc/sysvipc/sem");
        usage.msg_used = countLines("/proc/sysvipc/msg");

        usage.shm_limit = readKernelVal("/proc/sys/kernel/shmmni");
        usage.sem_limit = readSemmniFromSem(); // 从 sem 中提取第4个值
        usage.msg_limit = readKernelVal("/proc/sys/kernel/msgmni");

        return usage;
    }

    static void printUsage() {
        IPCUsage usage = getUsage();
        auto     percent = [](size_t used, size_t total) { return total == 0 ? 0.0 : 100.0 * used / total; };

        std::cout << "IPC 资源使用情况:\n";
        std::cout << "-------------------------------------\n";
        std::cout << "共享内存段 (shm):   " << usage.shm_used << " / " << usage.shm_limit << "   "
                  << percent(usage.shm_used, usage.shm_limit) << " %\n";
        std::cout << "信号量集合 (sem):   " << usage.sem_used << " / " << usage.sem_limit << "   "
                  << percent(usage.sem_used, usage.sem_limit) << " %\n";
        std::cout << "消息队列 (msg):     " << usage.msg_used << " / " << usage.msg_limit << "   "
                  << percent(usage.msg_used, usage.msg_limit) << " %\n";
        std::cout << "-------------------------------------\n";
    }

private:
    static size_t countLines(const std::string& path) {
        std::ifstream file(path);
        std::string   line;
        size_t        count = 0;
        if (!file.is_open())
            return 0;
        std::getline(file, line); // skip header
        while (std::getline(file, line)) {
            if (!line.empty())
                count++;
        }
        return count;
    }

    static size_t readKernelVal(const std::string& path) {
        std::ifstream file(path);
        std::string   val;
        if (!file.is_open())
            return 0;
        std::getline(file, val);
        return std::stoull(val);
    }

    static size_t readSemmniFromSem() {
        std::ifstream file("/proc/sys/kernel/sem");
        std::string   line;
        if (!file.is_open())
            return 0;
        std::getline(file, line);
        std::istringstream iss(line);
        size_t             val, count = 0;
        while (iss >> val) {
            count++;
            if (count == 4)
                return val; // SEMMNI 是第4个值
        }
        return 0;
    }
};

#endif
