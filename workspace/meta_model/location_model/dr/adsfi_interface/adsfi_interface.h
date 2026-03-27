#ifndef __DR_ADSFI_INTERFACE_H__
#define __DR_ADSFI_INTERFACE_H__

#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"
#include "ara/adsfi/impl_type_msghaflocation.h"

#include <memory>
#include <string>
#include <vector>
#include <cstddef>

#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

class AdsfiInterface : public BaseAdsfiInterface
{
public:
    AdsfiInterface() = default;
    ~AdsfiInterface() = default;

    void Init();
    void DeInit();

    // ================== 输入回调 (Callbacks) ==================
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafIMU> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);

    // ================== 输出处理 (Process) ==================
    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr);
    // 文件回放接口（离线仿真）
    bool LoadSimData(const std::string &imu_csv, const std::string &wheel_csv,
                     const std::string &tpimu_csv = "");
    int ProcessFromFile(std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr);

    BASE_TEMPLATE_FUNCION

private:
    struct ImuCsvRow {
        double t = 0.0;
        double gyro_z_deg = 0.0;
        double acc_x = 0.0;
        double acc_y = 0.0;
        double acc_z = 0.0;
    };

    struct WheelCsvRow {
        double t = 0.0;
        double v_lf = 0.0;
        double v_rf = 0.0;
        double v_lr = 0.0;
        double v_rr = 0.0;
    };

    struct TpimuCsvRow {
        double t = 0.0;
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double roll_deg = 0.0;
        double pitch_deg = 0.0;
    };

    bool sim_loaded_ = false;
    std::size_t sim_index_ = 0;
    std::vector<ImuCsvRow> sim_imu_;
    std::vector<WheelCsvRow> sim_wheel_;
    std::vector<TpimuCsvRow> sim_tpimu_;
    double sim_tp_x_ = 0.0;
    double sim_tp_y_ = 0.0;
    double sim_tp_last_t_ = -1.0;

    ThreadSafeBuffer<ara::adsfi::MsgHafIMU> safebuf_imu_;
    ThreadSafeBuffer<ara::adsfi::MsgHafWheelSpeedList> safebuf_wheel_;
    ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_tpimu_;

    // ================== 仿真模式 ==================
    bool simulation_mode_ = false;  // 仿真标志位，true=使用虚拟数据
    uint32_t sim_seq_ = 0;         // 仿真序列号
    double sim_time_ = 0.0;        // 仿真时间
    double sim_x_ = 0.0;           // 仿真位置 X
    double sim_y_ = 0.0;           // 仿真位置 Y
    double sim_yaw_ = 0.0;         // 仿真航向角 (度)
    double sim_speed_ = 1.0;       // 仿真速度 (m/s)

    // 初始化仿真数据
    void InitSimulationData();
    // 生成虚拟 IMU 数据
    void GenerateSimIMU(ara::adsfi::MsgHafIMU &imu);
    // 生成虚拟轮速数据
    void GenerateSimWheel(ara::adsfi::MsgHafWheelSpeedList &wheel);
    // 生成虚拟 TPIMU 数据
    void GenerateSimTpimu(ara::adsfi::MsgHafLocation &tpimu);
};

#endif
