# 事件触发式录包系统（Event Triggered Recorder）

## 需求

使用 rtfbag 进行录包，rtfbag 使用方式如下：

```
Usage: rtfbag record EVENT1 [EVENT2 EVENT3 ...]                               

Record a bag file with the contents of specified events.                      

Options:                                                                      
  -h, --help            show this help message and exit                       
  -a, --all             record all events                                     
  -o PREFIX, --output-prefix=PREFIX                                           
                        prepend PREFIX to beginning of bag name (name will    
                        always end with date stamp)                           
  -O NAME, --output-name=NAME                                                 
                        record to bag with name NAME.bag                      
  --split               split the bag when maximum size or duration is reached
  --max-splits=MAX_SPLITS                                                     
                        Keep a maximum of N bag files, when reaching the      
                        maximum erase the oldest one to keep a constant number
                        of files.                                             
  --size=SIZE           record a bag of maximum size SIZE MB. (Default:       
                        infinite)                                             
  --duration=DURATION   record a bag of maximum duration DURATION in seconds, 
                        unless 'm', or 'h' is appended.                       
  -b SIZE, --buffsize=SIZE                                                    
                        use an internal buffer of SIZE MB (Default: 256, 0 =  
                        infinite)                                             
  -l NUM, --limit=NUM   only record NUM messages on each event                
  -p PATH, --path=PATH  specify the save path of recorded bag files           
  --someip-network=SOME/IP_NETWORK                                            
                        specify SOME/IP network name                          
  --dds-network=DDS_NETWORK                                                   
                        specify DDS network IP                                
  --skip-frame=NUM      record one for every NUM messages on each event       
  --compression=TYPE    use compression (gzip or zlib)                        
  --uuid                specify uuid                                          
  --domain-id           specify DDS domain id                                 
  --ip                  specify IP for filter                                 
  --sharePool           specify sharePool groups  
```

要求：

1. 调主外部程序 rtfbag 进行录包，并存放到指定目录（目录可配置），录包指定录制时长（可配置）
2. 这个目录会指定限额，同时指定开始清理的阈值，目录内文件总大小超过这个清理阈值时开始清理旧文件；
3. 录包过程中，如果没有收到任何触发事件（如故障、手动录制请求），则这个包录制完成后立即删除；
4. 录制的 topic 也可配置
5. 考虑到事件发生时间是随机的，这种情况下需要保留事件发生时刻正在录制的文件的前后各一个文件；
6. 系统当前所有的故障以及故障代码会通过  ara::adsfi::FaultData 这个参数传进来，大约是 10 Hz 的频率。
    ```
    #include "ara/adsfi/impl_type_faultdata.h"
    ```
7. 因为只有请求录包的命令，而没有结束录包的命令。所以我的想法是 HMI 在发送请求录包的命令后，保持 5 秒这样一个周期。会有一个从 0 到 1 的上升沿，然后执行 5 秒后，会有一个从 1 到 0 的下降沿。所以实际上，我们自身会有 start、stop 语义。如果用户在 这 5 秒内再次按按钮，仍然保持为 1 。"start/stop 语义"是否意味着需要额外操作？不需要，因为我们的录包是持续运行的，start 只是标记文件保留的触发点，下降沿不需要做任何事情。

配置如下：

```
on_demand:
  recorder:
    # 需要记录的 topic 列表
    topics:
      - "/tpimu"
      - "/ivsensorgps"
      - "/tpimu_in"
      - "/wheel_speed"
      - "/tpvehiclestate"
      - "front_left_lidar_packets"
      - "front_middle_lidar_packets"
      - "front_right_lidar_packets"
      - "front_middle_radar_packets"
      - "front_left_radar_packets"
      - "front_right_radar_packets"
      - "compressed_image_list"
      - "/tpappremotecmd"
      - "/tpbusiness_cmd"
      - "video_profile"
      - "video_parameter"
      - "/tptask_cmd"
      - "/tppathplan"
      - "/tppathplan_rc"
      - "/tppathplan_auto"
      - "/tppathplan_path_following"
      - "/tppathplan_person_following"
      - "/tpvehicle_to_can"
      - "/tpvehicle_to_can_rc"
      - "/tpvehicle_to_can_auto"
      - "/tpvehicle_to_can_pre_aeb"
      - "/tpplanningstatus"
      - "/tpplanningstatus_rc"
      - "/tpplanningstatus_auto"
      - "/tpplanningstatus_path_following"
      - "/tpplanningstatus_person_following"
      - "/tpsemanticmap"
      - "/tpperceptionobjects"
      - "/tpleftboundary"
      - "/tplidarobjects"
      - "/tprightboundary"
      - "/tptarget_obj"
      - "/perception/vpresult"
      - "image_list"
    # 输出路径
    output: "/opt/usr/records/trigger/"
    # 缓存大小（单位：KB）
    buffer_size: 1536
    # 单个文件时长（单位：秒）
    duration: 60
    # 存储空间配额（单位：MB）,
    quota: 10240
    # 清理存储空间的阈值（单位：MB），不能大于 quota 值
    clean_threshold: 8192
    # 需要自动触发的故障码
    fault:
      fault_codes:
        - "202"
        - "301"
        - "302"
        - "306"
```

实现一个类完成上述需求。


## 概述

这是一个基于事件触发的自动录包系统，用于自动驾驶场景下的数据记录和故障复现。系统持续进行滚动录包，只保留触发事件（故障或手动请求）发生时的相关数据。

## 核心特性

### 1. 滚动录包
- 使用 `rtfbag` 持续录包，每个文件固定时长（默认60秒）
- 后台线程自动管理录包进程的启动和停止

### 2. 事件触发保留
当以下任一事件发生时，系统会保留相关的录包文件：
- **故障触发**：系统检测到配置的故障码（如 202, 301, 302, 306）
- **手动触发**：用户通过 HMI 手动请求录包

### 3. 智能文件管理
- **前后文保留**：触发时保留当前文件及前后各一个文件（共3个文件，约180秒数据）
- **自动清理**：未触发的文件录制完成后立即删除
- **空间管理**：目录总大小超过阈值时，自动清理最旧的文件

### 4. 去重机制
- **故障去重**：每个故障码在60秒内只触发一次
- **手动触发去重**：检测上升沿（0→1），避免重复触发

## 架构设计

```
┌──────────────────────────────────────────────────────────────┐
│                      AdsfiInterface                          │
│  (10Hz 调用)                                                  │
│                                                              │
│  ┌────────────────┐        ┌──────────────────────┐        │
│  │  Process()     │───────>│  updateFaultData()   │        │
│  │  (10Hz)        │        │  updateManualTrigger()│       │
│  └────────────────┘        └──────────────────────┘        │
└──────────────────────────────────────────────────────────────┘
                                    │
                                    v
┌──────────────────────────────────────────────────────────────┐
│              EventTriggeredRecorder                          │
│                                                              │
│  ┌─────────────────────────────────────────────────┐        │
│  │  recordingLoop() (后台线程)                      │        │
│  │  ┌──────────────────────────────────────────┐   │        │
│  │  │  launchRtfbag()                          │   │        │
│  │  │  waitForRecordingComplete()              │   │        │
│  │  │  handleCompletedSession()                │   │        │
│  │  │  cleanOldFilesIfNeeded()                 │   │        │
│  │  └──────────────────────────────────────────┘   │        │
│  └─────────────────────────────────────────────────┘        │
│                                                              │
│  ┌─────────────────┐    ┌──────────────────┐               │
│  │  Session Window │    │  Files To Keep   │               │
│  │  (滑动窗口)      │    │  (保留集合)       │               │
│  └─────────────────┘    └──────────────────┘               │
└──────────────────────────────────────────────────────────────┘
                                    │
                                    v
                            ┌──────────────┐
                            │   rtfbag     │
                            │   进程池      │
                            └──────────────┘
```

## 工作流程

### 录包流程
```
[启动] → [创建Session 0] → [启动rtfbag进程]
   ↓
[等待60秒] → [检查是否被触发]
   ↓                ↓
[是] → [保留文件]    [否] → [删除文件]
   ↓                ↓
[创建Session 1] → [启动rtfbag进程]
   ↓
[循环...]
```

### 触发流程
```
[收到故障数据/手动信号] (10Hz)
   ↓
[检查是否满足触发条件]
   ↓
[是] → [标记当前Session及前后各一个]
   ↓
[文件被保留，不会删除]
```

### 文件标记示例
```
时间线：
[文件A: 0-60s] [文件B: 60-120s] [文件C: 120-180s] [文件D: 180-240s]
                      ↑
                 故障@110s (在文件B录制期间)

标记结果：
- 文件A：标记保留（前一个）
- 文件B：标记保留（当前）
- 文件C：预标记保留（后一个）
- 文件D：未标记（如果没有新触发，将被删除）
```

## 配置说明

配置文件路径：`/home/idriver/ap_adsfi/workspace/config/event_triggered_recorder.proj_cfg`

配置通过 `CustomStack` 系统加载，使用标准的项目配置格式。

```yaml
system:
  recorder:
    trigger:
      topics:                 # 需要录制的 topic 列表
        - "/tpimu"
        - "/ivsensorgps"
        # ...
      output: "/opt/usr/records/trigger/"  # 输出目录
      buffer_size: 1536       # rtfbag 缓存大小（KB）
      duration: 60            # 单个文件时长（秒）
      quota: 10240            # 磁盘配额（MB）
      clean_threshold: 8192   # 清理阈值（MB）
      fault:
        fault_codes:          # 触发的故障码列表
          - "202"
          - "301"
          - "302"
          - "306"
```

## 使用方法

### 1. 编译

```bash
# 在项目根目录
mkdir build && cd build
cmake ..
make
```

### 2. 配置

编辑配置文件 `config/event_triggered_recorder.proj_cfg`，设置：
- 需要录制的 topics
- 输出路径
- 磁盘空间限制
- 触发的故障码

配置通过项目的 `CustomStack` 系统自动加载，启动前确保配置文件已部署到正确位置。

### 3. 运行

系统会自动启动，通过 adsfi_interface 集成到主程序中。

### 4. 集成手动触发

在 `adsfi_interface.cpp` 的 `updateManualTriggerSignal()` 中实现手动触发信号的获取：

```cpp
void AdsfiInterface::updateManualTriggerSignal() {
    // 从 HMI 或其他模块获取手动触发信号
    double signal = getManualTriggerFromHMI();  // 0 或 1

    if (recorder_) {
        recorder_->updateManualTrigger(signal);
    }
}
```

或通过 Callback 接收：

```cpp
void AdsfiInterface::Callback(const std::string& name,
                               const std::shared_ptr<ManualTriggerMsg>& msg) {
    if (msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        manual_trigger_signal_ = msg->trigger_signal;  // 0 或 1
    }
}
```

## 核心类说明

### EventTriggeredRecorder

主要接口：

```cpp
// 启动录包器
void start();

// 停止录包器
void stop();

// 更新故障数据（10Hz调用）
void updateFaultData(const ara::adsfi::FaultData& fault_data);

// 更新手动触发信号（10Hz调用，0 或 1）
void updateManualTrigger(double trigger_signal);
```

### 线程安全

所有公共接口都是线程安全的，使用 mutex 保护共享状态。

## 故障排查

### 1. 录包未启动

- 检查 rtfbag 是否在 PATH 中：`which rtfbag`
- 检查输出目录权限：`ls -la /opt/usr/records/trigger/`
- 查看日志：`/opt/usr/zxz/log/`

### 2. 文件未保留

- 确认故障码在配置中：检查 `fault_codes` 列表
- 确认触发时机：查看日志中的 "Fault triggered" 或 "Manual trigger detected"
- 确认去重：同一故障码60秒内只触发一次

### 3. 磁盘空间问题

- 确认 `clean_threshold` < `quota`
- 检查磁盘实际可用空间：`df -h /opt/usr/records/`
- 查看清理日志："Directory size exceeds threshold"

## 性能考虑

- **CPU占用**：后台线程 + rtfbag进程，正常情况下 < 10% CPU
- **内存占用**：约 50MB（主要是 rtfbag 的缓存）
- **磁盘写入**：取决于 topics 的数据量，典型约 5-10 MB/s
- **调用频率**：10Hz（100ms周期），足够响应触发事件

## 扩展功能

### 自定义触发条件

修改 `shouldTriggerByFault()` 方法，添加自定义逻辑：

```cpp
bool EventTriggeredRecorder::shouldTriggerByFault(const ara::adsfi::FaultData& fault_data) {
    // 自定义逻辑：例如检查故障等级
    if (fault_data.fault_level >= 3) {
        return true;
    }

    // 原有逻辑
    // ...
}
```

### 添加新的触发源

扩展接口，添加新的触发方法：

```cpp
// 例如：速度触发
void updateVelocityTrigger(double velocity) {
    if (velocity > 80.0) {  // 超速触发
        markFilesForKeep(current_session_index_);
    }
}
```

## 许可证

内部项目，请勿外传。