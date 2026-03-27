#ifndef XMESSAGESEND_H_
#define XMESSAGESEND_H_
#include "ZmqConstruct.h"
#include "Manager2Chassis.h"


class XMessageSend {
public:
    XMessageSend(std::shared_ptr<ZmqConstruct> zmq_construct,
                  std::shared_ptr<zmq::socket_t> zmq_socket);

    ~XMessageSend(){};

public:
    //自主数据发送
    bool sendVehicleData(const std::string topic,const protocol_common::manager2chassis_control& msg) ;

private:
    std::shared_ptr<ZmqConstruct> zmq_publisher;
    std::shared_ptr<zmq::socket_t> publisher;
};

#endif
