 # Fusion_semantic_map 模块技术文档
 
 # 1. 文档信息
 
 | 项目 | 内容 |
 | :--- | :--- |
 | **模块名称** | FusionSemanticMap（语义地图融合 / 静态语义栅格） |
 | **模块编号** | — |
 | **所属系统 / 子系统** | 自动驾驶系统 / 感知融合层（fusion_model） |
 | **模块类型** | 平台模块 |
 | **负责人** | — |
 | **参与人** | — |
 | **当前状态** | 草稿 |
 | **版本号** | V1.0 |
 | **创建日期** | 2026-03-05 |
 | **最近更新** | 2026-03-05 |
 
 ---
 
 # 2. 模块概述
 
 ## 2.1 模块定位
 
 **职责**：`FusionSemanticMap` 模块负责将多路前向 LiDAR 点云与（可选）前向相机语义分割结果、定位与业务模式信息进行融合处理，输出统一的静态语义环境结果 `ara::adsfi::PerceptionStaticEnv`（语义栅格/静态障碍语义对象集合），供下游规划、可视化、诊断等模块消费。
 
 - **上游模块（输入来源）**：
   - 前向 LiDAR 点云：`ara::adsfi::MsgLidarFrame`（左前/前/右前，Topic 名称映射见 7.1）
   - 自车定位：`ara::adsfi::MsgHafLocation`
   - 业务命令：`ara::adsfi::BusinessCommand`（用于坐标模式选择、业务模式等上下文）
   - 视觉结果：`ara::adsfi::MsgCameraVPResult`（可选，配置 `open_vision_seg` 打开后参与融合）
 - **下游模块（输出去向）**：
   - 静态语义环境消费者：订阅 `PerceptionStaticEnv` 输出 Topic（由系统部署/绑定配置决定；接口层对外暴露 `Process(..., std::shared_ptr<PerceptionStaticEnv>&)`）
 - **对外能力**：
   - 提供“静态语义对象集合 + 语义栅格 cell 集合”的统一输出接口，内部融合策略与细节可演进而不改变下游接入方式。
 
 ## 2.2 设计目标
 
 - **功能目标**：
   - 接入多路前向 LiDAR 点云并以主雷达触发一次语义融合计算；
   - 按配置选择是否融合视觉语义分割（可通行区域/扬尘等）并投影到相机坐标用于可视化；
   - 输出 `PerceptionStaticEnv`：包含静态语义对象（`semantic_objs`）及每个对象的栅格 cell 列表；
   - 支持输出坐标系切换（DR / UTM / 业务模式）与定位坐标旋转修正（90° 旋转场景）。
 - **性能目标**：
   - 消息驱动（主雷达帧触发）计算，避免固定周期空转；
   - `Process` 从结果队列阻塞取数，保证下游取到的都是已完成融合的一帧结果。
 - **稳定性目标**：
   - 初始化/配置失败可检测并上报（初始化错误、处理超时）；
   - 输入 LiDAR Topic 名称异常时仅忽略该路输入并给出可用名称提示，不崩溃。
 - **可维护性 / 可扩展性目标**：
   - 接口层（`AdsfiInterface`）只做数据路由与结果出队，核心融合逻辑封装在 `libperception_semantic_map.so`（`SemanticMLidarFuse`）中，便于升级替换。
 
 ## 2.3 设计约束
 
 - **硬件平台 / OS**：车载 Linux（工程同时支持 x86 与 mdc 交叉编译环境，具体由构建选项决定）
 - **框架约束**：继承 `BaseAdsfiInterface`，使用 ADSFI 的 Callback + Process 模式
 - **关键依赖（见 `model.cmake`）**：
   - `libperception_semantic_map.so`（语义融合核心库）
   - `perception_fusion_common`（信号量、RLE 解压等通用能力）
   - PCL / OpenCV / Proj4 / glog / yaml-cpp 等
 
 ---
 
 # 3. 需求与范围
 
 ## 3.1 功能需求（FR）
 
 | 需求ID | 描述 | 优先级 |
 | :--- | :--- | :--- |
 | FR-01 | 接收三路前向 LiDAR 点云 `MsgLidarFrame`，按 Topic 名称映射到内部雷达 ID 并缓存 | 高 |
 | FR-02 | 接收定位 `MsgHafLocation` 并推入内部缓存（必要时做坐标旋转修正） | 高 |
 | FR-03 | 接收 `BusinessCommand` 并推入内部缓存，作为坐标模式/业务模式上下文 | 高 |
 | FR-04 | 接收 `MsgCameraVPResult`（可选），对压缩语义结果做解码并缓存 | 中 |
 | FR-05 | 以主雷达（ID=0）作为触发信号，驱动内部融合线程生成一帧 `PerceptionStaticEnv` | 高 |
 | FR-06 | 对外提供 `Process` 接口，阻塞式输出一帧最新语义融合结果 | 高 |
 | FR-07 | 检测处理超时（>150ms）并上报超时错误 | 中 |
 
 ## 3.2 非功能需求（NFR）
 
 | 需求ID | 类型 | 指标 | 目标值 |
 | :--- | :--- | :--- | :--- |
 | NFR-01 | 性能 | 单帧处理耗时 | 以系统测试验收为准；超过 150ms 触发超时上报 |
 | NFR-02 | 稳定性 | 输入异常 | Topic 名称不匹配：告警 + 忽略，不崩溃 |
 | NFR-03 | 可观测性 | 日志与错误上报 | 初始化失败/处理超时等可观测 |
 
 ## 3.3 范围界定
 
 ### 3.3.1 本模块必须实现：
 
 - 多路点云/定位/业务/视觉语义结果的接入与缓存；
 - 主雷达触发语义融合线程；
 - 阻塞式输出 `PerceptionStaticEnv`；
 - 初始化失败与处理超时的基础上报。
 
 ### 3.3.2 本模块明确不做：
 
 - 不在接口层实现语义融合算法本体（由 `libperception_semantic_map.so` 实现）；
 - 不负责传感器驱动、标定文件生成等上游工作；
 - 不承担规划/控制决策，只输出静态语义环境结果。
 
 ---
 
 # 4. 设计思路
 
 ## 4.1 方案概览
 
 模块采用**“多输入回调缓存 + 主雷达触发 + 内部融合线程 + 结果队列输出”**的设计：
 
 ```mermaid
 flowchart TD
     subgraph Inputs["上游输入"]
       LFL[左前 LiDAR<br/>MsgLidarFrame] --> CbLidar
       LFM[前向 LiDAR(主雷达)<br/>MsgLidarFrame] --> CbLidar
       LFR[右前 LiDAR<br/>MsgLidarFrame] --> CbLidar
       LOC[Location<br/>MsgHafLocation] --> CbLoc[Callback(Location)]
       BC[BusinessCommand] --> CbBC[Callback(Business)]
       VP[Camera VPResult<br/>MsgCameraVPResult] --> CbVP[Callback(VPResult)]
     end
 
     CbLidar[Callback(Lidar)] -->|CheckLidarData(i)| Core[SemanticMLidarFuse<br/>libperception_semantic_map]
     CbLoc -->|PushLocData| Core
     CbBC -->|PushBusinessCommandData| Core
     CbVP -->|PushVresultData| Core
 
     Core -->|主雷达触发 Post()| Sem[process_semantic_sem_]
     Sem --> Thr[语义融合线程<br/>ProcessFollowThread]
     Thr --> Q[static_env_result_deque_<br/>结果队列]
     Q --> Proc[Process(name, out)]
     Proc --> Down[下游发布/消费<br/>PerceptionStaticEnv]
 ```
 
 ## 4.2 关键决策与权衡
 
 | 决策点 | 选择 | 理由 |
 | :--- | :--- | :--- |
 | 驱动方式 | 主雷达触发（ID=0） | 以最稳定/最关键输入作为“帧边界”，降低多源同步复杂度 |
 | 输出方式 | 阻塞式结果队列 `waitAndPop` | 保证输出时一定有结果，避免下游拿到空数据 |
 | 语义融合实现 | 动态库封装（`libperception_semantic_map.so`） | 接口层轻量、算法可独立升级，降低耦合 |
 
 ## 4.3 失败模式与降级
 
 - 初始化/配置失败：内部线程检测 `init_error_str_` 并上报初始化错误，持续跳过处理；
 - 输入时间戳回退：检测到某路 LiDAR 时间回退时清空内部缓存，避免时序错乱导致异常输出；
 - 处理超时：若一次融合处理耗时 >150ms，上报 “ProcessTimeout” 错误，便于系统监控与定位。
 
 ---
 
 # 5. 架构与技术方案
 
 ## 5.1 模块内部架构
 
 - **接口层**：`workspace/meta_model/fusion_model/fusion_semantic_map/adsfi_interface/adsfi_interface.{h,cpp}`
   - 负责：初始化日志、构建核心单例、接收回调、输出结果
 - **核心融合库**：`libperception_semantic_map.so`
   - 关键类：`SemanticMLidarFuse`
   - 负责：配置读取、数据缓存、语义融合线程、结果入队、错误上报
 
 ## 5.2 线程 / 同步模型
 
 - 多路 `Callback` 可能并发调用，分别将数据推入内部缓存；
 - `SemanticMLidarFuse` 内部启动独立线程 `ProcessFollowThread`：
   - 通过信号量 `process_semantic_sem_` 等待主雷达触发；
   - 每次唤醒后拉取同步后的多源数据并执行融合；
   - 融合结果以 `std::shared_ptr<PerceptionStaticEnv>` 推入 `static_env_result_deque_`；
 - 对外 `Process` 使用 `waitAndPop` 阻塞取数。
 
 ## 5.3 核心流程（简述）
 
 - **Init**：
   - `AdsfiInterface::Init()`：
     - `SetScheduleInfo("message")`
     - 初始化日志：`ApLogInterface::Instance()->Init("XFusion_semantic_map")`
     - 获取核心单例：`SemanticMLidarFuse::GetInstance()`
 - **回调输入**：
   - `Callback(MsgLidarFrame)`：校验 Topic 名称 → 映射内部雷达 ID → `CheckLidarData(lidar_ptr, id)`
   - `Callback(MsgHafLocation)`：`PushLocData(location_ptr)`
   - `Callback(BusinessCommand)`：`PushBusinessCommandData(*business_ptr)`
   - `Callback(MsgCameraVPResult)`：`PushVresultData(*vpresult_ptr)`（若 `open_vision_seg` 开启则解码并缓存）
 - **内部融合线程**（核心库）：
   - 主雷达（ID=0）新帧触发 `process_semantic_sem_.Post()`
   - 线程 `Wait()` 后拉取数据（定位/业务/视觉/多雷达），生成 `PerceptionStaticEnv`，完成必要坐标变换/投影，再将结果入队
 - **对外输出**：
   - `AdsfiInterface::Process(...)`：从 `static_env_result_deque_` 阻塞取出一帧 `PerceptionStaticEnv`
 
 ---
 
 # 6. 界面设计
 
 模块无界面，**本节不适用（N/A）**。
 
 ---
 
 # 7. 接口设计（评审重点）
 
 ## 7.1 对外接口（ADSFI）
 
 | 接口名 | 类型(API/RPC/Topic) | 输入 | 输出 | 频率 | 备注 |
 | :--- | :--- | :--- | :--- | :--- | :--- |
 | `Init()` | API | — | — | 启动时 1 次 | 初始化日志与核心单例 |
 | `Callback(name, std::shared_ptr<MsgLidarFrame>)` | Topic | LiDAR 点云 | — | 随雷达帧率 | 依赖 `sensor_lidar_remap_` 映射，主雷达触发融合 |
 | `Callback(name, std::shared_ptr<MsgHafLocation>)` | Topic | 定位 | — | 随定位频率 | 推入定位缓存 |
 | `Callback(name, std::shared_ptr<BusinessCommand>)` | Topic | 业务命令 | — | 事件/周期 | 推入业务缓存 |
 | `Callback(name, std::shared_ptr<MsgCameraVPResult>)` | Topic | 视觉 VP 结果 | — | 随视觉频率 | 可选参与（`open_vision_seg`） |
 | `Process(name, std::shared_ptr<PerceptionStaticEnv>&)` | Topic/Output | — | 语义地图结果 | 由下游拉取/发布频率决定 | 当前实现忽略 `name`，始终从结果队列取 1 帧 |
 
 **LiDAR Topic 名称映射（接口层硬编码）**：
 
 | Topic 名称（name） | 内部雷达 ID | 语义 |
 | :--- | :--- | :--- |
 | `tpfront_middle_lidar_points` | 0 | **主雷达（触发融合）** |
 | `tpfront_left_lidar_points` | 1 | 左前雷达 |
 | `tpfront_right_lidar_points` | 2 | 右前雷达 |
 
 ## 7.2 对内接口（核心库 `SemanticMLidarFuse`）
 
 - **单例与线程**：
   - `SemanticMLidarFuse::GetInstance()`：创建实例并读取配置，启动 `ProcessFollowThread`
 - **数据输入**：
   - `bool CheckLidarData(std::shared_ptr<MsgLidarFrame>, int lidar_id)`：缓存点云，检测时间回退；`lidar_id==0` 时触发信号量
   - `bool PushLocData(std::shared_ptr<MsgHafLocation>)`：定位转换与缓存
   - `bool PushBusinessCommandData(const BusinessCommand&)`：业务缓存
   - `bool PushVresultData(const MsgCameraVPResult&)`：可选解码与缓存（RLE 解压）
 - **数据输出**：
   - `ThreadSafeQueue<std::shared_ptr<PerceptionStaticEnv>> static_env_result_deque_`：融合结果队列
 
 ## 7.3 接口稳定性声明
 
 - **稳定接口（变更必须评审）**：
   - `PerceptionStaticEnv` 的输出语义（坐标系约定、`semantic_objs` 字段含义）
   - LiDAR Topic 名称映射表（`tpfront_*_lidar_points`）及“主雷达触发”机制
   - `Callback/Process` 的签名与数据类型
 - **非稳定接口（允许调整但需标注）**：
   - `libperception_semantic_map.so` 内部算法细节、内部缓存结构、具体阈值策略等
 
 ## 7.4 接口行为契约（必须填写）
 
 - `Callback(name, MsgLidarFrame)`：
   - **前置条件**：`name` 必须在映射表内；点云时间戳需单调递增（至少不回退）
   - **后置条件**：点云被缓存；若为主雷达且新帧有效，则触发一次融合线程唤醒
   - **失败语义**：`name` 不合法时，输出错误日志并直接 `return`（不抛异常）
 - `Process(name, PerceptionStaticEnv&)`：
   - **前置条件**：内部融合线程已经启动；至少产生过一帧结果
   - **后置条件**：`semanticmap_ptr` 被填充为一帧最新结果
   - **阻塞性**：当结果队列为空时，`waitAndPop` 将阻塞直到有结果
   - **失败语义**：当前实现返回 0；若内部线程永不产出结果则会长期阻塞（系统集成侧需确保主雷达输入与配置正确）
 
 ---
 
 # 8. 数据设计
 
 ## 8.1 输出数据结构（`PerceptionStaticEnv`）
 
 输出包含：
 
 - `header`：时间戳（通常取主雷达时间）、模块名、数据名、frame_id 等
 - `cell_size`：栅格分辨率（默认 0.25m，具体取配置 `voxel_size`）
 - `semantic_objs`：语义对象数组，每个对象包含：
   - `type`：语义类别 ID
   - `x/y/z` 与 `obj_size`：对象中心与尺寸
   - `cells[]`：对象由哪些栅格 cell 组成（cell 内包含 `x/y/dh` 等）
 
 **静态栅格类别（示例，来源于核心库定义）**：
 
 - 13：车/人
 - 15：扬尘
 - 16：负障碍物
 - 70：图像可通行区域（IPM 投影的可通行区域）
 - 40：视觉赋值的栅格属性类别（锥桶/隔离柱/标志牌等）
 - 255：未知
 
 ## 8.2 数据生命周期
 
 - 输入数据：回调到达即缓存（按时间戳去重/清理回退）
 - 输出数据：融合线程生成 `PerceptionStaticEnv` 后入队，直至被 `Process` 拉取并发布/消费
 
 ---
 
 # 9. 异常与边界处理（评审必查）
 
 | 异常场景 | 检测方式 | 处理策略 | 是否可恢复 | 上报方式 |
 | :--- | :--- | :--- | :--- | :--- |
 | LiDAR Topic 名称错误 | `sensor_lidar_remap_` 查找失败 | 打印可用名称，忽略该路输入 | 是 | 日志（`ApError("fusion_semantic_map")`） |
 | LiDAR 时间戳回退 | `time < last_time` | 清空内部缓存并重新开始 | 是 | 日志 |
 | 初始化/配置失败 | `init_error_str_` 非空 | 上报初始化错误，跳过处理 | 需修复配置/重启 | 错误处理器（核心库）+ 日志 |
 | 处理超时 | \((send\_stamp-start\_stamp)>0.15\) | 上报超时错误并允许后续恢复 | 是 | `SetTopicTimeOut`/`ResetError` |
 
 ---
 
 # 10. 性能与资源预算（必须可验收）
 
 ## 10.1 性能指标（建议验收点）
 
 | 场景 | 指标 | 目标值 | 测试方法 |
 | :--- | :--- | :--- | :--- |
 | 主雷达触发融合 | 单帧融合耗时 | < 150ms（否则上报超时） | 回放数据统计 `start_stamp/send_stamp` 差值 |
 | 下游取数 | 输出阻塞等待时长 | 可控（由输入触发决定） | 监控 `Process` 阻塞时间与队列积压 |
 
 ## 10.2 资源预算（定性）
 
 - CPU：主要消耗在栅格提取、聚类、视觉融合与投影、跟踪/记忆逻辑（由配置开关影响）
 - 内存：多路点云与视觉缓存 + 输出队列（系统集成需关注队列积压）
 
 ---
 
 # 11. 构建与部署
 
 ## 11.1 环境依赖
 
 | 依赖项 | 版本要求 | 说明 |
 | :--- | :--- | :--- |
 | 操作系统 | Linux | 车载/开发环境 |
 | 核心动态库 | `libperception_semantic_map.so` | 语义融合核心 |
 | 依赖库 | PCL/OpenCV/Proj4/yaml-cpp/glog | 由 `model.cmake` 链接 |
 
 ## 11.2 构建要点
 
 - 源码入口：`adsfi_interface/adsfi_interface.cpp`
 - 关键 include/link 由 `workspace/meta_model/fusion_model/fusion_semantic_map/model.cmake` 指定
 
 ## 11.3 配置项（核心库读取的关键键）
 
 核心库通过 `CustomStack::GetProjectConfigValue(...)` 读取工程配置，主要包括：
 
 - `vehicle_interface/body_base_height`
 - `vehicle_interface/car_front` / `car_back` / `car_left` / `car_right`（自车区域）
 - `perception/XSemanticMapMLidars/lidar_num`
 - `perception/XSemanticMapMLidars/lidar_i_max_x/min_x/max_y/min_y/filter_base`（i 从 0 到 lidar_num-1）
 - `perception/XSemanticMapMLidars/lidar_max_z` / `lidar_min_z`
 - `perception/XSemanticMapMLidars/voxel_size` / `ground_dh` / `filter_hung_dh`
 - `perception/XSemanticMapMLidars/remember_dis`
 - `perception/XSemanticMapMLidars/delete_width` / `delete_length` / `delete_height`
 - `perception/XSemanticMapMLidars/force_detect_edge`（>0.01m 生效，建议值 0.1）
 - `perception/XSemanticMapMLidars/open_vision_seg`（视觉语义融合开关）
 - `perception/XSemanticMapMLidars/vision_seg_front_cam`
 - `perception/XSemanticMapMLidars/vision_seg_front_distortion_cam`
 - `perception/is_rotate_location_coordinate`
 - `perception/XSemanticMapMLidars/is_out_to_utm`
 - `perception/XSemanticMapMLidars/is_out_nobjs`
 - `perception/manual_loc_mode`（`dr` / `utm` / `business_cmd`）
 
 另外，模块的 `global.conf` 中存在 `FusionSemanticMap/config_path_prefix` 占位配置项，用于约定读取配置的路径前缀（如需要可在系统集成中启用）。
 
 ## 11.4 启动与日志
 
 - 日志节点名：`FusionSemanticMap`（见 `workspace/config/log.conf`）
 - 接口层初始化 tag：`XFusion_semantic_map`
 - 算法日志 tag：`fusion_semantic_map`
 
 ---
 
 # 12. 可测试性与验证（建议）
 
 - **回放验证**：
   - 回放三路 LiDAR + 定位 + 业务命令 + 视觉（可选）数据，检查输出 `semantic_objs` 数量、类别、坐标是否合理；
 - **边界测试**：
   - 故意输入错误 LiDAR name，验证不会崩溃且日志提示合法 name；
   - 人为制造时间戳回退，验证内部缓存清空逻辑触发；
   - 打开 `open_vision_seg` 并输入压缩 segmentation，验证解码与投影路径正常。
 
 ---
 
 # 13. 风险分析（概要）
 
 | 风险 | 影响 | 可能性 | 应对措施 |
 | :--- | :--- | :--- | :--- |
 | 主雷达缺失或 Topic 名称不匹配 | 融合线程不触发，`Process` 长时间阻塞 | 中 | 启动自检/监控：检查主雷达输入与队列产出 |
 | 配置键缺失/错误 | 初始化失败或输出异常 | 中 | 配置项清单化管理，启动阶段输出配置打印与错误上报 |
 | 处理耗时抖动 | 影响下游延迟 | 中 | 使用超时上报，系统层面做性能回归与资源隔离 |
 
 ---
 
 # 14. 附录
 
 - 代码位置：
   - 接口层：`workspace/meta_model/fusion_model/fusion_semantic_map/adsfi_interface/adsfi_interface.h`
   - 构建配置：`workspace/meta_model/fusion_model/fusion_semantic_map/model.cmake`
 - 参考实现/写法：
   - `workspace/meta_model/fusion_model/fusion_follow/design_fusion_follow.md`
