 # DR / Odometry（轮速+IMU 推算定位）模块设计文档
 
 # 1. 文档信息
 
 | 项目 | 内容 |
 | :--- | :--- |
 | **模块名称** | Odometry（DR，轮速+IMU 推算） |
 | **模块编号** | — |
 | **所属系统 / 子系统** | 自动驾驶系统 / 定位层（location_model） |
 | **模块类型** | 平台模块（ADSFI 接口层 + 算法实现库） |
 | **负责人** | — |
 | **参与人** | — |
 | **当前状态** | 草稿 |
 | **版本号** | V1.0 |
 | **创建日期** | 2026-03-05 |
 | **最近更新** | 2026-03-05 |
 
 ---
 
 # 2. 模块概述
 
 ## 2.1 模块定位
 
 **职责**：DR（Dead-Reckoning）模块以 IMU 角速度与四轮轮速为主要输入，通过轻量 EKF 推算车辆在平面上的位姿（x/y/yaw）与速度，并输出统一的 `ara::adsfi::MsgHafLocation`（通常发布为 `/tpododr`），供上层定位融合接口（`Location` 模块）在 `dr_pose` 中使用，实现**主定位缺失/退化时的保底位姿**。
 
 **接口层与实现层关系（必须明确）**：
 
 - `workspace/meta_model/location_model/dr/`：**ADSFI 接口层**
   - 负责 `Init/Callback/Process`、缓存输入、定时触发、对外输出
 - `workspace/meta_model/location_model/dr_basic_src/`：**算法实现层（共享库 `dr_basic_src`）**
   - 负责 DR 核心算法 `DR_(...)`、EKF（IMU+轮速）、故障诊断与关键参数读取
 
 ```mermaid
 flowchart TD
   IMU[/tpsensorimu<br/>MsgHafIMU] --> CbIMU[Callback(IMU)]
   WHEEL[/tpwheelspeed<br/>MsgHafWheelSpeedList] --> CbW[Callback(Wheel)]
   TPIMU[/tpimu<br/>MsgHafLocation] --> CbTP[Callback(TPIMU)]
 
   CbIMU --> BUF[ThreadSafeBuffer]
   CbW --> BUF
   CbTP --> BUF
 
   Timer[20ms Timmer] --> Proc[Process]
   BUF --> Proc
   Proc -->|调用| ALG[DR_(...)<br/>dr_basic_src]
   ALG --> OUT[/tpododr<br/>MsgHafLocation]
 ```
 
 ## 2.2 设计目标
 
 - **功能目标**：
   - 接收 IMU、轮速、TPIMU 位姿（用于高度/姿态补充与 RTK 跳变检测）；
   - 推算并输出 DR 位姿（x/y/yaw）与速度（vx/vy）；
   - 提供离线回放能力（CSV 驱动）与简单仿真数据生成能力，用于联调与自测。
 - **性能目标**：
   - 50Hz（20ms）定时输出，单次 `Process` 计算耗时远小于 20ms；
 - **稳定性目标**：
   - 输入时间戳回退/重复时，不驱动 EKF 反向更新；
   - 关键异常（输入频率异常、轮速/IMU 时间异常、滑移异常、RTK 跳变嫌疑）可上报故障码。
 
 ## 2.3 设计约束
 
 - **框架**：继承 `BaseAdsfiInterface`，定时驱动 `SetScheduleInfo("timmer", 20)`
 - **实现语言**：C++17
 - **系统集成**：组件名为 `Odometry`（见 `workspace/cmake/*.opt`），输出通常绑定到 `/tpododr`
 
 ---
 
 # 3. 需求与范围
 
 ## 3.1 功能需求（FR）
 
 | 需求ID | 描述 | 优先级 |
 | :--- | :--- | :--- |
 | FR-01 | 接收并缓存 IMU（`MsgHafIMU`） | 高 |
 | FR-02 | 接收并缓存轮速（`MsgHafWheelSpeedList`） | 高 |
 | FR-03 | 接收并缓存 TPIMU（`MsgHafLocation`） | 高 |
 | FR-04 | 以 20ms 周期输出 DR 定位（`MsgHafLocation`） | 高 |
 | FR-05 | 支持 CSV 回放：`LoadSimData` + `ProcessFromFile` | 中 |
 | FR-06 | 支持简单仿真输入生成（直线匀速） | 低 |
 | FR-07 | 故障诊断：滑移嫌疑、输入频率异常、时间非单调、参数比值异常、RTK 跳变嫌疑 | 中 |
 
 ## 3.2 非功能需求（NFR）
 
 | 需求ID | 类型 | 指标 | 目标值 |
 | :--- | :--- | :--- | :--- |
 | NFR-01 | 性能 | 输出周期 | 20ms（50Hz） |
 | NFR-02 | 稳定性 | 时间戳异常 | 不反向更新 EKF，保持输出时间单调 |
 | NFR-03 | 可观测性 | 故障上报 | 关键异常能通过 FaultApi 上报 |
 
 ## 3.3 范围界定
 
 ### 3.3.1 本模块必须实现：
 
 - IMU/轮速/TPIMU 输入缓存与 50Hz 输出；
 - `DR_(...)` 算法调用，输出 `MsgHafLocation`；
 - 基础故障诊断与上报能力。
 
 ### 3.3.2 本模块明确不做：
 
 - 不做 GNSS/RTK 融合（仅做 DR 推算，RTK 仅参与一致性检查与姿态补充）；
 - 不对外提供地图坐标转换与地图匹配；
 - 不直接替代上层定位融合（`Location` 模块负责多源定位策略与对外统一输出）。
 
 ## 3.4 需求-设计-验证映射（示意）
 
 | 需求ID | 对应设计章节 | 对应接口 | 验证方式 / 用例 |
 | :--- | :--- | :--- | :--- |
 | FR-01~FR-03 | 7.1 / 5.3 | `Callback(...)` | 回放/在线输入，检查缓存与日志 |
 | FR-04 | 5.3 / 7.1 | `Process` + `DR_` | 在线运行检查 `/tpododr` 连续性与频率 |
 | FR-05 | 12 | `LoadSimData` + `ProcessFromFile` | CSV 回放，检查输出轨迹与序列号 |
 | FR-07 | 9 | `FaultDiagnosis` | 制造异常（时间回退/滑移等），检查故障码上报 |
 
 ---
 
 # 4. 设计思路
 
 ## 4.1 方案概览
 
 以**轮速时间戳（优先）**作为本帧目标时间 `target_time`，将 IMU 在 `last_state_time~target_time` 区间内的样本依次喂入 EKF，然后用轮速对 EKF 做一次更新，得到当前 `x/y/yaw/v/w` 状态并封装输出。
 
 TPIMU 主要用于：
 
 - 对齐补充 `z/roll/pitch`（仅在时间差 < 0.2s 时更新，否则沿用历史值或置 0）；
 - 与 DR 增量做一致性检查，判定 RTK 跳变嫌疑。
 
 ## 4.2 关键决策与权衡
 
 | 决策点 | 选择 | 理由 |
 | :--- | :--- | :--- |
 | 驱动方式 | 定时器 20ms | 下游期望固定频率，便于对齐与监控 |
 | 状态估计 | 轻量 EKF（IMU yaw-rate + differential wheel speed） | 成本低、稳定性高，适合保底 DR |
 | 轮速静止判定 | 轮速门限 + 连续计数进入/退出 | 静止时约束速度/角速度并更新 bias，抑制漂移 |
 | RTK 跳变检测 | 窗口增量 + 帧间增量双判定 | 降低误报，提高可解释性 |
 
 ## 4.3 与现有系统的适配
 
 - 输出 `/tpododr` 由 `Location` 模块订阅并用于 `dr_pose`；
 - 输入 topic 与系统 remap/实例绑定一致（见 `workspace/cmake/share/remap_info/topic_remap.opt`）。
 
 ---
 
 # 5. 架构与技术方案
 
 ## 5.1 模块内部架构
 
 - **接口层**：`workspace/meta_model/location_model/dr/adsfi_interface/adsfi_interface.{h,cpp}`
   - `ThreadSafeBuffer`：缓存 IMU、轮速、TPIMU
   - `Process`：取最新输入 → 调用 `DR_` → 输出 `MsgHafLocation`
   - 支持仿真（内部生成）与 CSV 回放（文件驱动）
 - **实现层（dr_basic_src）**：
   - 核心入口：`DR_(IMU, WHEEL, TPIMU, out_loc)`（位于 `dr_basic_src/src/src/DR_.cpp`）
   - EKF 核心：`BodyImuWheelEkfCore`（`imu_wheel_ekf_core.*`）
   - 故障诊断：`pertarget::FaultDiagnosis`（`fault.h`）
 
 ## 5.2 关键技术点
 
 - **状态向量（EKF）**：
   - \([x,y,yaw,v,w,b_a,b_g]\)，其中 \(b_a/b_g\) 为加速度/角速度 bias（当前主要用于 yaw-rate bias 更新）
 - **数值稳定**：
   - Joseph 形式更新协方差矩阵，降低不正定风险
 - **输入异常容错**：
   - IMU/轮速时间戳回退/重复：不驱动 EKF 反向更新，确保输出时间单调
 - **轮速/IMU 一致性监控**：
   - `w_enc = k_slip * (v_right - v_left) / wheel_base`
   - 对比 `w_enc` 与 IMU 低通 `w_gyro_filt`，触发滑移/参数不合理告警
 
 ## 5.3 核心流程
 
 1. **Init**
    - `SetScheduleInfo("timmer", 20)`
    - `ApLogInterface::Instance()->Init("dr")`
 2. **Callback**
    - IMU → `safebuf_imu_.Set(ptr)`
    - Wheel → `safebuf_wheel_.Set(ptr)`
    - TPIMU → `safebuf_tpimu_.Set(ptr)`
 3. **Process（正常模式）**
    - 从 3 个 buffer 拉取最新输入（任一缺失则返回 -1）
    - 调用 `DR_(imu, wheel, tpimu, out_loc)`
    - 输出 `std::shared_ptr<MsgHafLocation>(out_loc)`
 4. **Process（仿真/回放）**
    - 仿真：内部生成 IMU/Wheel/TPIMU（直线匀速）
    - CSV 回放：`LoadSimData` 载入 → `ProcessFromFile` 逐行喂入
 
 ---
 
 # 6. 界面设计
 
 纯后端模块，无 UI，**本节 N/A**。
 
 ---
 
 # 7. 接口设计（评审重点）
 
 ## 7.1 对外接口
 
 | 接口名 | 类型(API/RPC/Topic) | 输入 | 输出 | 频率 | 备注 |
 | :--- | :--- | :--- | :--- | :--- | :--- |
 | `Callback(name, MsgHafIMU)` | Topic | `/tpsensorimu` | — | 随 IMU | name 当前不使用 |
 | `Callback(name, MsgHafWheelSpeedList)` | Topic | `/tpwheelspeed` | — | 随轮速 | name 当前不使用 |
 | `Callback(name, MsgHafLocation)` | Topic | `/tpimu` | — | 随 TPIMU | name 当前不使用 |
 | `Process(name, MsgHafLocation&)` | 定时输出 | — | `/tpododr`（典型绑定） | 50Hz | name 当前不使用 |
 | `LoadSimData(imu_csv, wheel_csv, tpimu_csv)` | API | CSV 文件 | — | — | 离线回放入口 |
 | `ProcessFromFile(MsgHafLocation&)` | API | — | `MsgHafLocation` | 按样本推进 | 仅离线回放 |
 
 ## 7.2 对内接口（dr_basic_src）
 
 - `void DR_(const MsgHafIMU&, const MsgHafWheelSpeedList&, const MsgHafLocation&, MsgHafLocation&)`
 - `BodyImuWheelEkfCore`：
   - `processImu(stamp, gyro_z, acc_x)`
   - `processWheel(stamp, v_left, v_right, pitch_rad)`
 
 ## 7.3 接口稳定性声明
 
 - **稳定接口（变更必须评审）**：
   - 输入/输出数据类型：`MsgHafIMU`、`MsgHafWheelSpeedList`、`MsgHafLocation`
   - 输出 Topic 语义：`/tpododr` 表示轮速+IMU 推算位姿
 - **非稳定接口（允许调整）**：
   - EKF 噪声参数、门限与告警策略（需版本记录与回归验证）
 
 ## 7.4 接口行为契约
 
 - **前置条件（Process）**：
   - 需要收到至少一帧 IMU、轮速、TPIMU（否则返回 -1）
 - **后置条件（Process 成功）**：
   - 输出 `MsgHafLocation.pose` 与 `dr_pose` 均为 DR 推算的 \(x,y,yaw\)（欧拉角以“度”写入 orientation 字段）
   - `velocity.twist.linear` 输出世界坐标系下的 \(v_x,v_y\)
 - **时间语义**：
   - 输出时间戳优先使用轮速时间戳；异常时回落到 IMU 时间戳，并保证不小于上一帧输出时间
 - **失败语义**：
   - 输入缺失：返回 -1（接口层不会输出新结果）
 
 ---
 
 # 8. 数据设计
 
 ## 8.1 输入数据
 
 - `MsgHafIMU`
   - `imuHeader.timestamp`
   - `angularVelocity.z`（角速度）
   - `linearAcceleration.x/y/z`（用于颠簸强度 proxy 与预留）
 - `MsgHafWheelSpeedList`
   - `header.timestamp`
   - `wheel_speed_vec`（按 `wheelSpeedPos` 区分四轮）
 - `MsgHafLocation`（TPIMU）
   - `pose.pose.position.x/y/z`（用于 RTK 增量对比与姿态补充）
   - `pose.pose.orientation.x/y`（roll/pitch，度；用于 pitch 输入）
 
 ## 8.2 输出数据（`MsgHafLocation`）
 
 - `header.timestamp`：`target_time`
 - `pose.pose.position.(x,y)`：DR 推算平面坐标
 - `pose.pose.orientation.z`：yaw（度）
 - `dr_pose`：镜像 `pose`（当前实现中两者一致）
 - `velocity.twist.linear.(x,y)`：世界坐标速度分量
 - `v`：速度标量（\(|v|\)）
 - `llh`：透传自 TPIMU（用于下游保留经纬度字段一致性）
 
 **注意**：当前实现将 `locationState` 用作“RTK 跳变嫌疑”标志位（`rtk_jump_suspect ? 1 : 0`），系统集成需确认该字段在全栈语义上的一致性。
 
 ---
 
 # 9. 异常与边界处理（评审必查）
 
 | 异常场景 | 检测方式 | 处理策略 | 是否可恢复 | 上报方式 |
 | :--- | :--- | :--- | :--- | :--- |
 | IMU 时间非单调（回退/重复） | 比较 `t_imu` 与 `last_imu_stamp_recv` | 不驱动 EKF 反向更新，必要时跳过样本 | 是 | 3112003 |
 | 输入频率异常/抖动 | EMA 的 `imu_dt_ema/wheel_dt_ema/imu_jitter_ema` 超阈 | 继续输出，但上报告警 | 是 | 3112002 |
 | 轮速滑移嫌疑 | 转弯且 \(|w_enc-w_imu|>0.30\) | 上报告警 | 是 | 3112001 |
 | 参数/比值异常 | `w_enc/w_imu` 比值偏离 1 的 EMA 超阈 | 上报告警（提示 wheel_base/k_slip 等不合理） | 是 | 3112004 |
 | RTK 跳变嫌疑 | RTK 增量与 DR 增量差异超阈（窗口+帧间） | 上报告警，并在输出 `locationState` 标记 | 是 | 3112005 |
 
 ---
 
 # 10. 性能与资源预算（必须可验收）
 
 ## 10.1 性能指标
 
 | 场景 | 指标 | 目标值 | 测试方法 |
 | :--- | :--- | :--- | :--- |
 | 正常运行 | 输出周期 | 20ms | 统计 `/tpododr` 帧间隔 |
 | 正常运行 | 单次计算耗时 | 远小于 20ms | 线上 profiling / 日志采样 |
 
 ## 10.2 资源预算
 
 - CPU：轻量 EKF + 简单门控逻辑（低）
 - 内存：少量缓存（`ThreadSafeBuffer` + IMU deque）
 
 ---
 
 # 11. 构建与部署
 
 ## 11.1 环境依赖
 
 | 依赖项 | 版本要求 | 说明 |
 | :--- | :--- | :--- |
 | 操作系统 | Linux | 车载/开发环境 |
 | Eigen | — | EKF 线代 |
 | glog / yaml-cpp | — | 日志/配置依赖 |
 
 ## 11.2 构建关系
 
 - 接口层：`workspace/meta_model/location_model/dr/model.cmake`
   - 链接：`dr_basic_src`（典型为 `workspace/third_party/dr_basic_src/lib/libdr_basic_src.so`）
   - include：`workspace/third_party/dr_basic_src/include` 与 `dr_basic_src/src/include`
 - 实现库：`workspace/meta_model/location_model/dr_basic_src/CMakeLists.txt`
   - 安装到：`workspace/third_party/dr_basic_src/{lib,include}`
 
 ## 11.3 配置项（实现库读取）
 
 关键参数由实现库从工程配置读取（`CustomStack::GetProjectConfigValue`）：
 
 | 配置键 | 说明 | 默认值 |
 | :--- | :--- | :--- |
 | `vehicle_interface/wheel_base` | 轴距（用于 `w_enc`） | 2.5 |
 | `vehicle_interface/k_slip` | 滑移系数（用于 `w_enc`） | 0.8087（异常范围会被重置为 1.0） |
 | `vehicle_interface/wheel_speed_scale` | 轮速标定比例因子 | 1.014 |
 
 **Topic remap 参考**：`workspace/cmake/share/remap_info/topic_remap.opt` 中定义了 `/tpimu`、`/tpwheelspeed`、`/tpsensorimu`、`/tpododr` 等与平台实例的映射。
 
 ---
 
 # 12. 可测试性与验证
 
 - **在线联调**
   - 确认 IMU/轮速/TPIMU 三路输入存在；
   - 检查 `/tpododr` 频率为 50Hz，且 x/y 连续、yaw 合理；
 - **故障码验证**
   - 制造 IMU 时间回退（回放错序）验证 3112003；
   - 转弯/滑移场景验证 3112001；
   - 调整 `wheel_base/k_slip` 到不合理值验证 3112004；
 - **离线回放**
   - 使用 `LoadSimData(imu.csv, wheel.csv, tpimu.csv)` 加载样本，调用 `ProcessFromFile` 逐帧输出，检查轨迹一致性。
 
 ---
 
 # 13. 风险分析（概要）
 
 | 风险 | 影响 | 可能性 | 应对措施 |
 | :--- | :--- | :--- | :--- |
 | 轮速打滑/轮径误差 | DR 漂移 | 中 | 引入 `wheel_speed_scale/k_slip` 标定与告警 |
 | IMU 抖动/丢帧 | yaw 积分误差 | 中 | 输入频率异常检测 + 静止约束 |
 | RTK/TPIMU 跳变误用 | 影响姿态/一致性检查 | 低~中 | 使用双判定策略并只做告警，不直接改写 DR 轨迹 |
 
 ---
 
 # 14. 附录
 
 - 接口层代码：
   - `workspace/meta_model/location_model/dr/adsfi_interface/adsfi_interface.h`
   - `workspace/meta_model/location_model/dr/adsfi_interface/adsfi_interface.cpp`
 - 实现层代码：
   - `workspace/meta_model/location_model/dr_basic_src/src/src/DR_.cpp`
   - `workspace/meta_model/location_model/dr_basic_src/src/include/imu_wheel_ekf_core.h`
   - `workspace/meta_model/location_model/dr_basic_src/src/include/fault.h`
