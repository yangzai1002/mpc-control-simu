#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include "custom_stack.h"
#include "log/avos_node.h"
#include "common_header/geotool.h" 
#include "base_control/adsfi_struct_adapter.h"

// 初始化静态成员
LatcontrollerInterfaceConfig AdsfiInterface::lat_config;

void AdsfiInterface::Init()  
{
    // AEB 需要高频运行，设为 50ms (20Hz)
    SetScheduleInfo("timmer", 50); 
    std::cout << "init aeb" << std::endl;
    if (simulation_mode_) {
        std::cout << "Simulation mode enabled" << std::endl;
        InitSimulationData();
    }
    flag_ptr_ = std::make_shared<int>();
    // ApLogInterface::Instance()->Init("AEBControl");
}

void AdsfiInterface::DeInit()  
{
    std::cout << "deinit aeb" << std::endl;
}

// ================== Callbacks ==================

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) {
    safebuf_bus_cmd_.Set(ptr);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &ptr) {
    safebuf_input_ctrl_.Set(ptr);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr) {
    safebuf_veh_info_.Set(ptr);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr) {
    if (ptr == nullptr) return;

    auto internal_path = std::make_shared<PlanningTrajectory>();
    // 调用转换函数
    adsfi::utils::ConvertMsgHafToPlanningTraj(*ptr, *internal_path);
    safebuf_input_path_.Set(internal_path);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) {
    safebuf_loc_.Set(ptr);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr) {
    safebuf_static_env_.Set(ptr);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr) {
    safebuf_fusion_objs_.Set(ptr);
}
void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &ptr) {
    safebuf_fault_data_.Set(ptr);
}

// --- 4. 辅助逻辑 ---

// 根据模式适配定位数据
void AdsfiInterface::AdaptLocalizationByMode(const int business_mode, 
                                    const ara::adsfi::MsgHafLocation& raw_loc, 
                                    ara::adsfi::MsgHafLocation& adapted_loc) {
    // 复制原始数据
    adapted_loc = raw_loc;
    bool use_dr_source = (business_mode == 11);

    if (use_dr_source) {
        adapted_loc.pose = raw_loc.dr_pose; 
        ApInfo("aeb") << "[AEB_Adapter] Mode " << business_mode << ": Switched to DR Source";
    }
}

bool AdsfiInterface::AebFaultDiagnosis(const ara::adsfi::MsgHafLocation& loc_pos, const PerceptionFinalObjects& perception_objects,
                        const ara::adsfi::VehicleInformation& car_status) {
    static double last_loc_time = 0.0;
    static double last_perception_time = 0.0;
    static double last_chassis_time = 0.0;
    
    // 1. 检查定位频率
    double current_loc_time = TimeToSec(loc_pos.header.timestamp);
    if (last_loc_time > 0.0) {
        double time_diff = std::fabs(current_loc_time - last_loc_time);
        if (time_diff > 0.5) {
            ApError("aeb") << "[AEB FAULT] Loc delay: " << time_diff;
            FaultHandle::FaultApi::Instance()->SetFaultCode("3102001", "AEB Loc Freq Error");
            last_loc_time = current_loc_time;
            return false;
        } else {
            FaultHandle::FaultApi::Instance()->ResetFaultCode("3102001");
        }
    }
    last_loc_time = current_loc_time;

    // 2. 检查感知频率
    if (last_perception_time > 0.0) {
        double time_diff = std::fabs(perception_objects.header.time - last_perception_time);
        if (time_diff > 0.5) {
            ApError("aeb") << "[AEB FAULT] Perception delay: " << time_diff;
            FaultHandle::FaultApi::Instance()->SetFaultCode("3102002", "AEB Perception Freq Error");
            last_perception_time = perception_objects.header.time;
            return false;
        } else {
            FaultHandle::FaultApi::Instance()->ResetFaultCode("3102002");
        }
    }
    last_perception_time = perception_objects.header.time;

    // 3. 检查底盘频率
    double current_chassis_time = TimeToSec(car_status.header.time);
    if (last_chassis_time > 0.0) {
        double time_diff = std::fabs(current_chassis_time - last_chassis_time);
        if (time_diff > 0.5) {
            ApError("aeb") << "[AEB FAULT] Chassis delay: " << time_diff;
            FaultHandle::FaultApi::Instance()->SetFaultCode("3102003", "AEB Chassis Freq Error");
            last_chassis_time = current_chassis_time;
            return false;
        } else {
            FaultHandle::FaultApi::Instance()->ResetFaultCode("3102003");
        }
    }
    last_chassis_time = current_chassis_time;

    return true;
}

// ================== Process 1: 主处理 (输出 VehicleActControl) ==================
int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleActControl> &control_ptr) 
{
    auto start_time = std::chrono::high_resolution_clock::now();
    // --- 1. 获取所有输入指针 ---
    std::shared_ptr<ara::adsfi::BusinessCommand> bus_ptr;
    std::shared_ptr<ara::adsfi::VehicleActControl> input_ctrl_ptr;
    std::shared_ptr<ara::adsfi::VehicleInformation> veh_ptr;
    std::shared_ptr<PlanningTrajectory> path_ptr;
    std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr;
    std::shared_ptr<ara::adsfi::PerceptionStaticEnv> env_ptr;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> obj_ptr;
    std::shared_ptr<ara::adsfi::FaultData> fault_ptr;

    bool b_bus = safebuf_bus_cmd_.Get(bus_ptr);
    bool b_ctrl = safebuf_input_ctrl_.Get(input_ctrl_ptr);
    bool b_veh = safebuf_veh_info_.Get(veh_ptr);
    bool b_path = safebuf_input_path_.Get(path_ptr);
    bool b_loc = safebuf_loc_.Get(loc_ptr);
    bool b_env = safebuf_static_env_.Get(env_ptr);
    bool b_obj = safebuf_fusion_objs_.Get(obj_ptr);
    bool b_fault = safebuf_fault_data_.Get(fault_ptr);

    // --- 2. 严格保底检查 ---
    if (!b_loc) {
        ApInfo("aeb") << "[AEB] loc data missing";
        if (simulation_mode_) loc_ptr = sim_loc_;
        else return -1;
    }
    if (!b_veh) {
        ApInfo("aeb") << "[AEB] veh data missing";
        if (simulation_mode_) veh_ptr = sim_veh_info_;
        else return -1;
    }
    if (!b_bus) {
        ApInfo("aeb") << "[AEB] bus_cmd data missing";
        if (simulation_mode_) bus_ptr = sim_bus_cmd_;
        else return -1;
    }
    if (!b_ctrl) {
        ApInfo("aeb") << "[AEB] input_ctrl data missing";
        if (simulation_mode_) input_ctrl_ptr = sim_input_ctrl_;
        else return -1;
    }
    // 非关键数据保底
    if (!b_obj) {
        ApInfo("aeb") << "[AEB] obj data missing, using empty array";
        obj_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
    }
    
    // --- 3. 内部静态变量 ---
    static ara::adsfi::BusinessCommand m_10_0;       // BusCmd (直接赋值)
    static ara::adsfi::VehicleActControl m_5_0;      // InputControl (直接赋值)
    static ara::adsfi::VehicleInformation m_3_0;     // VehInfo (直接赋值)
    static PlanningTrajectory m_1_0;     // InputPath (直接赋值)
    static ara::adsfi::PerceptionStaticEnv m_6_0;    // StaticEnv (直接赋值)
    static ara::adsfi::FaultData m_8_0;              // Fault (直接赋值)
    
    // 这3个必须转换 (源数据非Idp)
    static ara::adsfi::MsgHafLocation m_2_0;        // Loc (需转换)
    static PerceptionFinalObjects m_7_0; // Objs (需转换)
    
    // 输出变量 (Process 2 需要访问)
    static PlanningTrajectory m_0_0;     // OutputPath (AEB Trajectory)
    static ara::adsfi::VehicleActControl m_0_1;      // OutputControl (Final)

    // --- 4. 数据转换 (Conversion) ---
    // [直接赋值] 对于Idp类型，无需 Converter
    if (bus_ptr) m_10_0 = *bus_ptr;
    if (input_ctrl_ptr) m_5_0 = *input_ctrl_ptr;
    if (veh_ptr) m_3_0 = *veh_ptr;
    if (path_ptr) m_1_0 = *path_ptr;
    if (env_ptr) m_6_0 = *env_ptr;
    if (fault_ptr) m_8_0 = *fault_ptr;
    if (loc_ptr) m_2_0 = *loc_ptr;
    if (obj_ptr) {
        adsfi::utils::ConvertFusionToFinalObjects(obj_ptr, m_7_0);
    }

    static long long global_seq = 0; 
    m_5_0.header.seq = ++global_seq;
    // m_5_0.header.time = avos::common::NodeTime::Now().ToSecond();
    m_5_0.header.module_name = "AEBControl20260209";

    // --- 5. 初始化 AEB 核心 ---
    static avos::xaeb_control::TrajectorySafetyChecker aeb_checker;
    static avos::control_pid::LookUpTable lookup_controller;

    if (!is_aeb_init_) {
        ApInfo("aeb") << "[XAEB] Initializing...";
        if (!lat_config.LoadVechleParamFromModle() || !lat_config.LoadLatControlParamFromModle()) {
            ApError("aeb") << "AEB Load Lat Config Failed!"; return -1;
        }
        LoncontrollerInterfaceConfig lon_config;
        if (!lon_config.LoadVechleParamFromModle() || !lon_config.LoadLonControlParamFromModle()) {
            ApError("aeb") << "AEB Load Lon Config Failed!"; return -1;
        }
        aeb_checker.SetVehicleParam(lat_config, lon_config);
        if (lookup_controller.Init(lon_config.lower_controller_config) != 0) {
            ApError("aeb") << "AEB Init Lookup Table Failed!";
        }
        is_aeb_init_ = true;
    }

    // --- 6. 执行核心逻辑 (复刻 XAEBControl) ---
    ApInfo("aeb") << "----------- [XAEBControl] Process Start ------------------";
    
    // 默认输出 = 输入 (透传)
    m_0_1 = m_5_0; 

    // 6.1 业务模式判断
    bool valid_mode = (m_10_0.business_mode == 1 || m_10_0.business_mode == 11);
    if (!valid_mode || m_10_0.param.task_avoid_mode == 3) {
        // 非自主模式或任务避障模式，AEB不工作
        *control_ptr = m_0_1; 
        m_0_0.trajectory.clear();
        
        auto traj_ptr = std::make_shared<PlanningTrajectory>(m_0_0);
        safebuf_output_traj_.Set(traj_ptr);
        safebuf_flag_info_.SetNotify(flag_ptr_); 
        return 0;
    }
    ara::adsfi::MsgHafLocation adapted_loc;
    AdaptLocalizationByMode(m_10_0.business_mode, m_2_0, adapted_loc);

    // 6.2 曲率计算
    ara::adsfi::VehicleActControl modified_control = m_5_0;
    bool is_skid_steer = (lat_config.car_type == "6x6" || lat_config.car_type == "sim");
    if (is_skid_steer) {
        double track_width = lat_config.latcontrol_param.vehicle_width * 0.90;

        double vl = 0.0;
        double vr = 0.0;
        
        // 遍历 wheel_speed_vec 数组查找前左(0)和前右(1)
        for (const auto& wheel : m_3_0.wheel_speed.wheel_speed_vec) {
            // 检查数据有效性 (wheelSpeedMpsValid == 1)
            if (wheel.wheelSpeedMpsValid == 1) {
                if (wheel.wheelSpeedPos == 0) { // 0: 前左
                    vl = static_cast<double>(wheel.wheelSpeedMps);
                } 
                else if (wheel.wheelSpeedPos == 1) { // 1: 前右
                    vr = static_cast<double>(wheel.wheelSpeedMps);
                }
            }
        }
        double v_avg = (vr + vl) / 2.0;

        // 定义两个计算结果变量
        double calc_curvature_wheel = 0.0; // 方法1：基于轮速差
        double calc_curvature_imu = 0.0;   // 方法2：基于定位IMU

        // 滑移系数 (仅用于轮速方法)
        const double k_slip = 0.82;
        // 低速保护阈值
        const double LOW_SPEED_THRESHOLD = 0.3;
        const double MAX_CURVATURE_LIMIT = 0.35;

        // --- 计算 1: 基于轮速差 (Wheel Speed) ---
        if (std::abs(v_avg) > LOW_SPEED_THRESHOLD && track_width > 0.1) {
            calc_curvature_wheel = k_slip * (vr - vl) / (track_width * v_avg);
            if (calc_curvature_wheel > MAX_CURVATURE_LIMIT)
                calc_curvature_wheel = MAX_CURVATURE_LIMIT;
            if (calc_curvature_wheel < -MAX_CURVATURE_LIMIT)
                calc_curvature_wheel = -MAX_CURVATURE_LIMIT;
        } else {
            calc_curvature_wheel = 0.0;
        }

        // --- 计算 2: 基于定位IMU (Loc YawRate) ---
        double yaw_rate_deg = adapted_loc.velocity.twist.angular.z;

        if (std::abs(v_avg) > LOW_SPEED_THRESHOLD) {
            double yaw_rate_rad = yaw_rate_deg * M_PI / 180.0;
            calc_curvature_imu = yaw_rate_rad / v_avg;
            // 防止低速噪音导致计算出物理上不可能的急转弯
            if (calc_curvature_imu > MAX_CURVATURE_LIMIT) {
                calc_curvature_imu = MAX_CURVATURE_LIMIT;
            } else if (calc_curvature_imu < -MAX_CURVATURE_LIMIT) {
                calc_curvature_imu = -MAX_CURVATURE_LIMIT;
            }

        } else {
            calc_curvature_imu = 0.0;
        }

        // 确定最终使用的计算曲率 (优先使用 IMU)
        double calc_curvature = calc_curvature_imu;

        static double last_filtered_curvature = 0.0;
        const double  LPF_ALPHA = 0.2;
        // 如果处于低速保护状态，直接重置滤波器
        if (std::abs(v_avg) <= LOW_SPEED_THRESHOLD) {
            calc_curvature = 0.0;
            last_filtered_curvature = 0.0;
        } else {
            calc_curvature = calc_curvature * LPF_ALPHA + last_filtered_curvature * (1.0 - LPF_ALPHA);
            // 更新历史值
            last_filtered_curvature = calc_curvature;
        }

        // --- 误差对比与打印 ---
        double curvature_diff = std::abs(calc_curvature_imu - calc_curvature_wheel);

        ApInfo("aeb") << "[AEB曲率源对比] "
              << "IMU计算(Raw): " << calc_curvature_imu << " | 滤波后(Final): " << calc_curvature
              << " | 轮速计算: " << calc_curvature_wheel << " | 原始数据(YawRate/Speed): " << yaw_rate_deg << "deg/s / "
              << v_avg << "m/s";

        // double origin_curvature = input_control.lat_control_debug.temp1;
        // ApInfo("aeb") << "[AEB-Debug] 实际曲率(用于预测): " << calc_curvature
        //       << " | 指令曲率(用于参考): " << origin_curvature
        //       << " | 偏差: " << std::abs(calc_curvature - origin_curvature);
        // 覆盖控制指令
        modified_control.lat_control.target_curvature = calc_curvature;
    }

    // 6.3 故障诊断
    if (!AebFaultDiagnosis(adapted_loc, m_7_0, m_3_0)) {
        ApError("aeb") << "AEB Input Data Fault!";
        // 故障时透传控制，或者进行降级处理
        // coverter(m_0_1, control_ptr);
        safebuf_flag_info_.SetNotify(flag_ptr_);
        return 0;
    }

    // 6.4 安全检查
    PerceptionFinalObjects output_risk_objs;
    // 调用 AEB 库函数
    aeb_checker.ProcessSafetyCheck(m_1_0, modified_control, m_3_0, adapted_loc, m_6_0, m_7_0, output_risk_objs);

    // 6.5 获取结果
    avos::xaeb_control::AEBCommand aeb_cmd = aeb_checker.GetAebCommand();
    int fcw_level = 0;
    aeb_checker.GetAlertSignal(fcw_level);
    
    // 获取 AEB 预测的轨迹 (赋值给 m_0_0，这是 Process 2 要输出的)
    m_0_0 = aeb_checker.GetTransformedTrajectory();

    ApInfo("aeb") << "[AEB] FCW Level: " << fcw_level;

    // 6.6 覆盖控制指令
    if (aeb_cmd.aeb_active) {
        ApError("aeb") << "[AEB] TRIGGERED! Decel=" << aeb_cmd.desired_deceleration;
        
        m_0_1.lon_control.target_torque = 0.0;

        if (is_skid_steer) {
            m_0_1.lon_control.target_speed = aeb_cmd.target_speed;
            m_0_1.lon_control.target_pressure = 0.0;
        } else {
            // 查表计算制动压力
            LowerControllerInput lower_input;
            lower_input.vehicle_speed = std::abs(m_3_0.vehicle_act_state.speed);
            lower_input.acel_desired = aeb_cmd.desired_deceleration;
            
            LoncontrolOutput lower_output;
            LoncontrolDebugStruct local_debug;
            lookup_controller.ComputationalControlOut(lower_input, lower_output, "AEB", 1, local_debug);
            
            m_0_1.lon_control.target_torque = lower_output.torque;
            m_0_1.lon_control.target_pressure = lower_output.pressure;
            
            // g318 适配
            if (lat_config.car_type == "g318" && m_0_1.lon_control.target_pressure > 0.0) {
                // m_0_1.lon_control.target_accelation = -m_0_1.lon_control.target_pressure;
                m_0_1.lon_control.target_torque = 0.0;
                m_0_1.lon_control.actuator_mode = 0;
                // m_0_1.lon_control.epb_mode = 1;
            }
        }
    } else {
        // 对齐版本B日志
        ApWarn("aeb") << "[AdsfiInterface] No AEB trigger. Following normal control command.";
    }

    // 7. 输出最终控制 (Control)
    *control_ptr = m_0_1;

    // 8. 将计算好的轨迹 m_0_0 传递给 Process 2
    // 使用 shared_ptr 包装以便放入 Buffer
    auto output_traj_ptr = std::make_shared<PlanningTrajectory>(m_0_0);
    safebuf_output_traj_.Set(output_traj_ptr);

    // 9. 准备状态数据 (Status) 并发送
    static ara::adsfi::PlanningStatus status_data;
    status_data.risk_level = fcw_level;

    // 10. 通知 Flag (用于 Status 同步，虽然 Buffer Set 已经足够唤醒 Process 2，但保持一致性)
    safebuf_flag_info_.SetNotify(flag_ptr_);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    ApWarn("aeb") << "[AEB] 处理过程耗时：" << duration.count() / 1000.0 << " ms.";

    return 0;
}

// ================== Process 2: 副处理 (输出 PlanningTrajectory) ==================

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &traj_ptr) 
{
    // 等待逻辑，确保 Process 1 已经计算完毕并发出通知
    std::shared_ptr<int> temp_flag;
    safebuf_flag_info_.WaitForGet(temp_flag);
    // 阻塞等待 Process 1 的数据
    std::shared_ptr<PlanningTrajectory> internal_traj_ptr;
    // Get 会阻塞直到 Buffer 非空
    if (safebuf_output_traj_.Get(internal_traj_ptr)) {
        if (internal_traj_ptr) {
            adsfi::utils::ConvertTrajectoryToMsgHafEgoTrajectory(*internal_traj_ptr, *traj_ptr);
            return 0;
        }
    }
    return -1;
}

void AdsfiInterface::InitSimulationData()
{
    sim_loc_ = std::make_shared<ara::adsfi::MsgHafLocation>();
    sim_veh_info_ = std::make_shared<ara::adsfi::VehicleInformation>();
    sim_veh_info_->vehicle_act_state.speed = 5.0; 
    sim_input_ctrl_ = std::make_shared<ara::adsfi::VehicleActControl>();
    sim_input_ctrl_->lon_control.target_speed = 5.0;
    sim_bus_cmd_ = std::make_shared<ara::adsfi::BusinessCommand>();
    sim_bus_cmd_->business_mode = 1;
}
