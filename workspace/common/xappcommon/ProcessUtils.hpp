#ifndef __PROCESS_UTILS_HPP__
#define __PROCESS_UTILS_HPP__

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "log/log.h"

namespace app_common {
    class ProcessUtils {
    public:
        ProcessUtils() = delete;

    public:
        static std::vector<int> find_process_by_name(const std::string& process_name) {
            std::vector<int> matching_pids;
            DIR*             dir = opendir("/proc");
            if (!dir) {
                AERROR << __FUNCTION__ << ": Failed to open /proc directory!";
                return matching_pids;
            }

            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                if (!isdigit(entry->d_name[0]))
                    continue;

                std::string pid = entry->d_name;
                std::string cmdline_path = "/proc/" + pid + "/cmdline";

                std::ifstream cmdline_file(cmdline_path);
                if (!cmdline_file.is_open())
                    continue;

                std::string cmdline;
                std::getline(cmdline_file, cmdline, '\0');
                cmdline_file.close();
                if (cmdline.find(process_name) != std::string::npos) {
                    matching_pids.push_back(std::stoi(pid));
                }
            }

            closedir(dir);
            return matching_pids;
        }

        // 获取指定进程的所有子孙进程
        static std::vector<pid_t> descendant_pids(pid_t pid) {
            std::vector<pid_t>                       pids;
            std::string                              command = "pstree -p " + std::to_string(pid);
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
            if (!pipe) {
                AERROR << "Failed to run pstree command";
                return pids;
            }

            // 使用正则表达式匹配 PID
            std::regex  pid_regex("\\((\\d+)\\)");
            std::smatch match;
            char        buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe.get())) {
                std::string          line(buffer);
                std::sregex_iterator iter(line.begin(), line.end(), pid_regex);
                std::sregex_iterator end;
                while (iter != end) {
                    pids.push_back(std::stoi((*iter)[1]));
                    ++iter;
                }
            }
            std::reverse(pids.begin(), pids.end());
            return pids;
        }

        // 检查指定 PID 是否存在
        static bool running1(pid_t pid) {
            std::string procPath = "/proc/" + std::to_string(pid);
            struct stat sts;
            return (stat(procPath.c_str(), &sts) == 0);
        }

        static bool running2(pid_t pid) {
            return 0 == kill(pid, 0);
        }

        // 尝试终止进程
        static bool terminate(pid_t pid, int timeout_milliseconds = 1000) {
            if (kill(pid, SIGTERM) == -1) {
                return false;
            }

            auto start = std::chrono::steady_clock::now();
            while (true) {
                if (!running2(pid)) {
                    AINFO << "Process " << pid << " terminated successfully with SIGTERM.";
                    return true;
                }
                auto now = std::chrono::steady_clock::now();
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
                if (elapsedSeconds >= timeout_milliseconds) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            AINFO << "Process " << pid << " did not terminate with SIGTERM, sending SIGKILL.";
            if (kill(pid, SIGKILL) == -1) {
                perror("Failed to send SIGKILL");
                return false;
            }

            // 最终确认进程已退出
            if (!running2(pid)) {
                AINFO << "Process " << pid << " terminated successfully with SIGKILL.";
                return true;
            } else {
                AERROR << "Failed to terminate process " << pid << " with SIGKILL.";
                return false;
            }
        }

        static void terminate_include_descendants(pid_t pid) {
            auto pids = descendant_pids(pid);
            for (const auto& pid : pids) {
                terminate(pid);
            }
        }

        static std::pair<std::int32_t, std::string> cmd_exec(const std::string& command) noexcept(false) {
            std::array<char, 128> buffer;
            std::string           result;
            FILE*                 pipe = popen(command.c_str(), "r");
            AINFO << __FUNCTION__ << " command: " << command;
            if (!pipe) {
                return {-1, ""};
            }
            try {
                while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                    result += buffer.data();
                }
            } catch (const std::exception& e) {
                AERROR << command << " error: " << e.what();
                pclose(pipe);
                return {-2, ""};
            } catch (...) {
                AERROR << command << " error: unknown";
                pclose(pipe);
                return {-3, ""};
            }

            return {pclose(pipe), result};
        }

        static pid_t daemonize_and_exec(const char* cmd, char* const argv[]) {
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                AERROR << "pipe failed: " << strerror(errno);
                return -1;
            }

            auto pid = fork();
            if (pid < 0) {
                AERROR << "fork failed: " << strerror(errno);
                return -1;
            }

            // 父进程：读取子进程写入的孙子进程 PID
            if (pid > 0) {
                close(pipefd[1]);
                pid_t grandchild_pid;
                if (read(pipefd[0], &grandchild_pid, sizeof(grandchild_pid)) > 0) {
                    AINFO << "Parent process received grandchild PID: " << grandchild_pid;
                } else {
                    AERROR << "Parent process failed to read grandchild PID";
                    grandchild_pid = -1;
                }
                close(pipefd[0]);
                wait(nullptr); // 等待子进程结束，防止僵尸进程
                return grandchild_pid;
            }

            pid_t grandchild_pid = fork();
            if (0 == grandchild_pid) { // 孙子进程
                close(pipefd[0]);
                close(pipefd[1]);
                AINFO << "Grandchild process PID: " << getpid() << ", Parent PID: " << getppid();

                // 使孙子进程脱离父进程，成为守护进程
                setsid();

                // 执行目标程序
                execvp(cmd, argv);

                // exec 失败才会执行到这里
                perror("execvp failed");
                exit(1);
            } else {                                                       // 子进程
                close(pipefd[0]);                                          // 关闭读端
                write(pipefd[1], &grandchild_pid, sizeof(grandchild_pid)); // 子进程：向父进程写入孙子进程的 PID
                close(pipefd[1]);                                          // 关闭写端
                if (grandchild_pid > 0) {                                  // 子进程
                    AINFO << "Child process exiting, Grandchild PID: " << grandchild_pid;
                    exit(0);
                } else if (grandchild_pid < 0) {
                    perror("fork grandchild failed");
                    exit(1);
                }
            }
        }
    };
} // namespace app_common

#endif
