 # Fusion_follow 模块设计文档

 # 1. 文档信息

 | 项目 | 内容 |
 | :--- | :--- |
 | **模块名称** | Fusion_follow（前向融合跟随） |
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

 **职责**：Fusion_follow 模块负责前向多传感器目标融合及跟车目标选取。其核心职责是：
 - 汇聚前向多路激光雷达点云、毫米波雷达点云、前向相机检测结果、前向激光雷达 3D 检测结果等多源感知信息；
 - 结合自车定位与业务命令（`BusinessCommand`），完成前向目标融合（统一坐标系、目标关联与跟踪）；
 - 输出融合后的目标集合和跟随目标（target object），为规划/控制层提供稳定的前向目标输入。

 **上游模块（输入来源）**：
 - 前向激光雷达点云：`ara::adsfi::MsgLidarFrame`（左前/前/右前）；
 - 前向毫米波雷达点云：`ara::adsfi::SensorRadarPointCloud`（左前/前/右前）；
 - 前向激光雷达 3D 检测结果：`ara::adsfi::MsgHaf3dDetectionOutArray`；
 - 多相机视觉感知结果：`ara::adsfi::MsgCameraVPResult`；
 - 自车定位：`ara::adsfi::MsgHafLocation`；
 - 业务命令：`ara::adsfi::BusinessCommand`（模式、速度、任务信息等）。

 **下游模块（输出去向）**：
 - 感知对象消费方（规划/控制）：订阅融合目标集合 `/tpperceptionobjects`；
 - 跟车控制/ACC：订阅跟随目标 `/tptarget_obj`（目标车辆/障碍物）。

 **对外能力**：
 - 通过 `MsgHafFusionOutArray` 提供融合对象结果（全局/车体坐标下的统一目标列表）；
 - 提供单独的跟随目标输出（同样使用 `MsgHafFusionOutArray` 形式，只包含当前选中的跟随目标）。

 ## 2.2 设计目标

 - **功能目标**：
   - 支持多源输入（雷达、激光、相机、定位、业务命令）的统一接入与缓存；
   - 通过第三方库 `perception_fusion_follow`（`target_finder_entry`）完成目标级融合与跟随目标选取；
   - 对外提供两类输出：融合目标集合（objects）与跟随目标集合（target_obj）。
 - **性能目标**：
   - 采用消息驱动方式（`SetScheduleInfo("message")`），避免固定周期空转，按需输出结果；
   - 满足整车对前向融合更新频率与延迟的要求（由 `perception_fusion_follow` 和上游频率共同决定）。
 - **稳定性目标**：
   - 算法初始化失败时通过 `FaultDiagnosis` 上报错误，并打日志提示；
   - 输入传感器名称异常时给出错误提示并忽略该路数据，而不影响整体运行。
 - **安全目标**：
   - 通过统一的故障诊断接口暴露初始化异常等问题，便于系统级安全评估；
   - 利用定位与业务命令信息进行合理的目标选取，避免明显的目标跳变。
 - **可维护性 / 可扩展性目标**：
   - 融合策略实现封装在 `perception_fusion_follow` 动态库中，接口层仅负责数据路由与封装，便于后续升级；
   - 支持通过配置扩展新的传感器 Topic 名称映射，而不修改核心逻辑。

 ## 2.3 设计约束

 - **硬件平台 / OS / 架构约束**：
   - 面向 mdc 平台，运行在车载 Linux 上；
   - 依赖上游传感器驱动及感知模块输出的稳定性。
 - **中间件 / 框架依赖**：
   - ADSFI 框架：继承 `BaseAdsfiInterface`，使用 `"message"` 驱动模式；
   - 日志与时间同步由 `ApLogInterface` 与平台 `AvosNode` 提供。
 - **第三方库 / 模块依赖**：
   - `perception_fusion_follow`：前向目标融合与跟随目标选取的核心算法库；
   - PCL：点云相关辅助处理；
   - OpenCV：用于可能的图像/投影调试；
   - Proj4：坐标转换相关；
   - `perception_fusion_common`：通用融合工具库；
   - `FaultDiagnosis`：故障诊断与上报。
 - **兼容性 / 版本约束**：
   - `perception_fusion_follow` 与 `perception_fusion_common` 的版本由 `model.cmake` 与工程统一管理；
   - 若变更库版本需验证接口兼容性与行为一致性。

 ---

 # 3. 需求与范围

 ## 3.1 功能需求（FR）

 | 需求ID | 描述 | 优先级 |
 | :--- | :--- | :--- |
 | FR-01 | 接收多路前向激光雷达点云 `MsgLidarFrame`，并映射到内部传感器 ID | 高 |
 | FR-02 | 接收多路前向毫米波雷达点云 `SensorRadarPointCloud`，并映射到内部传感器 ID | 高 |
 | FR-03 | 接收前向激光雷达 3D 检测结果 `MsgHaf3dDetectionOutArray` | 高 |
 | FR-04 | 接收前向相机感知结果 `MsgCameraVPResult` | 高 |
 | FR-05 | 接收自车定位 `MsgHafLocation` | 高 |
 | FR-06 | 接收业务命令 `BusinessCommand`，为跟随策略提供上下文 | 高 |
 | FR-07 | 通过 `perception_fusion_follow` 完成前向目标级融合和跟随目标选取 | 高 |
 | FR-08 | 通过 Topic `tpperceptionobjects` 输出融合目标集合 `MsgHafFusionOutArray` | 高 |
 | FR-09 | 通过 Topic `tptarget_obj` 输出跟随目标集合（通常为 1 个目标）`MsgHafFusionOutArray` | 高 |
 | FR-10 | 在算法初始化失败时，记录错误信息并通过 `FaultDiagnosis` 进行初始化错误上报 | 中 |

 ## 3.2 非功能需求（NFR）

 | 需求ID | 类型 | 指标 | 目标值 |
 | :--- | :--- | :--- | :--- |
 | NFR-01 | 性能 | 单次融合计算时延 | 满足前向融合刷新频率要求（由系统测试确认） |
 | NFR-02 | 稳定性 | 传感器名称异常处理 | 不导致崩溃，仅日志告警并忽略该路数据 |
 | NFR-03 | 可观测性 | 关键输入与输出日志 | 至少在调试级别记录每类输入回调和输出结果的关键字段 |

 ## 3.3 范围界定

 ### 3.3.1 本模块必须实现：

 - 多路传感器输入的统一接入与推送至 `target_finder_entry`；
 - 基于 `target_finder_entry` 的融合结果与跟随目标输出；
 - 初始化失败等关键异常的检测和上报。

 ### 3.3.2 本模块明确不做：

 - 不实现任何底层感知算法（检测/分割/点云聚类等）；
 - 不实现融合算法的内部逻辑（全部在 `perception_fusion_follow` 中）；
 - 不参与路径规划与控制逻辑（只输出对象级结果）。

 ## 3.4 需求-设计-验证映射（示意）

 | 需求ID | 对应设计章节 | 对应接口 | 验证方式 / 用例 |
 | :--- | :--- | :--- | :--- |
 | FR-01 | 5.3 / 7.2 | `Callback(MsgLidarFrame)` | 多路雷达 Topic 回放，检查映射与推送行为 |
 | FR-02 | 5.3 / 7.2 | `Callback(SensorRadarPointCloud)` | 多路毫米波 Topic 回放，检查映射与推送行为 |
 | FR-03 | 5.3 / 7.2 | `Callback(MsgHaf3dDetectionOutArray)` | 验证 3D 激光检测结果被正确推送到 handle |
 | FR-04 | 5.3 / 7.2 | `Callback(MsgCameraVPResult)` | 验证视觉感知结果被正确推送到 handle |
 | FR-05 | 5.3 / 7.2 | `Callback(MsgHafLocation)` | 检查定位信息是否按帧推送 |
 | FR-06 | 5.3 / 7.2 | `Callback(BusinessCommand)` | 业务命令变化是否进入融合模块 |
 | FR-08 | 5.3 / 7.1 | `Process("tpperceptionobjects", ...)` | 测试 `/tpperceptionobjects` 输出是否包含融合目标 |
 | FR-09 | 5.3 / 7.1 | `Process("tptarget_obj", ...)` | 测试 `/tptarget_obj` 输出是否包含跟随目标 |

 ---

 # 4. 设计思路

 ## 4.1 方案概览

 模块采用**消息驱动 + 数据缓冲 + 算法库处理 + 按需输出**的设计：

 ```mermaid
 flowchart TD
     subgraph Inputs["多源输入"]
       Lidar[多路前向 Lidar<br/>MsgLidarFrame]
       Radar[多路前向 Radar<br/>SensorRadarPointCloud]
       LidarObj[Lidar 3D Objects<br/>MsgHaf3dDetectionOutArray]
       Camera[Camera VPResult<br/>MsgCameraVPResult]
       Loc[Location<br/>MsgHafLocation]
       BC[BusinessCommand]
     end

     Lidar --> CbLidar[Callback(Lidar)]
     Radar --> CbRadar[Callback(Radar)]
     LidarObj --> CbLidarObj[Callback(LidarObjs)]
     Camera --> CbCam[Callback(CameraVP)]
     Loc --> CbLoc[Callback(Location)]
     BC --> CbBC[Callback(Business)]

     CbLidar --> Handle[perception_fusion_follow<br/>target_finder_entry]
     CbRadar --> Handle
     CbLidarObj --> Handle
     CbCam --> Handle
     CbLoc --> Handle
     CbBC --> Handle

     Handle --> OutAll[objects_result_deque_<br/>融合对象队列]
     Handle --> OutTarget[target_object_result_deque_<br/>跟随目标队列]

     OutAll --> ProcAll[Process(\"tpperceptionobjects\")]
     OutTarget --> ProcTarget[Process(\"tptarget_obj\")]
 ```

 ## 4.2 关键决策与权衡

 | 决策点 | 选择 | 理由 |
 | :--- | :--- | :--- |
 | 驱动方式 | `SetScheduleInfo("message")` | 按消息驱动，减少无用计算，适应不同输入频率 |
 | 算法集成方式 | 链接 `libperception_fusion_follow.so` 并通过 `target_finder_entry` 调用 | 算法统一封装，易于升级/替换 |
 | 传感器路由 | 通过 `sensor_lidar_remap_` / `sensor_radar_remap_` 做名称到内部 ID 的映射 | 解耦 Topic 名称与算法内部编号，支持配置拓展 |
 | 输出通道 | 使用两个 Topic（融合对象 + 跟随对象） | 满足规划与跟随控制对不同粒度输出的需求 |

 ## 4.3 与现有系统的适配

 - **ADSFI**：继承 `BaseAdsfiInterface`，遵循其 `Init`、`Callback`、`Process` 规范，与其他 meta_model 模块保持一致；
 - **故障诊断**：使用 `FaultDiagnosis` 上报初始化失败等系统级问题；
 - **其它感知模块**：输入来自感知链路（Lidar/Radar/Camera/LidarDetection），输出被规划/控制模块消费。

 ## 4.4 失败模式与降级（概要）

 - 算法初始化失败：在 `Init` 中检测 `handle.InitedFaild()`，上报错误并记录日志，后续可维持空输出或上层降级；
 - 传感器名称不在映射表中：打印错误日志并忽略该路数据，避免崩溃；
 - 融合内部异常：由 `perception_fusion_follow` 内部处理并通过错误信息反馈，接口层视情况记录日志。

 ---

 # 5. 架构与技术方案

 ## 5.1 模块内部架构

 ```mermaid
 graph TD
     A[AdsfiInterface<br/>Fusion_follow] -->|Init| H[target_finder_entry<br/>handle]
     A -->|Callback Lidar| H
     A -->|Callback Radar| H
     A -->|Callback LidarObjs| H
     A -->|Callback CameraVP| H
     A -->|Callback Location| H
     A -->|Callback Business| H

     H --> Q1[objects_result_deque_]
     H --> Q2[target_object_result_deque_]

     A -->|Process(\"tpperceptionobjects\")| Q1
     A -->|Process(\"tptarget_obj\")| Q2
 ```

 - **线程 / 同步模型**：
   - 多个输入 Callback 线程并发写入 `handle` 内部的数据结构；
   - `target_finder_entry` 内部通过自有队列（如 `objects_result_deque_`、`target_object_result_deque_`）管理输出；
   - `Process` 根据请求名称从对应队列阻塞式取出一帧融合结果。

 ## 5.2 关键技术选型

 | 技术点 | 方案 | 选择原因 | 备选方案 |
 | :--- | :--- | :--- | :--- |
 | 融合算法 | `perception_fusion_follow` 库 | 项目已有、可重用，被其它项目验证 | 重新实现融合算法（成本高） |
 | 点云处理 | PCL | 点云邻域、聚类等常规功能 | 自行实现点云算法 |
 | 坐标转换 | Proj4 + 内部工具 | 支持多种地理坐标转换 | 简单自实现（精度与鲁棒性风险） |

 ## 5.3 核心流程（消息驱动）

 1. **初始化流程**：
    - `Init()` 中：
      - 调用 `SetScheduleInfo("message")` 配置消息驱动模式；
      - 初始化日志系统；
      - 创建 `FaultDiagnosis` 对象；
      - 调用 `handle.Init(error_info, error_handle_ptr)`；
      - 若 `handle.InitedFaild()` 返回 true，则通过 `error_handle_ptr->SetInitError(error_info)` 以及日志进行上报。
 2. **输入回调流程**：
    - `Callback(MsgLidarFrame)`：根据 `sensor_lidar_remap_` 将 Topic 名字映射为内部 ID，并调用 `handle.PushPointcloud(lidar_ptr, id)`；
    - `Callback(SensorRadarPointCloud)`：根据 `sensor_radar_remap_` 映射 ID，并调用 `handle.PushRadarcloud(radar_ptr, id)`；
    - `Callback(MsgHaf3dDetectionOutArray)`：调用 `handle.PushLidarObjsData(*lidar_objs_ptr)`；
    - `Callback(MsgHafLocation)`：调用 `handle.PushLocData(*location_ptr)`；
    - `Callback(BusinessCommand)`：调用 `handle.PushBusinessCommandData(*business_ptr)`；
    - `Callback(MsgCameraVPResult)`：调用 `handle.PushVresultData(*vpresult_ptr)`。
 3. **输出流程（Process）**：
    - `Process("tpperceptionobjects", fusion_objs)`：从 `handle.objects_result_deque_` 中阻塞获取一帧融合结果，填充到 `fusion_objs` 并返回 0；
    - `Process("tptarget_obj", fusion_objs)`：从 `handle.target_object_result_deque_` 中阻塞获取跟随目标结果，填充到 `fusion_objs` 并返回 0；
    - 其他 `name`：返回 -1，表示不支持。

 ---

 # 6. 界面设计

 模块无界面，**本节不适用（N/A）**。

 ---

 # 7. 接口设计（概要）

 ## 7.1 对外接口

 | 接口名 | 类型 | 输入 | 输出 | 频率 | 备注 |
 | :--- | :--- | :--- | :--- | :--- | :--- |
 | `Callback(name, std::shared_ptr<MsgLidarFrame>)` | Topic 订阅 | 多路前向 Lidar 点云 | — | 随雷达帧率 | 推入 `handle` 内部缓冲 |
 | `Callback(name, std::shared_ptr<SensorRadarPointCloud>)` | Topic 订阅 | 多路前向 Radar 点云 | — | 随雷达帧率 | 推入 `handle` 内部缓冲 |
 | `Callback(name, std::shared_ptr<MsgHaf3dDetectionOutArray>)` | Topic 订阅 | Lidar 3D 检测结果 | — | 随检测模块输出 | 推入 `handle` |
 | `Callback(name, std::shared_ptr<MsgHafLocation>)` | Topic 订阅 | 自车定位 | — | 按定位模块输出 | 推入 `handle` |
 | `Callback(name, std::shared_ptr<BusinessCommand>)` | Topic 订阅 | 业务命令 | — | 事件触发 | 推入 `handle` |
 | `Callback(name, std::shared_ptr<MsgCameraVPResult>)` | Topic 订阅 | Camera VP 结果 | — | 随视觉模块输出 | 推入 `handle` |
 | `Process(name, std::shared_ptr<MsgHafFusionOutArray>&)` | ADSFI 输出接口 | — | `MsgHafFusionOutArray` | 由下游拉取频率决定 | name 为 `tpperceptionobjects` 或 `tptarget_obj` |

 ## 7.2 对内接口（简要）

 - `target_finder_entry` 提供的主要接口（推断）：
   - `Init(error_info, error_handle_ptr)`：初始化融合跟随模块；
   - `InitedFaild()`：查询初始化结果；
   - `PushPointcloud(lidar_ptr, id)`：推送一帧指定雷达 ID 的点云；
   - `PushRadarcloud(radar_ptr, id)`：推送一帧指定毫米波雷达点云；
   - `PushLidarObjsData(lidar_objs)`：推送一帧激光雷达 3D 对象；
   - `PushLocData(location)`：推送当前定位；
   - `PushBusinessCommandData(business)`：推送业务命令；
   - `PushVresultData(vpresult)`：推送相机感知结果；
   - 内部输出队列：`objects_result_deque_`、`target_object_result_deque_` 供 `Process` 读取。

 ## 7.3 接口稳定性声明

 - **稳定接口（需严格评审）**：
   - 所有 `Callback` / `Process` 签名及对应 Topic 名称；
   - `MsgHafFusionOutArray` 的结构和坐标系约定；
   - `sensor_lidar_remap_`、`sensor_radar_remap_` 的语义（左右前/前/右前）。
 - **非稳定接口（可调整）**：
   - `target_finder_entry` 内部数据结构细节；
   - 输出队列具体实现方式。

 ## 7.4 接口行为契约（示例）

 - `Process("tpperceptionobjects", fusion_objs)`：
   - **前置条件**：`target_finder_entry` 已成功初始化，且内部对象结果队列中已有数据；
   - **后置条件**：`fusion_objs` 包含一帧最新的融合对象结果；
   - **阻塞性**：若队列为空，`waitAndPop` 将阻塞直至有数据；
   - **失败语义**：若 name 不匹配或内部异常，返回 -1。

 ---

 # 8. 数据设计（概要）

 ## 8.1 核心数据结构（外部）

 | 结构体 | 说明 |
 | :--- | :--- |
 | `ara::adsfi::MsgLidarFrame` | 前向激光雷达点云输入 |
 | `ara::adsfi::SensorRadarPointCloud` | 前向毫米波雷达点云输入 |
 | `ara::adsfi::MsgHaf3dDetectionOutArray` | 激光雷达 3D 检测结果输入 |
 | `ara::adsfi::MsgCameraVPResult` | 视觉检测/分割结果输入 |
 | `ara::adsfi::MsgHafLocation` | 自车定位输入 |
 | `ara::adsfi::BusinessCommand` | 业务命令输入 |
 | `ara::adsfi::MsgHafFusionOutArray` | 融合/跟随对象输出 |

 ## 8.2 数据生命周期

 - 输入数据由各上游模块产生，通过 ADSFI 传入本模块，在 `Callback` 中即时推送至 `target_finder_entry` 内部缓存；
 - 融合输出数据由 `target_finder_entry` 生成并存入内部队列，直至被 `Process` 拉取并向外发布。

 ---

 # 9. 异常与边界处理（概要）

 | 异常场景 | 检测方式 | 处理策略 | 是否可恢复 | 上报方式 |
 | :--- | :--- | :--- | :--- | :--- |
 | 融合库初始化失败 | `handle.InitedFaild()` 返回 true | 上报初始化错误，日志记录，后续不产生有效输出 | 是（重启或修复配置后） | `FaultDiagnosis` + 日志 |
 | 输入 Topic 名称不在 `sensor_lidar_remap_` / `sensor_radar_remap_` 中 | Callback 中 map 查找失败 | 日志打印所有合法名称，忽略该路数据 | 是 | ApError 日志 |
 | 输出队列长期无数据 | `waitAndPop` 长时间阻塞 | 依赖上游输入与融合库内部逻辑，必要时增加监控与告警 | 视情况而定 | 监控系统 |

 ---

 # 10. 性能与资源预算（简要）

 - 性能指标与资源使用主要由 `perception_fusion_follow` 算法库决定，本模块自身仅为轻量的适配与路由层；
 - 在系统性能测试中需重点关注：
   - 融合输出更新频率；
   - 单帧融合计算耗时；
   - 对 CPU 与内存占用的整体影响。

 ---

 # 11. 构建与部署（概要）

 - 通过 `fusion_follow/model.cmake` 将 `adsfi_interface/adsfi_interface.cpp` 编译进工程；
 - 链接 `perception_fusion_follow`、`perception_fusion_common`、PCL、OpenCV、Proj4 等库；
 - 部署与启动方式与其它 ADSFI 模块一致，由统一 launcher 或 systemd 管理。

 ---

 # 12. 可测试性与验证（简要）

 - **联调测试**：
   - 与 Lidar/Radar/Camera/LidarDetection/定位模块联合测试，验证多源融合效果；
   - 与规划/控制模块联合测试，验证跟随目标输出的稳定性与合理性。
 - **回放测试**：
   - 使用录制的多传感器数据回放，观察融合对象与跟随目标的时序与轨迹。

 ---

 # 13. 风险分析（概要）

 | 风险 | 影响 | 可能性 | 应对措施 |
 | :--- | :--- | :--- | :--- |
 | 上游某一路传感器长时间缺失 | 融合精度下降 / 某些场景下目标漏检 | 中 | 在融合库内对缺失传感器做降级策略 |
 | Topic 名称配置错误 | 对应传感器数据被忽略 | 中 | 增强配置校验与启动自检 |
 | 融合库行为变更 | 跟随策略与历史版本不一致 | 中 | 升级前进行回归测试与版本记录 |

 ---

 # 14. 设计评审与变更（简要）

 - 接口和 Topic 名称、坐标系约定、输出对象语义等为 L3 级变更，需系统级评审；
 - 内部日志、部分输入映射细节调整为 L1/L2 级变更，可在模块内评审后更新。

