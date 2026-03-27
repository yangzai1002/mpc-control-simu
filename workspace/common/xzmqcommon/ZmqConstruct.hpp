#pragma once
#include <fmt/format.h>

#include <csignal>
#include <functional>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>
#include <vector>

#include "./protocol/Protocol.h"
#include "HexUtils.hpp"
#include "StringUtils.hpp"
#include "log/log.h"
#include "zmq.hpp"
#define ZMQ_CPP11

class ExceptionZmqRecv : public std::runtime_error {
public:
    ExceptionZmqRecv(const std::string& message) : runtime_error(message){};
};

class ExceptionZmqDeserialize : public std::runtime_error {
public:
    ExceptionZmqDeserialize(const std::string& message) : runtime_error(message){};
};

class ExceptionZmqSend : public std::runtime_error {
public:
    ExceptionZmqSend(const std::string& message) : runtime_error(message){};
};

class ZmqConstruct {
private:
    zmq::context_t context_;
    // std::map<std::string, std::string> topic_endpoint_map_;
public:
    /**
     * @brief 构造函数
     *
     * @param threading_pool_num 消息池大小，一般以 1GB / 1 的形式设置
     */
    ZmqConstruct(int threading_pool_num = 1) : context_(threading_pool_num){};

    /**
     * @brief 析构函数，将 context 销毁，包括socket的释放
     *
     */
    ~ZmqConstruct() {
        context_.close();
    };

    /**
     * @brief 创建一个发布者
     *
     * @param endpoint eg. "tcp://localhost:5050"
     * @param bind 是否绑定端口
     * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
     */
    std::shared_ptr<zmq::socket_t> createPublisher(const std::string& endpoint, bool bind = true) {
        auto publisher = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::pub);

        if (bind) { // 绑定端口
            publisher->bind(endpoint);
        } else { // 连接端口
            // 通过hash生成唯一IPC端口
            // 这样可以在本地使用IPC通信
            std::hash<std::string> hasher;
            size_t                 endpoint_hash = hasher(endpoint);
            auto                   endpoint_hash_str = std::to_string(endpoint_hash);
            std::string            ipc_endpoint = "ipc://" + endpoint_hash_str;
            publisher->connect(ipc_endpoint);
        }
        return publisher;
    }

    /**
     * @brief 创建一个发布者
     *
     * @param endpoint eg. "tcp://localhost:5050"
     * @param bind 是否绑定端口
     * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
     */
    std::shared_ptr<zmq::socket_t> createPusher(const std::string& endpoint, bool bind = true) {
        // 实例化接收者套接字
        auto pusher = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::push);

        // 设置是否绑定
        if (bind) {
            pusher->bind(endpoint);
        } else {
            pusher->connect(endpoint);
        }
        // 设置过滤器

        return pusher;
    }

    /**
     * @brief 创建一个ROUTER服务端
     *
     * @param endpoint 监听端 eg. "tcp://*:6000"
     * @return std::shared_ptr<zmq::socket_t> socket 指针
     */
    std::shared_ptr<zmq::socket_t> createRouter(const std::string& endpoint) {
        auto socket = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::router);
        socket->bind(endpoint);
        return socket;
    }

    /**
     * @brief 创建一个接收者
     *
     * @param endpoint eg. "tcp://localhost:5050"
     * @param bind 是否绑定端口
     * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
     */
    std::shared_ptr<zmq::socket_t> createSubscriber(const std::string&              endpoint,
                                                    const std::vector<std::string>& filters, bool bind = false) {
        // 实例化接收者套接字
        auto subscriber = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::sub);

        // 设置是否绑定
        if (bind) {
            subscriber->bind(endpoint);
        } else {
            subscriber->connect(endpoint);
        }
        // 设置过滤器
        for (const auto& filter : filters) {
            // subscriber->setsockopt(ZMQ_SUBSCRIBE, filter);
            subscriber->setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
        }

        return subscriber;
    }

    /**
     * @brief 另一种创建方式，直接用声明创建，需要指定direct_construct
     *
     * @param endpoint
     * @param filters
     * @param bind
     * @param direct_construct
     * @return zmq::socket_t
     */
    zmq::socket_t createSubscriber(const std::string& endpoint, const std::vector<std::string>& filters, bool bind,
                                   bool direct_construct) {
        zmq::socket_t subscriber(context_, zmq::socket_type::sub);

        // 设置是否绑定
        if (bind) {
            subscriber.bind(endpoint);
        } else {
            subscriber.connect(endpoint);
        }
        // 设置过滤器
        for (const auto& filter : filters) {
            subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
        }
        return subscriber;
    }

    /**
     * @brief 创建一个服务实例
     *
     * @param endpoint eg. "tcp://localhost:5050"
     * @param bind 是否绑定端口
     * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
     */
    std::shared_ptr<zmq::socket_t> createService(const std::string& endpoint, bool bind) {
        auto service = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::rep);

        if (bind) {
            service->bind(endpoint);
        } else {
            service->connect(endpoint);
        }
        return service;
    }

    /**
     * @brief 創建一個客戶端实例
     *
     * @param endpoint eg. "tcp://localhost:5050"
     * @param bind 是否绑定端口
     * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
     */
    std::shared_ptr<zmq::socket_t> createClient(const std::string& endpoint, bool bind) {
        auto client = std::make_shared<zmq::socket_t>(context_, ZMQ_REQ);

        if (bind) {
            client->bind(endpoint);
        } else {
            client->connect(endpoint);
        }
        return client;
    }

    /**
     * @brief 将zmq信息解析为字符串
     *
     * @param msg
     * @return std::string 解析内容
     */
    std::string zmqMsgToStr(const zmq::message_t& msg) {
        auto   data = static_cast<const char*>(msg.data());
        size_t size = msg.size();

        if (size == 0) {
            AERROR << "错误: 消息为空";
            return "";
        } else {
            std::string result(data, size);
            return result;
        }
    }

    /**
     * @brief 发送字符串信息
     *
     * @param content 字符串消息内容
     * @param socket 需要调用的套接字
     * @param wait_response 一般为客户端需要，等待response
     * @return true
     * @return false
     */
    bool zmqSendMsg(const std::string& content, std::shared_ptr<zmq::socket_t>& socket, bool wait_response = true) {
        if (!wait_response) {
            auto sent_result = socket->send(zmq::buffer(content), zmq::send_flags::dontwait);
            if (!sent_result) {
                AERROR << "发送失败!! 错误码: " << zmq_errno();
                return false;
            } else {
                return true;
            }
        } else {
            auto sent_result = socket->send(zmq::buffer(content), zmq::send_flags::none);
            if (!sent_result) {
                AERROR << "发送失败!! 错误码: " << zmq_errno();
                return false;
            } else {
                return true;
            }
        }
    }

    /**
     * @brief 发送json 信息，在内部转为字符串发送
     *
     * @param content
     * @param socket
     * @param wait_response 是否等待回执
     * @return true
     * @return false
     */
    bool zmqSendMsg(const nlohmann::json& content, std::shared_ptr<zmq::socket_t>& socket, bool wait_response) {
        std::string json_content = content.dump();
        return zmqSendMsg(json_content, socket, wait_response);
    };

    bool zmqSendMsg(const int& content, std::shared_ptr<zmq::socket_t>& socket, bool wait_response = false) {
        auto content_str = std::to_string(content);
        return zmqSendMsg(content_str, socket, wait_response);
    };

    bool zmqSendMsg(const long& content, std::shared_ptr<zmq::socket_t>& socket, bool wait_response = false) {
        auto content_str = std::to_string(content);
        return zmqSendMsg(content_str, socket, wait_response);
    };

    bool zmqSendMsg(const char* content, std::shared_ptr<zmq::socket_t>& socket, bool wait_response) {
        if (!content) {
            AERROR << "无效字符串, 长度为0!";
            return false;
        }
        size_t         len_content = strlen(content);
        zmq::message_t zmq_msg(len_content);
        memcpy(zmq_msg.data(), content, len_content);
        if (wait_response) {
            auto result = socket->send(zmq_msg, zmq::send_flags::none);
        } else {
            auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
        }
        return true;
    };

    template <typename T>
    bool zmqSendStruct(const T& content_inst, const std::shared_ptr<zmq::socket_t>& socket, bool wait_response) {
        try {
            // 对于POD类型，直接序列化
            if (std::is_pod<T>::value) {
                zmq::message_t msg(sizeof(T));
                memcpy(msg.data(), &content_inst, sizeof(T));

                if (wait_response) {
                    auto res = socket->send(msg, zmq::send_flags::none);
                    if (!res) {
                        AERROR << "发送失败: " << zmq_strerror(errno);
                        return false;
                    } else {
                        return true;
                    }
                } else {
                    auto res = socket->send(msg, zmq::send_flags::dontwait);
                    if (!res) {
                        AERROR << "发送失败: " << zmq_strerror(errno);
                        return false;
                    } else {
                        return true;
                    }
                }
            }
            // 对于非POD类型，使用自定义序列化
            else {
                std::vector<uint8_t> buffer; // REVIEW 在这里使用新的变长buffer，需要写新的msg_pack,
                                             // parse不用管
                protocol_common::msg_pack(content_inst, buffer);

                // 使用智能指针管理内存，实现零拷贝
                auto           data_ptr = std::make_shared<std::vector<uint8_t>>(std::move(buffer));
                zmq::message_t msg(
                    data_ptr->data(), data_ptr->size(),
                    [](void* /*data*/, void* hint) {
                        delete static_cast<std::shared_ptr<std::vector<uint8_t>>*>(hint);
                    },
                    new std::shared_ptr<std::vector<uint8_t>>(data_ptr));

                if (wait_response) {
                    auto res = socket->send(msg, zmq::send_flags::none);
                    if (!res) {
                        AERROR << "发送失败: " << zmq_strerror(errno);
                        return false;
                    }
                    // 接收响应
                    zmq::message_t response;
                    auto           recv_res = socket->recv(response, zmq::recv_flags::none);
                    return recv_res.has_value();
                } else {
                    auto res = socket->send(msg, zmq::send_flags::dontwait);
                    return true;
                }
            }
        } catch (const std::exception& e) {
            AERROR << "ZMQ发送异常: " << e.what();
            return false;
        } catch (...) {
            AERROR << "ZMQ发送异常";
            return false;
        }
    }

    template <typename T>
    bool zmqSendStruct(std::unique_ptr<T>& content_ptr, std::shared_ptr<zmq::socket_t>& socket, bool wait_response) {
        if (!content_ptr) {
            AERROR << "空指针无法序列化发送!!";
            return false;
        }
        return zmqSendStruct(*content_ptr, socket, wait_response);
    }

    template <typename T> bool zmqSendStruct(const std::shared_ptr<T>&       content_ptr,
                                             std::shared_ptr<zmq::socket_t>& socket, bool wait_response) {
        if (!content_ptr) {
            AERROR << "空指针无法序列化发送!!";
            return false;
        }
        return zmqSendStruct(*content_ptr, socket, wait_response);
    }

    std::vector<uint8_t> serialize(const protocol_common::manager2chassis_control& msg) {
        std::vector<uint8_t> bytes;
        bytes.reserve(sizeof(msg)); // 预分配内存提高效率

        // 1. 序列化帧头（CommunicationHead）
        // 假设CommunicationHead也需要大端转换，需要知道具体结构
        uint32_t       message_id_be = htobe32(msg.header.message_id);
        const uint8_t* message_id_ptr = reinterpret_cast<const uint8_t*>(&message_id_be);
        bytes.insert(bytes.end(), message_id_ptr, message_id_ptr + sizeof(message_id_be));

        uint32_t       message_length_be = htobe32(msg.header.message_length);
        const uint8_t* message_length_ptr = reinterpret_cast<const uint8_t*>(&message_length_be);
        bytes.insert(bytes.end(), message_length_ptr, message_length_ptr + sizeof(message_length_be));

        uint16_t       client_id_be = htobe16(msg.header.client_id);
        const uint8_t* client_id_ptr = reinterpret_cast<const uint8_t*>(&client_id_be);
        bytes.insert(bytes.end(), client_id_ptr, client_id_ptr + sizeof(client_id_be));

        uint16_t       session_id_be = htobe16(msg.header.session_id);
        const uint8_t* session_id_ptr = reinterpret_cast<const uint8_t*>(&session_id_be);
        bytes.insert(bytes.end(), session_id_ptr, session_id_ptr + sizeof(session_id_be));

        bytes.push_back(msg.header.protocol_version);
        bytes.push_back(msg.header.interface_version);
        bytes.push_back(msg.header.message_type);
        bytes.push_back(msg.header.return_code);

        // 2. 序列化主体字段（按声明顺序）
        // 单字节字段直接拷贝
        bytes.push_back(msg.heart);
        bytes.push_back(msg.control_mode);
        bytes.push_back(msg.gear);

        // 处理16位字段（steering）并转为大端
        uint16_t       steering_be = htobe16(msg.steering);
        const uint8_t* steering_ptr = reinterpret_cast<const uint8_t*>(&steering_be);
        bytes.insert(bytes.end(), steering_ptr, steering_ptr + sizeof(steering_be));

        // 处理16位字段（throttle_braking）并转为大端
        uint16_t       throttle_be = htobe16(msg.throttle_braking);
        const uint8_t* throttle_ptr = reinterpret_cast<const uint8_t*>(&throttle_be);
        bytes.insert(bytes.end(), throttle_ptr, throttle_ptr + sizeof(throttle_be));

        // 单字节字段
        bytes.push_back(msg.steering_mode_req);
        bytes.push_back(msg.centrsteer_req);

        // 处理16位字段（steering_req）并转为大端
        // int16_t steering_req_be = htobe16(static_cast<int16_t>(msg.steering_req));
        const int8_t* steering_req_ptr = reinterpret_cast<const int8_t*>(&msg.steering_req);
        bytes.insert(bytes.end(), steering_req_ptr, steering_req_ptr + sizeof(msg.steering_req));

        // 处理16位字段（speed_req）并转为大端
        // int16_t speed_req_be = htobe16(static_cast<int16_t>(msg.speed_req));
        const int8_t* speed_req_ptr = reinterpret_cast<const int8_t*>(&msg.speed_req);
        bytes.insert(bytes.end(), speed_req_ptr, speed_req_ptr + sizeof(msg.speed_req));

        // 单字节字段
        bytes.push_back(msg.emergency_breaking);
        bytes.push_back(msg.chassis_enable);
        bytes.push_back(msg.control_mode_req);
        bytes.push_back(msg.engine_control);
        bytes.push_back(msg.engine_power_mode);
        bytes.push_back(msg.speed_limit);
        bytes.push_back(msg.high_voltage_power);
        bytes.push_back(msg.motor_mode);
        bytes.push_back(msg.run_mode);
        bytes.push_back(msg.parking_mode);
        bytes.push_back(msg.parking_brake);
        bytes.push_back(msg.water_pump_control1);
        bytes.push_back(msg.water_pump_control2);
        bytes.push_back(msg.fan_control);
        bytes.push_back(msg.low_beams);
        bytes.push_back(msg.high_beams);
        bytes.push_back(msg.leftturn_signals);
        bytes.push_back(msg.rightturn_signals);
        bytes.push_back(msg.hazard_lights);
        bytes.push_back(msg.double_flash);
        bytes.push_back(msg.horn);
        bytes.push_back(msg.drainage_pump);
        bytes.push_back(msg.radar_power);
        bytes.push_back(msg.lidar_power);
        bytes.push_back(msg.night_vision_power);
        bytes.push_back(msg.load_power);
        bytes.push_back(msg.debug_equipment);
        bytes.push_back(msg.protection);
        bytes.push_back(msg.error_reset);
        bytes.push_back(msg.powerbattery_heating);
        bytes.push_back(msg.mains_output);
        bytes.push_back(msg.battery_output);

        // 3. 序列化帧尾（CommunicationTail）
        // 假设CommunicationTail也需要大端转换，需要知道具体结构
        uint32_t       msg_tail_be = htobe32(msg.tail.payload_checksum);
        const uint8_t* msg_tail_ptr = reinterpret_cast<const uint8_t*>(&msg_tail_be);
        bytes.insert(bytes.end(), msg_tail_ptr, msg_tail_ptr + sizeof(msg_tail_be));
        return bytes;
    }

    bool zmqSendChassis(const protocol_common::manager2chassis_control& _msg, std::shared_ptr<zmq::socket_t>& socket,
                        bool wait_response) {
        // 1. 创建与结构体大小相同的字节数组
        // std::vector<uint8_t> bytes(sizeof(_msg));
        // uint8_t bytes[8];
        // toNetworkByteOrder(_msg,bytes);
        // 2. 直接内存拷贝（高效且避免逐字段操作）
        // memcpy(bytes.data(), &_msg, sizeof(_msg));
        // std::cout  <<"发送成功11 开始:";
        // for (auto b : bytes) {
        //   printf("%02X ", b);  // 应与 raw_ptr 完全一致
        // }
        // std::cout << std::endl;
        // 发送时使用序列化后的数据
        auto serialized = serialize(_msg);
        // std::vector<uint8_t> bytes(sizeof(_msg));
        // memcpy(bytes.data(), &_msg, sizeof(_msg));
        zmq::message_t zmq_msg(serialized.data(), sizeof(_msg));
        const uint8_t* zmq_ptr = static_cast<const uint8_t*>(zmq_msg.data());
        std::cout << "发送成功 开始:";
        for (size_t i = 0; i < zmq_msg.size(); ++i) {
            printf("%02X ", zmq_ptr[i]); // 应与 bytes 一致
        }
        std::cout << std::endl;
        auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
        if (!result) {
            std::cerr << "发送失败 failed！!";
            return false;
        } else {
            std::cout << "发送成功 success:" << std::endl;
        }
        return true;
    }

    template <typename RT> RT zmqRecvStruct(std::shared_ptr<zmq::socket_t>& socket, const std::string& topic,
                                            int timeout = 10000000) noexcept(false) {
        // 创建消息对象并接收数据
        zmq::message_t message;
        // 设置接收超时
        socket->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

        std::int32_t i = 0;
        while (true) {
            auto result = socket->recv(message, zmq::recv_flags::none);
            if (result) {
                break;
            } else if (EAGAIN == result) {
                if (i < 10) {
                    AINFO << "zmq recv would retry";
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                } else {
                    throw ExceptionZmqRecv("zmq recv error: " + app_common::StringUtils::str_error(errno));
                }
            } else {
                throw ExceptionZmqRecv("zmq recv error: " + app_common::StringUtils::str_error(errno));
            }
        }

        AINFO << "zmq recv data, topic " << topic << ", data "
              << app_common::HexUtils::to_hex_string((std::uint8_t*)message.data(), message.size());

        if (std::is_pod<RT>::value) {
            if (message.size() != sizeof(RT)) {
                throw ExceptionZmqDeserialize(
                    fmt::format("zmq data mismatch, expected {}, received {}", sizeof(RT), message.size()));
            }

            RT d{};
            memcpy(&d, message.data(), sizeof(d));
            return d;
        } else {
            auto   buffer = static_cast<const uint8_t*>(message.data());
            size_t offset = 0;
            try {
                auto d = protocol_common::msg_parse<RT>(buffer, message.size(), offset);
                if (offset != message.size()) {
                    throw ExceptionZmqDeserialize(
                        fmt::format("zmq data mismatch, parsed {}, received {}", offset, message.size()));
                }
                return d;
            } catch (const std::runtime_error& e) {
                throw ExceptionZmqDeserialize(e.what());
            } catch (...) {
                std::rethrow_exception(std::current_exception());
            }
        }
    }

    zmq::message_t zmqRecvMsg(std::shared_ptr<zmq::socket_t>& socket, int timeout = 10000000) noexcept(false) {
        // 创建消息对象并接收数据
        zmq::message_t message;
        // 设置接收超时
        socket->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

        std::int32_t i = 0;
        while (true) {
            auto result = socket->recv(message, zmq::recv_flags::none);
            if (result) {
                break;
            } else if (EAGAIN == result) {
                if (i < 10) {
                    AWARN << "zmq recv would retry";
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                } else {
                    throw std::runtime_error("zmq recv error: " + app_common::StringUtils::str_error(errno));
                }
                i++;
            } else {
                throw std::runtime_error("zmq recv error: " + app_common::StringUtils::str_error(errno));
            }
        }

        return message;
    }

    /**
     * @brief 创建一个DEALER客户端
     *
     * @param endpoint 连接端 eg. "tcp://localhost:6000"
     * @param identity 可选，设置DEALER身份
     * @return std::shared_ptr<zmq::socket_t> socket 指针
     */
    std::shared_ptr<zmq::socket_t> createDealer(const std::string& endpoint, const std::string& identity = "") {
        auto socket = std::make_shared<zmq::socket_t>(context_, zmq::socket_type::dealer);
        if (!identity.empty()) {
            socket->setsockopt(ZMQ_ROUTING_ID, identity.c_str(), identity.size());
        } else {
            // 自动生成一个唯一的identity（如使用UUID或时间戳+随机数）
            std::string auto_id = "auto-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
            socket->setsockopt(ZMQ_ROUTING_ID, auto_id.c_str(), auto_id.size());
        }
        socket->connect(endpoint);
        return socket;
    }

    /**
     * @brief ROUTER接收多帧buffer（[identity][empty][topic][data]）
     *
     * @param socket ROUTER套接字
     * @param identity 返回identity字符串
     * @param topic 返回topic字符串
     * @param data 返回数据buffer指针
     * @param data_size 返回数据长度
     * @param timeout_ms 超时时间ms，<=0阻塞
     * @return true 接收成功
     * @return false 超时或失败
     */
    int recvRouterTopicBase(std::shared_ptr<zmq::socket_t>& socket, std::string& identity, std::string& topic,
                            int timeout_ms = 1000) {
        if (timeout_ms > 0) {
            socket->setsockopt(ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
        }
        zmq::message_t id_msg, empty, topic_msg;
        if (!socket->recv(id_msg, zmq::recv_flags::none)) {
            return 1;
        }
        if (!socket->recv(empty, zmq::recv_flags::none)) {
            return 1;
        }
        if (!socket->recv(topic_msg, zmq::recv_flags::none)) {
            return 1;
        }
        identity.assign(static_cast<const char*>(id_msg.data()), id_msg.size());
        topic.assign(static_cast<const char*>(topic_msg.data()), topic_msg.size());
        return 0;
    }

    /**
     * @brief
     * ROUTER通过identity发送多帧buffer（[identity][empty][topic][data]）
     *
     * @param socket ROUTER套接字
     * @param identity 目标DEALER的identity字符串
     * @param topic 发送的topic字符串
     * @param data 需要发送的数据buffer指针
     * @param data_size 发送数据的字节数
     */
    void sendRouterTopicBuffer(std::shared_ptr<zmq::socket_t>& socket, const std::string& identity,
                               const std::string& topic, const void* data, size_t data_size) noexcept(false) {
        zmq::message_t id_msg(identity.c_str(), identity.size());
        zmq::message_t topic_msg(topic.c_str(), topic.size());
        zmq::message_t data_msg(data, data_size);

        socket->send(id_msg, zmq::send_flags::sndmore);
        socket->send(topic_msg, zmq::send_flags::sndmore);
        socket->send(data_msg, zmq::send_flags::none);
    }
};
