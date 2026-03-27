#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include "custom_stack.h"
#include "log/avos_node.h"
#include "common_header/geotool.h"
#include "mviz_interface/mviz_interface.h"

#include "BlockPerceptionData.h"
#include "XBridge.h"
#include "XBridgeFollow.h"       
#include "XBusinessFollow.h"
#include "XConfigParamReader.h"
#include "XEnvironmentalCognition.h"
#include "follow_debug.h"
#include "XFollowPathPlanner.h"
#include "XFollowPathSelector.h"
#include "XRefDirFollow.h"
#include "XRefRecFollow.h"
#include "XSpeedPlanFollow.h"
#include "SpeedPlanDebugInfo.h"
#include "base_control/lat/latcontrol_header.h"
#include "base_control/adsfi_struct_adapter.h"

void AdsfiInterface::Init()  
{
    SetScheduleInfo("timmer", 100); // 100ms 周期
	// ApLogInterface::Instance()->Init("PlanningFollow");

    ApInfo("planning_follow") << "init planning_follow";
    
    has_valid_output_cache_ = false; // 重置缓存有效性标志
    
    // 初始化仿真模式
    if (simulation_mode_) {
        ApInfo("planning_follow") << "Simulation mode enabled - initializing simulation data";
        InitSimulationData();
    } else {
        ApInfo("planning_follow") << "Normal mode - waiting for external inputs";
    }
    flag_ptr_ = std::make_shared<int>();
}

void AdsfiInterface::DeInit()  
{
    ApInfo("planning_follow") << "deinit planning_follow";
}

// ---------------------------------------------------------
// --- Callbacks: 数据接收 ---
// ---------------------------------------------------------

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr) 
{
    std::string tagstr = "callback_point_size=" + std::to_string(ptr->business_path.points.size());
    AVOS_CODETAG_ADV1(tagstr.c_str(),0,0);
    ApInfo("planning_follow") << "BusinessCommand callback:" << ptr->header.seq;
    safebuf_bus_command_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> &ptr) 
{
    ApInfo("planning_follow") << "FusionOutArray Callback name: " << name << ", seq: " << ptr->seq;

    // 假设 "RPortPerceptionFusion" 是通用障碍物列表的端口名
    if (name == "tpperceptionobjects") { 
        safebuf_fusion_out_array_.Set(ptr);
    }
    // 假设 "RPortFollowTarget" 是跟随目标的端口名 
    else if (name == "tptarget_obj") {
        safebuf_target_array_.Set(ptr);
    }
    else {
        ApWarn("planning_follow") << "Unknown FusionOutArray topic: " << name;
    }
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr) 
{
    ApInfo("planning_follow") << "IdpVehicleInformation callback:" << ptr->header.seq;
    safebuf_vehicle_info_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr) 
{
    ApInfo("planning_follow") << "LocationInfo callback:" << ptr->header.seq;
    safebuf_location_info_.Set(ptr);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::PerceptionStaticEnv> &ptr) 
{
    ApInfo("planning_follow") << "PerceptionStaticEnv callback";
    safebuf_static_env_.Set(ptr);
}

// 转换函数: MsgHafFusionOutArray -> PerceptionTargetObj (内部跟随目标)
void ConvertFusionToTargetObj(const std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> in_ptr, PerceptionTargetObj & out)
{
    out.Clear(); // 先清空

    // 1. 转换头部
    double timestamp = static_cast<double>(in_ptr->stamp.sec) + 
                       static_cast<double>(in_ptr->stamp.nsec) * 1e-9;
    out.header.time = timestamp;
    out.header.seq = in_ptr->seq;
    out.header.data_name = in_ptr->frameID;

    // 2. 提取目标数据
    // 跟随目标 Topic 发过来的数组中，第 0 个就是选定的跟随目标
    if (!in_ptr->fusionOut.empty()) {
        const auto& target = in_ptr->fusionOut[0];

        // 根据 confidence 判断无效性 (反向: fusionOut.confidence = perception_objs.invalid?0.0F:1.0F)
        out.invalid = (target.cipvFlag ? 0:1);
        ApInfo("planning_follow") << "ConvertFusionToTargetObj invalid: " << out.invalid;
        out.trk_id = target.objectID;
        out.type = target.fusionType; // 类型字段对应 cs

        // --- 车辆坐标系 (VCS) ---
        out.x = target.rect.center.x;
        out.y = target.rect.center.y;
        out.z = target.rect.center.z;
        out.vx = static_cast<double>(target.velocity.x); // m/s
        out.vy = static_cast<double>(target.velocity.y);
        out.yaw = target.rect.orientation;

        // 尺寸
        out.length = target.rect.size.x;
        out.width = target.rect.size.y;
        out.height = target.rect.size.z;

        // --- 绝对坐标系 (UTM) ---
        out.utm_x = target.absRect.center.x;
        out.utm_y = target.absRect.center.y;
        out.utm_z = target.absRect.center.z;
        out.utm_vx = static_cast<double>(target.absVelocity.x);
        out.utm_vy = static_cast<double>(target.absVelocity.y);
        out.utm_yaw = target.absRect.orientation;
        
        // 计算绝对标量速度
        out.speed = std::hypot(out.utm_vx, out.utm_vy);

        // --- 经纬度 (Lat/Lon) ---
        // 暂时置 0
        out.lat = 0.0;
        out.lon = 0.0; 

    } else {
        out.invalid = true; // 数组为空，目标无效
    }
}

// ---------------------------------------------------------
// --- Process: 核心业务逻辑 ---
// ---------------------------------------------------------

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &planning_result_ptr) 
{
	ApInfo("planning_follow") << "PROCESS PLANNINGFOLLOW";
    // 1. 数据获取 (Data Fetching)
    std::shared_ptr<ara::adsfi::BusinessCommand> bus_command_ptr;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> fusion_objarray_ptr;
    std::shared_ptr<ara::adsfi::MsgHafFusionOutArray> target_raw_ptr;
    std::shared_ptr<ara::adsfi::VehicleInformation> vehicle_info_ptr;
    std::shared_ptr<ara::adsfi::MsgHafLocation> loc_ptr;
    std::shared_ptr<ara::adsfi::PerceptionStaticEnv> static_env_ptr;
    
    bool bGet_bus_cmd = safebuf_bus_command_.Get(bus_command_ptr);
    bool bGet_fus_obj = safebuf_fusion_out_array_.Get(fusion_objarray_ptr);
    bool bGet_target = safebuf_target_array_.Get(target_raw_ptr);
    bool bGet_veh_inf = safebuf_vehicle_info_.Get(vehicle_info_ptr);
    bool bGet_loc_inf = safebuf_location_info_.Get(loc_ptr);
    bool bGet_static_env = safebuf_static_env_.Get(static_env_ptr);
    
    // 2. 异常检查与数据预处理
    // 2.1 定位数据检查
    if(bGet_loc_inf == false)
    {
        if(simulation_mode_ && sim_location_info_) {
            loc_ptr = sim_location_info_;
        } else {
            ApInfo("planning_follow") << "Process error: LocationInfo missing";
            safebuf_flag_info_.SetNotify(flag_ptr_);
            return -1;
        }
    }
    
    // 2.2 业务指令检查
    if(bGet_bus_cmd == false)
    {
        if(!simulation_mode_) {
            // ApInfo("planning_follow") << "Process error: BusinessCommand missing";
            safebuf_flag_info_.SetNotify(flag_ptr_);
            return -1;
        }
        // 仿真模式
        bus_command_ptr = std::make_shared<ara::adsfi::BusinessCommand>();
        bus_command_ptr->header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
        bus_command_ptr->param.command = 1; 
        bus_command_ptr->business_path.points.clear();
    }
    
    // 2.3 感知数据检查
    if(bGet_fus_obj == false)
    {
        if(simulation_mode_)
        {
            // 仿真模式：创建带有时间戳的空数组，模拟"无障碍物"的完美路况
            fusion_objarray_ptr->seq = 0;
            fusion_objarray_ptr->stamp = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
            fusion_objarray_ptr->frameID = "simulation";
            fusion_objarray_ptr->fusionOut.clear(); 
            // ApInfo("planning_follow") << "Simulation mode: using empty FusionObjectArray\n";
        }
        else
        {
            // 实车模式：创建空数组防止崩溃，但打印错误日志
            fusion_objarray_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
            fusion_objarray_ptr->seq = 0;
            fusion_objarray_ptr->stamp = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
            fusion_objarray_ptr->frameID = "error_fallback";
            fusion_objarray_ptr->fusionOut.clear(); 
            ApInfo("planning_follow") << "Process error: FusionObjectArray has no new data\n";
            // 实车如果感知丢失，是否 return -1 取决于策略，这里暂不 return，允许盲开一段或由内部逻辑处理
        }
    }

    if(bGet_target == false) {
        if(simulation_mode_) {
             target_raw_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
        } else {
             target_raw_ptr = std::make_shared<ara::adsfi::MsgHafFusionOutArray>(); // 防止空指针
             AINFO_EVERY(10) << "Process warning: FollowTarget has no new data";
        }
    }
    // 2.4 车辆信息检查
    if(bGet_veh_inf == false)
    {
        if(simulation_mode_)
        {
            // 仿真模式：使用 InitSimulationData 中初始化的 sim_vehicle_info_
            vehicle_info_ptr = sim_vehicle_info_;
            if(!vehicle_info_ptr)
            {
                // 双重保底：如果 sim_vehicle_info_ 也没初始化
                vehicle_info_ptr = std::make_shared<ara::adsfi::VehicleInformation>();
                vehicle_info_ptr->header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
                vehicle_info_ptr->vehicle_act_state.speed = 0.0; // 默认静止
            }
            // 更新时间戳，防止被当作过期数据丢弃
            vehicle_info_ptr->header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
            // ApInfo("planning_follow") << "Simulation mode: using preset VehicleInformation\n";
        }
        else
        {
            // 实车模式：无车辆信息（如车速），规划无法进行，必须报错退出
            ApInfo("planning_follow") << "Process error: IdpVehicleInformation has no new data";
            safebuf_flag_info_.SetNotify(flag_ptr_);
            return -1;
        }
    }

    // 2.5 静态语义地图检查
    if(bGet_static_env == false)
    {
        if(simulation_mode_)
        {
            // 仿真模式：创建一个默认的空地图，带有当前时间戳
            static_env_ptr = std::make_shared<ara::adsfi::PerceptionStaticEnv>();
            static_env_ptr->header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
            static_env_ptr->header.seq = 0;
            static_env_ptr->header.data_name = "simulation_map"; // 标记为仿真数据
            static_env_ptr->semantic_objs.clear();
            // ApInfo("planning_follow") << "Simulation mode: using empty PerceptionStaticEnv";
        }
        else
        {
            // 实车模式：如果没有收到地图数据，打印警告
            // 为了防止空指针崩溃，这里给一个空对象
            static_env_ptr = std::make_shared<ara::adsfi::PerceptionStaticEnv>();
            static_env_ptr->header.time = DoubleToHafTime(avos::common::NodeTime::Now().ToSecond());
            static_env_ptr->semantic_objs.clear();
            
            AINFO_EVERY(10) << "Process warning: PerceptionStaticEnv has no new data";
        }
    }

    std::lock_guard<std::mutex> lock(process_data_mutex_);
    // 3. 核心算法变量定义 (严格对应 XObjectFollow)
    
    // --- 输入/输出数据结构 ---
    static ara::adsfi::BusinessCommand m_0_0;         // BusinessCommand (Recv U-0)
    PerceptionFinalObjects m_23_1; // Output of BlockPerception
    PerceptionFinalObjects m_24_0; // Input (Recv U-24)
    ara::adsfi::PerceptionStaticEnv m_23_0;    // Output of BlockPerception
    ara::adsfi::PerceptionStaticEnv m_3_0;     // Input (Recv U-3)
    PerceptionTargetObj m_1_0;     // Input (Recv U-1) - 目标对象
    static ara::adsfi::PlanningStatus m_7_1;          // Output (Send U-10) - 规划状态
    static PlanningTarget m_5_0;          // Output of XBridgeFollow
    static PlanningTrajectory m_17_0;     // Output (Send U-18) - 最终轨迹
    static ara::adsfi::MsgHafLocation m_2_0;         // Input (Recv U-2)
    static ara::adsfi::VehicleInformation m_4_0;      // Input (Recv U-4)
    
    // --- 内部计算过程变量 ---
    static avos::planning::ConfigInfo m_13_0;            // Config
    static avos::planning::LocalizationData m_6_1;       // Converted Loc
    static avos::planning::Paths m_11_0;                 // Selected Path
    static avos::planning::Paths m_15_1;                 // Planned Path
    static avos::planning::Paths m_21_0;                 // Recorded Path
    static avos::planning::Paths m_22_2;                 // Env Path Debug
    static avos::planning::Paths m_9_0;                  // Direction Path
    static avos::planning::Perception m_6_0;             // Internal Perception
    static avos::planning::PlanningInfo m_15_0;          // Planner Info
    static avos::planning::PlanningInfo m_17_1;          // Feedback Info (上一帧规划信息)
    static avos::planning::PlanningInfo m_22_1;          // Env Info
    static avos::planning::PlanningInfo m_5_1;           // Bridge Info
    static avos::planning::PlanningInfo m_7_0;           // Business Info
    static avos::planning::UnstructEnv m_19_0;           // Unstructured Env
    static avos::planning::UnstructEnv m_22_0;           // Env Data
    static avos::planning::VehicleStatus m_6_2;          // Internal Vehicle Status
    static SpeedPlanDebugInfo m_17_2;

    // --- Debug/Vector 变量 ---
    static ara::adsfi::MsgPoint3DVec m_15_2;              // (Send U-20)
    static ara::adsfi::MsgPoint3DVec m_22_3;              // (Send U-16)

    // 4. 输入数据映射 (Mapping External -> Internal)
    if(bus_command_ptr) m_0_0 = *bus_command_ptr;  
    if(vehicle_info_ptr) m_4_0 = *vehicle_info_ptr;
    if(loc_ptr) m_2_0 = *loc_ptr;
    if(static_env_ptr) m_3_0 = *static_env_ptr;
    
    if(fusion_objarray_ptr) {
        adsfi::utils::ConvertFusionToFinalObjects(fusion_objarray_ptr, m_24_0);
    }

    if(target_raw_ptr) {
        ConvertFusionToTargetObj(target_raw_ptr, m_1_0);
    }
    // 5. 执行算法链
    // [U-13] 读取配置参数
    XConfigParamReader(m_13_0);

    // [U-23] 感知数据预处理
    // 输入: m_3_0, m_24_0, m_0_0
    // 输出: m_23_0, m_23_1
    BlockPerceptionData(m_3_0, m_24_0, m_0_0, m_23_0, m_23_1);

    // [U-6] 基础数据桥接 (转换定位、车辆、感知到内部格式)
    // 输入: m_0_0, m_2_0, m_23_0, m_23_1, m_4_0
    // 输出: m_6_0, m_6_1, m_6_2
    XBridge(m_0_0, m_2_0, m_23_0, m_23_1, m_4_0, m_6_0, m_6_1, m_6_2);

    // [U-5] 跟随模式桥接
    // 输入: m_6_1 (Loc), m_0_0 (Cmd), m_1_0 (Target), m_17_1 (上一帧的规划信息)
    // 输出: m_5_0 (Target), m_5_1 (Info)
    XBridgeFollow(m_6_1, m_0_0, m_1_0, m_17_1, m_5_0, m_5_1);

    // [U-7] 业务逻辑处理
    // 输入: m_6_2, m_5_1
    // 输出: m_7_0 (Info), m_7_1 (PlanningStatus - 待发送)
    XBusinessFollow(m_6_2, m_5_1, m_7_0, m_7_1);

    // [U-9] 生成参考方向路径
    // 输入: m_5_0, m_6_1, m_7_0
    // 输出: m_9_0 (RefDir Path)
    XRefDirFollow(m_5_0, m_6_1, m_7_0, m_9_0);

    // [U-21] 生成轨迹参考线路径
    // 输入: m_5_0, m_6_1, m_7_0
    // 输出: m_21_0 (RefRec Path)
    XRefRecFollow(m_5_0, m_6_1, m_7_0, m_21_0);

    // [U-11] 路径选择器 (决定使用哪条参考路径)
    // 输入: m_9_0, m_21_0
    // 输出: m_11_0 (Selected Path)
    XFollowPathSelector(m_9_0, m_21_0, m_11_0);

    // [U-22] 环境认知 (构建非结构化环境、代价地图等)
    // 输入: Loc, Percept, Veh, Config, Info, Path(m_11_0), Target
    // 输出: m_19_0, m_22_0, m_22_1, m_22_2, m_22_3
    XEnvironmentalCognition(m_6_1, m_6_0, m_6_2, m_13_0, m_7_0, m_11_0, m_19_0, m_5_0, m_22_0, m_22_1, m_22_2, m_22_3);

    follow_debug(m_22_0, m_19_0);

    // [U-15] 路径规划器
    // 输入: Loc, Percept, Veh, Config, Env Info (m_22_x)
    // 输出: m_15_0 (Info), m_15_1 (Planned Path), m_15_2 (Debug)
    XFollowPathPlanner(m_6_1, m_6_0, m_6_2, m_13_0, m_22_1, m_22_2, m_22_0, m_15_0, m_15_1, m_15_2);

    // [U-17] 速度规划器
    // 输入: Target, Path(m_15_1), Loc, Percept, Config, Veh, Info(m_15_0)
    // 输出: m_17_0 (Final Trajectory - 待发送), m_17_1 (Feedback Info - 用于下一帧)
    XSpeedPlanFollow(m_5_0, m_15_1, m_6_1, m_6_0, m_13_0, m_6_2, m_15_0, m_17_0, m_17_1, m_17_2);

    // 6. 结果输出 (Output Mapping)
    ara::adsfi::MsgHafEgoTrajectory temp_out_trajectory;
    adsfi::utils::ConvertTrajectoryToMsgHafEgoTrajectory(m_17_0, temp_out_trajectory);

    // 6.2 存入类成员缓存，供副 Process 安全读取
    cached_planning_status_ = m_7_1;
    has_valid_output_cache_ = true; // 标记缓存已刷新，数据可用

    // 6.3 重新分配一块干净的内存，把数据拷进去
    planning_result_ptr = std::make_shared<ara::adsfi::MsgHafEgoTrajectory>(temp_out_trajectory);

    // 7. 调试与可视化
    // 可视化最终轨迹 (m_17_0) 和 几何规划路径 (m_15_1)
    VisualizePaths(m_17_0, m_15_1, m_11_0); // m_11_0 为参考路径

    // 通知 Flag
    safebuf_flag_info_.SetNotify(flag_ptr_);
    
    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::PlanningStatus> &ptr)
{
    std::shared_ptr<int> temp;
    safebuf_flag_info_.WaitForGet(temp); // 阻塞等待主线程发出唤醒信号
    
    // 被唤醒后，第一时间加锁
    std::lock_guard<std::mutex> lock(process_data_mutex_);
    
    // 拿到锁后，检查是否有主线程合法写入的缓存数据
    if (has_valid_output_cache_) {
        // 分配全新内存，安全拷贝并返回
        ptr = std::make_shared<ara::adsfi::PlanningStatus>(cached_planning_status_);
        return 0;
    } 
    
    return -1; // 尚未生成有效数据（如刚启动或异常降级），拒绝发送
}

// 初始化仿真数据
void AdsfiInterface::InitSimulationData()
{
    sim_location_info_ = std::make_shared<ara::adsfi::MsgHafLocation>();
    sim_location_info_->header.frameID = "simulation";
    sim_location_info_->pose.pose.position.x = 116.3974; 
    sim_location_info_->pose.pose.position.y = 39.9093;
    sim_location_info_->pose.pose.orientation.z = 1.0;
    
    sim_vehicle_info_ = std::make_shared<ara::adsfi::VehicleInformation>();
    sim_fusion_objarray_ = std::make_shared<ara::adsfi::MsgHafFusionOutArray>();
    
    ApInfo("planning_follow") << "Simulation data initialized for planning_follow";
}

// 路径可视化实现
void AdsfiInterface::VisualizePaths(const PlanningTrajectory& final_trajectory,
                                    const avos::planning::Paths& inner_path,
                                    const avos::planning::Paths& debug_path)
{
    struct PathPoint {
        double x, y, z;
        double velocity;
    };
    
    // 使用不同的 topic 名称以区分模块
    static RGBPointCloud<PathPoint> viz_final_path("planning_follow/final_trajectory");
    const double UP_SPEED = 40.0 / 3.6; 
    
    // 简单的颜色映射
    auto setColorByVelocity = [UP_SPEED](void* p) -> float {
        PathPoint* point = static_cast<PathPoint*>(p);
        return (point->velocity >= UP_SPEED) ? 0.0 : (1.0 - point->velocity / UP_SPEED);
    };

    viz_final_path.Reset();
    viz_final_path.SetRFunc([](void* p) -> float { return 1.0; }); // 红色为主
    viz_final_path.SetGFunc(setColorByVelocity);
    viz_final_path.SetBFunc([](void* p) -> float { return 0.0; });
    
    for (const auto& pt : final_trajectory.trajectory) {
        PathPoint point;
        point.x = pt.x;
        point.y = pt.y;
        point.z = 0.0;
        point.velocity = pt.velocity;
        viz_final_path.inner_points.push_back(point);
    }
    
    if (!viz_final_path.inner_points.empty()) {
        // viz_final_path.publish('to_follow'); 
    }
}
