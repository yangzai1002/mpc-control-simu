#ifndef FAULTHANDLE_H
#define FAULTHANDLE_H

#include <sys/time.h>

#include <iostream>
#include <iterator>
#include <map>
#include <unordered_map>
#include <vector>

#include "monitor/monitor_api/monitor_api.h"

#include "macros.h"

#define ERROR_CODE_SIZE 30
#define FAULT_MOUDLE_SIZE 52
#define FAULT_MODULE_NAME_SIZE 52
#define FAULT_CODE_VECTOR_SIZE 52

namespace FaultHandle {
#if 1
class FaultApi {
public:
    static FaultApi *Instance() {
        static FaultApi inst;
        return &inst;
    }

    // 添加错误码
    void SetFaultCode(const std::string &str) {
        timeval tv{};
        gettimeofday(&tv, nullptr);
        auto ts = tv.tv_sec * 1000.0 + tv.tv_usec / 1000000.0;
        avos::common::MonitorApi::Instance()->SetFaultInfo(ts, strtoul(str.c_str(), nullptr, 10), "");
    }

    // 添加错误码
    void SetFaultCode(const std::string &str, const std::string &desc) {
        timeval tv{};
        gettimeofday(&tv, nullptr);
        auto ts = tv.tv_sec * 1000.0 + tv.tv_usec / 1000000.0;
        avos::common::MonitorApi::Instance()->SetFaultInfo(ts, strtoul(str.c_str(), nullptr, 10), desc);
    }

    // 移除错误码
    void ResetFaultCode(const std::string &str) {
        timeval tv{};
        gettimeofday(&tv, nullptr);
        auto ts = tv.tv_sec * 1000.0 + tv.tv_usec / 1000000.0;
        avos::common::MonitorApi::Instance()->ResetFaultInfo(ts, strtoul(str.c_str(), nullptr, 10));
    }

private:
    FaultApi() = default;
};
enum class FaultHandleMode { TRACK, REMOTECONTROL, FOLLOW };
#else

struct ErrorCode {
    char code_vec[ERROR_CODE_SIZE];

    // 设置错误码
    void SetErrorCode(const std::string &str) {
        memset(code_vec, 0, ERROR_CODE_SIZE);
        memcpy(code_vec, str.c_str(), str.size() > (ERROR_CODE_SIZE - 1) ? (ERROR_CODE_SIZE - 1) : str.size());
    }
    // 获取错误码
    std::string GetErrorCode() {
        return code_vec;
    }
};

struct ErrorCodeNode {
    int       error_num;
    char      module_name[FAULT_MODULE_NAME_SIZE];
    ErrorCode code_vec[ERROR_CODE_SIZE];

    // 构造函数
    ErrorCodeNode() {
        ErrorCodeNodeInit();
    }

public:
    // 初始化错误码节点
    void ErrorCodeNodeInit() {
        error_num = 0;
        memset(module_name, 0, sizeof(module_name)) for (int i = 0; i < ERROR_CODE_SIZE; ++i) {
            code_vec[i].SetErrorCode("");
        }
    }

    // 设置故障模块名称
    void SetFaultModuleName(const std::string &str) {
        memset(module_name, 0, FAULT_MODULE_NAME_SIZE);
        memcpy(module_name, str.c_str(),
               str.size() > (FAULT_MODULE_NAME_SIZE - 1) ? (FAULT_MODULE_NAME_SIZE - 1) : str.size());
    }

    // 获取故障模块名称
    std::string GetFaultModuleName() {
        return module_name;
    }

    // 添加错误码
    void AddErrorCode(const std::string &str) {
        if (IsFindErrorCode(str)) {
            return;
        }

        code_vec[error_num].SetErrorCode(str);
        error_num++;
    }

    // 移除错误码
    void RemoveErrorCode(const std::string &str) {
        std::cout << "remove error code:" << str << std::endl;
        for (auto &code_node : code_vec) {
            if (code_node.GetErrorCode() == str) {
                code_node.SetErrorCode("");
                error_num--;
                break;
            }
        }
    }

private:
    // 查找错误码
    bool IsFindErrorCode(const std::string &str) {
        for (auto code_node : code_vec) {
            if (code_node.GetErrorCode() == str) {
                return true;
            }
        }
        return false;
    }
};

struct FaultHandleShmPool {
    pthread_rwlock_t             rwlock;
    FaultHandle::FaultHandleMode fault_handle_mode; // 0:track;1:remote control;2:follow
    ErrorCodeNode                fault_moudle_vector[FAULT_MOUDLE_SIZE];
    ErrorCodeNode                all_error_code;
    FaultHandleShmPool() {
        fault_handle_mode = FaultHandle::FaultHandleMode::TRACK;
    }
};

class FaultApi {
public:
    FaultApi() : current_module_name_(""), current_shmid_(0), fault_shm_pool_(nullptr) {
        // TODO: 不能在此处清理共享内在，只能在整个服务启动时清理共享内存，此处不做清理操作
        // ExeLinuxCmd( "ipcs -m | awk '{if($6==0) print $2,$4}' | grep 666 | awk '{print $1}' | xargs -I {} ipcrm
        // -m {}");
        auto                 key = keygen("app_fault_monitor_pool_20250429");
        auto                 size = sizeof(FaultHandleShmPool) + 4095 / 4096;
        struct shmid_ds      buf;
        pthread_rwlockattr_t attr;

        current_shmid_ = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
        if (current_shmid_ < 0 && errno != EEXIST) {
            std::cout << "shmget error: " << strerror(errno) << std::endl;
            goto error1;
        }
        if (current_shmid_ < 0 && errno == EEXIST) {
            current_shmid_ = shmget(key, 0, 0666);
            if (current_shmid_ < 0) {
                std::cout << "shmget error: " << strerror(errno) << std::endl;
                goto erro1;
            }
        }
        shmctl(current_shmid_, IPC_STAT, &buf);
        if (buf.shm_segsz < size) {
            std::cout << "shm size error" << std::endl;
            goto error1;
        }
        std::cout << "shmid: " << current_shmid_ << std::endl;

        fault_shm_pool_ = (FaultHandleShmPool *)shmat(current_shmid_, nullptr, 0);
        if (fault_shm_pool_ == nullptr) {
            std::cout << "shmat error: " << strerror(errno) << std::endl;
            goto error1;
        }

        pthread_rwlockattr_init(&attr);
        pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        if (0 == buf.shm_nattch) {
            if (pthread_rwlock_init(&fault_shm_pool_->rwlock, &attr) < 0) {
                std::cout << "pthread_rwlock_init error: " << strerror(errno) << std::endl;
                goto error2;
            }
        }
        pthread_rwlockattr_destroy(&attr);
        return;

error2:
        pthread_rwlockattr_destroy(&attr);
        shmdt(fault_shm_pool_);
        fault_shm_pool_ = nullptr;
        shmctl(current_shmid_, IPC_STAT, &buf);
        if (0 == buf.shm_nattch) {
            shmctl(current_shmid_, IPC_RMID, nullptr);
        }

error1:
        exit(0);
    }
    ~FaultApi() {
        shmdt(fault_shm_pool_);
        shmctl(current_shmid_, IPC_STAT, &buf);
        if (0 == buf.shm_nattch) {
            shmctl(current_shmid_, IPC_RMID, nullptr);
            pthread_rwlock_destroy(&fault_shm_pool_->rwlock)
        }
    }

public:
    // 初始化监控
    /**
     * @brief 初始化监控模块
     *
     * 根据传入的模块名称，初始化监控模块。
     *
     * @param module_name 模块名称
     *
     * @return 如果模块已经初始化，返回 false；如果模块初始化成功，返回 true。
     */
    bool MonitirInit(const std::string &module_name) {
        bool find_flag = false;
        current_module_name_ = module_name;
        // 查找到当前模块
        for (int i = 0; i < FAULT_CODE_VECTOR_SIZE; i++) {
            std::string fault_name = fault_shm_pool_->fault_moudle_vector[i].GetFaultModuleName();
            if (fault_name == current_module_name_) {
                find_flag = true;
                std::cout << "该模块已完成初始化" << std::endl;
                return false;
            }
        }

        // 未查找到当前模块，新增
        if (find_flag == false) {
            for (int i = 0; i < FAULT_CODE_VECTOR_SIZE; i++) {
                std::string fault_name = fault_shm_pool_->fault_moudle_vector[i].GetFaultModuleName();
                if (fault_name == "") {
                    fault_shm_pool_->fault_moudle_vector[i].SetFaultModuleName(current_module_name_);
                    std::cout << "初始化成功" << std::endl;
                    break;
                }
            }
        }
    }

    // 设置错误码
    void SetFaultCode(const std::string &error_code) {
        pthread_rwlock_wrlock(&fault_shm_pool_->rwlock);
        fault_shm_pool_->all_error_code.AddErrorCode(error_code);
        pthread_rwlock_unlock(&fault_shm_pool_->rwlock);
    }

    // 重置错误码
    void ResetFaultCode(const std::string &error_code) {
        fault_shm_pool_->all_error_code.RemoveErrorCode(error_code);
    }

    // 获取错误码向量
    std::vector<std::string> GetFaultCodeVector() {
        std::vector<std::string> fault_code_vector;
        // std::cout << "fault_shm_pool_->all_error_code.code_vec" <<
        // fault_shm_pool_->all_error_code.code_vec.size() << std::endl;
        for (auto node : fault_shm_pool_->all_error_code.code_vec) {
            std::string temp_code = node.GetErrorCode();
            if (temp_code == "") {
                continue;
            }
            fault_code_vector.push_back(temp_code);
        }
        return fault_code_vector;
    }

    // 设置故障处理模式
    void SetFaultHandleMode(FaultHandleMode mode) {
        fault_shm_pool_->fault_handle_mode = mode;
    }

    // 获取故障处理模式
    FaultHandleMode GetFaultHandleMode() {
        return fault_shm_pool_->fault_handle_mode;
    }

    //清理所有故障
    void ClearFaultHandleVector() {
        for (int i = 0; i < FAULT_CODE_VECTOR_SIZE; ++i) {
            fault_shm_pool_->all_error_code.code_vec[i].SetErrorCode("");
        }
    }

private:
    key_t keygen(const std::string &key) {
        std::uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
        std::uint32_t hash = 0;

        for (std::size_t i = 0; i < key.size(); ++i) {
            hash = hash * seed + key.at(i);
        }

        return (hash & 0x7FFFFFFF);
    }

private:
    // 查找故障模块
    bool IsFIndFaultModule(int currnet_index, const std::string &module_name) {
        for (auto code : fault_shm_pool_->fault_moudle_vector[currnet_index].code_vec) {
            if (code.GetErrorCode() == module_name) {
                return true;
            }
        }
        return false;
    }

    // 设置错误码
    void SetErrorCodes(int currnet_index, const std::vector<std::string> &error_code) {
        size_t size = error_code.size();
        for (size_t i = 0; i < size && i < FAULT_CODE_VECTOR_SIZE; ++i) {
            if (IsFIndFaultModule(currnet_index, error_code[i]) == false) {
                fault_shm_pool_->fault_moudle_vector[currnet_index].code_vec[i].SetErrorCode(error_code[i]);
            }
        }
    }

    // 获取故障模块索引
    int GetFaultModuleIndex(std::string module_name) {
        for (int i = 0; i < FAULT_CODE_VECTOR_SIZE; i++) {
            std::string fault_name = fault_shm_pool_->fault_moudle_vector[i].GetFaultModuleName();
            if (fault_name == module_name) {
                return i;
            }
        }
        return -1;
    }

private:
    FaultHandleShmPool *                                       fault_shm_pool_;
    std::string                                                current_module_name_;
    int                                                        current_shmid_;
    std::unordered_map<std::string, std::vector<std::string> > fault_code_map_;

    BASE_DECLARE_SINGLETON(FaultApi)
}; // namespace FaultHandle
#endif
} // namespace FaultHandle
#endif // FAULTHANDLE_H
