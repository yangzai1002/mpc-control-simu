#include "XMessageSend.h"
#include "ap_log/ap_log_interface.h"

XMessageSend::XMessageSend(std::shared_ptr<ZmqConstruct> zmq_construct, std::shared_ptr<zmq::socket_t> zmq_socket)
    : zmq_publisher(zmq_construct), publisher(zmq_socket) {
}

bool XMessageSend::sendHealthStatus(const protocol_common::auto2manager_vehicle_status& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_vehicle_status";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send health status failed";
    }
    return ret;
}

bool XMessageSend::sendFormationTrajectory(const protocol_common::manager2auto_formation_trajectory& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "manager2auto_formation_trajectory";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send task status failed";
    }
    return ret;
}

bool XMessageSend::sendTaskStatus(const protocol_common::auto2manager_task_status& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_task_status";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send task status failed";
    }
    return ret;
}
bool XMessageSend::sendTpImu(const protocol_common::loc2manager_basic_data& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "loc2manager_basic_data";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "sendTpImu failed";
    }
    return ret;
}

bool XMessageSend::sendFormationPath(const protocol_common::auto2manager_formation_path& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_formation_path";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send formation path failed";
    }
    return ret;
}

bool XMessageSend::sendFormationLocalization(const protocol_common::manager2auto_formation_position& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "manager2auto_formation_position";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send formation localization failed";
    }
    return ret;
}

bool XMessageSend::sendTerrainInfo(const protocol_common::auto2manager_terrain_info& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_terrain_info";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send terrain info failed";
    }
    return ret;
}

bool XMessageSend::sendPerceptionTarget(const protocol_common::auto2manager_perception_target_publish& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_perception_target_publish";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send perception target failed";
    }
    return ret;
}

bool XMessageSend::sendVideoStatus(const protocol_common::auto2manager_video_status& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_video_status";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send video status failed";
    }
    return ret;
}

bool XMessageSend::sendControlData(const protocol_common::auto2manager_control_data_upload& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_control_data";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send control data failed";
    }
    return ret;
}

bool XMessageSend::sendFaultData(const protocol_common::auto2manager_faultdata& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_faultdata";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send fault data failed";
    }
    return ret;
}

bool XMessageSend::sendOffSetRelease(const protocol_common::auto2manager_offset_publish& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2manager_offset_release";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send offset_release data failed";
    }
    return ret;
}

bool XMessageSend::sendSensorFault(const protocol_common::auto2infosec_fault_upload& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "auto2infosec_fault_upload";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send sensor fault data failed";
    }
    return ret;
}

bool XMessageSend::sendLocStatus(const protocol_common::loc2manager_status& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApError("zmq_notification") << "zmq_publisher or publisher is null";
        return false;
    }

    std::string    topic = "loc2manager_status";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    publisher->send(topic_msg, ZMQ_SNDMORE);

    bool ret = zmq_publisher->zmqSendStruct(msg, publisher, false);
    if (!ret) {
        ApError("zmq_notification") << "send loc status failed";
    }
    return ret;
}
