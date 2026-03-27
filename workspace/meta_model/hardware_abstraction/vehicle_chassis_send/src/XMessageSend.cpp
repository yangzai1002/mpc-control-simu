#include "XMessageSend.h"
#include "ap_log/ap_log_interface.h"
#include "log/log.h"

XMessageSend::XMessageSend(std::shared_ptr<ZmqConstruct> zmq_construct, std::shared_ptr<zmq::socket_t> zmq_socket)
    : zmq_publisher(zmq_construct), publisher(zmq_socket) {
}

// bool XMessageSend::sendVehicleData(const std::string topic,const protocol_common::manager2chassis_control& msg) {
bool XMessageSend::sendVehicleData(const std::string topic, const protocol_common::manager2chassis_control& msg) {
    if (zmq_publisher == nullptr || publisher == nullptr) {
        ApInfo("vehicle_chassis_send") << "zmq_publisher or publisher is null";
        return false;
    }

    // std::string topic = "auto2manager/vehicle_status";
    zmq::message_t topic_msg(topic.c_str(), topic.size());
    bool           ret = publisher->send(topic_msg, ZMQ_SNDMORE);
    if (!ret) {
        ApInfo("vehicle_chassis_send") << "send ZMQ_SNDMORE failed";
    } else {
        ApInfo("vehicle_chassis_send") << "send ZMQ_SNDMORE success";
    }
    ret = zmq_publisher->zmqSendChassis(msg, publisher, false);
    if (!ret) {
        ApInfo("vehicle_chassis_send") << "send Vehicle Data failed";
    } else {
        ApInfo("vehicle_chassis_send") << "send Vehicle Data success:" << msg.heart;
    }
    return ret;
}
