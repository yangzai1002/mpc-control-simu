#ifndef XMESSAGESEND_H_
#define XMESSAGESEND_H_
#include "ZmqConstruct.hpp"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "protocol/Auto2Manager.h"
#include "protocol/Loc2Manager.h"
#include "protocol/SensorFault2Manager.h"

//---------------datapool 基础数据定义（适配新架构）-----------------------

class XMessageSend {
public:
    XMessageSend(std::shared_ptr<ZmqConstruct> zmq_construct, std::shared_ptr<zmq::socket_t> zmq_socket);

    ~XMessageSend(){};

public:
    //健康状态发布
    bool sendHealthStatus(const protocol_common::auto2manager_vehicle_status& msg);

    //行动状态发布
    bool sendTaskStatus(const protocol_common::auto2manager_task_status& msg);
    //融合定位
    bool sendTpImu(const protocol_common::loc2manager_basic_data& msg);
    //编队机动局部路径规划发布
    bool sendFormationPath(const protocol_common::auto2manager_formation_path& msg);

    //编队轨迹规划发布
    bool sendFormationTrajectory(const protocol_common::manager2auto_formation_trajectory& msg);

    //地形感知信息发布
    bool sendTerrainInfo(const protocol_common::auto2manager_terrain_info& msg);

    //感知目标信息发布
    bool sendPerceptionTarget(const protocol_common::auto2manager_perception_target_publish& msg);

    //视频状态上报
    bool sendVideoStatus(const protocol_common::auto2manager_video_status& msg);

    //触发控制量发布
    bool sendControlData(const protocol_common::auto2manager_control_data_upload& msg);

    //故障数据上传
    bool sendFaultData(const protocol_common::auto2manager_faultdata& msg);

    //编队定位数据发布
    bool sendFormationLocalization(const protocol_common::manager2auto_formation_position& msg);

    // 偏移量发布
    bool sendOffSetRelease(const protocol_common::auto2manager_offset_publish& msg);

    // 传感器故障上报
    bool sendSensorFault(const protocol_common::auto2infosec_fault_upload& msg);

    // 定位状态上报
    bool sendLocStatus(const protocol_common::loc2manager_status& msg);

private:
    std::shared_ptr<ZmqConstruct>  zmq_publisher;
    std::shared_ptr<zmq::socket_t> publisher;
};
#endif