#pragma once
#include <proto/Protocol.h>

#include <csignal>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#define ZMQ_CPP11
#include <zmq.hpp>
#include "common.h"
#include <cstring> 

// #include "Log.h"

// namespace collaborationSys {

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
  ZmqConstruct(int threading_pool_num = 1) : context_(threading_pool_num) {};

  /**
   * @brief 析构函数，将 context 销毁，包括socket的释放
   *
   */
  ~ZmqConstruct() { context_.close(); };

  /**
   * @brief 创建一个发布者
   *
   * @param endpoint eg. "tcp://localhost:5050"
   * @param bind 是否绑定端口
   * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
   */
  std::shared_ptr<zmq::socket_t> createPublisher(const std::string &endpoint,
                                                 bool bind = true);

  /**
   * @brief 创建一个接收者
   *
   * @param endpoint eg. "tcp://localhost:5050"
   * @param bind 是否绑定端口
   * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
   */
  std::shared_ptr<zmq::socket_t> createSubscriber(
      const std::string &endpoint, const std::vector<std::string> &filters,
      bool bind = false);

  /**
   * @brief 另一种创建方式，直接用声明创建，需要指定direct_construct
   *
   * @param endpoint
   * @param filters
   * @param bind
   * @param direct_construct
   * @return zmq::socket_t
   */
  zmq::socket_t createSubscriber(const std::string &endpoint,
                                 const std::vector<std::string> &filters,
                                 bool bind, bool direct_construct);


std::shared_ptr<zmq::socket_t> createRouter(const std::string &endpoint,
              bool bind = true);

  /**
   * @brief 创建一个服务实例
   *
   * @param endpoint eg. "tcp://localhost:5050"
   * @param bind 是否绑定端口
   * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
   */
  std::shared_ptr<zmq::socket_t> createService(const std::string &endpoint,
                                               bool bind);

  /**
   * @brief 創建一個客戶端实例
   *
   * @param endpoint eg. "tcp://localhost:5050"
   * @param bind 是否绑定端口
   * @return std::shared_ptr<zmq::socket_t> socket 指针，用->访问属性
   */
  std::shared_ptr<zmq::socket_t> createClient(const std::string &endpoint,
                                              bool bind);

  /**
   * @brief 将zmq::message_t 对象转换为json
   *
   * @param msg zmq message
   * @param result json对象
   * @return true 转换成功
   * @return false 转换失败
   */
  bool zmqMsgToJson(const zmq::message_t &msg, nlohmann::json &result);

  /**
   * @brief 将zmq信息解析为字符串
   *
   * @param msg
   * @return std::string 解析内容
   */
  std::string zmqMsgToStr(const zmq::message_t &msg);

  /**
   * @brief 发送字符串信息
   *
   * @param content 字符串消息内容
   * @param socket 需要调用的套接字
   * @param wait_response 一般为客户端需要，等待response
   * @return true
   * @return false
   */
  bool zmqSendMsg(const std::string &content,
                  std::shared_ptr<zmq::socket_t> &socket,
                  bool wait_response = true);

  /**
   * @brief 发送json 信息，在内部转为字符串发送
   *
   * @param content
   * @param socket
   * @param wait_response 是否等待回执
   * @return true
   * @return false
   */
  bool zmqSendMsg(const nlohmann::json &content,
                  std::shared_ptr<zmq::socket_t> &socket, bool wait_response) {
    std::string json_content = content.dump();
    return zmqSendMsg(json_content, socket, wait_response);
  };

  bool zmqSendMsg(const int &content, std::shared_ptr<zmq::socket_t> &socket,
                  bool wait_response = false) {
    auto content_str = std::to_string(content);
    return zmqSendMsg(content_str, socket, wait_response);
  };

  bool zmqSendMsg(const long &content, std::shared_ptr<zmq::socket_t> &socket,
                  bool wait_response = false) {
    auto content_str = std::to_string(content);
    return zmqSendMsg(content_str, socket, wait_response);
  };

  bool zmqSendMsg(const char *content, std::shared_ptr<zmq::socket_t> &socket,
                  bool wait_response) {
    if (!content) {
      std::cerr << "无效字符串, 长度为0!";
      return false;
    }
    size_t len_content = strlen(content);
    zmq::message_t zmq_msg(len_content);
    memcpy(zmq_msg.data(), content, len_content);
    if (wait_response) {
      auto result = socket->send(zmq_msg, zmq::send_flags::none);
    } else {
      auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
    }
    return true;
  };

  bool zmqSendChassis(const protocol_common::manager2chassis_control& _msg, std::shared_ptr<zmq::socket_t> &socket,
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
    std::cout  <<"发送成功 开始:";
    for (size_t i = 0; i < zmq_msg.size(); ++i) {
        printf("%02X ", zmq_ptr[i]);  // 应与 bytes 一致
    }
    std::cout << std::endl;
    auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
    if (!result) {
      std::cerr << "发送失败 failed！!";
      return false;
    }else{
      std::cout  <<"发送成功 success:" << std::endl;
    }
    return true;
  }
  std::vector<uint8_t> serialize(const protocol_common::manager2chassis_control& msg) {
    std::vector<uint8_t> bytes;
    bytes.reserve(sizeof(msg)); // 预分配内存提高效率

    // 1. 序列化帧头（CommunicationHead）
    // 假设CommunicationHead也需要大端转换，需要知道具体结构
    uint32_t message_id_be = htobe32(msg.header.message_id);
    const uint8_t* message_id_ptr = reinterpret_cast<const uint8_t*>(&message_id_be);
    bytes.insert(bytes.end(), message_id_ptr, message_id_ptr + sizeof(message_id_be));

    uint32_t message_length_be = htobe32(msg.header.message_length);
    const uint8_t* message_length_ptr = reinterpret_cast<const uint8_t*>(&message_length_be);
    bytes.insert(bytes.end(), message_length_ptr, message_length_ptr + sizeof(message_length_be));

    uint16_t client_id_be = htobe16(msg.header.client_id);
    const uint8_t* client_id_ptr = reinterpret_cast<const uint8_t*>(&client_id_be);
    bytes.insert(bytes.end(), client_id_ptr, client_id_ptr + sizeof(client_id_be));

    uint16_t session_id_be = htobe16(msg.header.session_id);
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
    uint16_t steering_be = htobe16(msg.steering);
    const uint8_t* steering_ptr = reinterpret_cast<const uint8_t*>(&steering_be);
    bytes.insert(bytes.end(), steering_ptr, steering_ptr + sizeof(steering_be));

    // 处理16位字段（throttle_braking）并转为大端
    uint16_t throttle_be = htobe16(msg.throttle_braking);
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
    uint32_t msg_tail_be = htobe32(msg.tail.payload_checksum);
    const uint8_t* msg_tail_ptr = reinterpret_cast<const uint8_t*>(&msg_tail_be);
    bytes.insert(bytes.end(), msg_tail_ptr, msg_tail_ptr + sizeof(msg_tail_be));
    return bytes;
}

template <typename T>
  bool zmqSendStruct(T &content_inst, std::shared_ptr<zmq::socket_t> &socket,
                     bool wait_response) {
    if constexpr (std::is_pod<T>::value) {
      auto size = sizeof(T);
      zmq::message_t zmq_msg(size);
      memcpy(zmq_msg.data(), &content_inst, size);
      if (wait_response) {
        auto result = socket->send(zmq_msg, zmq::send_flags::none);
        if (!result) {
          std::cerr << "发送失败 failed！!";
          return false;
        }
      } else {
        auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
        if (!result) {
          std::cerr << "发送失败 failed！!";
          return false;
        }else{
          std::cout  <<"发送成功 success:" << std::endl;
        }
      }

      return true;
    } else {
      std::cout << "非POD结构, 尝试调用结构体内部序列化函数";
      uint8_t buffer[1024] = {0};
      size_t offset = 0;
      protocol_common::msg_pack(content_inst, buffer, offset);
      zmq::message_t zmq_msg(offset);
      memcpy(zmq_msg.data(), buffer, offset);
      if (wait_response) {
        auto result = socket->send(zmq_msg, zmq::send_flags::none);
        if (!result) {
          std::cerr << "发送失败！!";
          std::cout << "*****************************************8888send fail!!" << std::endl;
          return false;
        }
      } else {
        auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
        if (!result) {
          std::cerr << "发送失败！!";
          std::cout << "*****************************************8888send fail!!" << std::endl;
          return false;
        }else{
          std::ostringstream oss_all;
                oss_all << "[ZMQ] 发送的长度: " << offset;
                oss_all << "[ZMQ] 发送的数据 (HEX): ";
          const uint8_t *data = static_cast<const uint8_t*>(zmq_msg.data());
          for (size_t i = 0; i < offset; ++i) {
              oss_all << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(data[i]) << " ";
          }
          std::cout << oss_all.str() << std::endl;
          std::cout << "*****************************************8888send success!!" << std::endl;
        }
      }
      std::cout << "*************************************************success!!" << std::endl;
      return true;
    }
  }

  template <typename T>
  bool zmqSendStruct(std::unique_ptr<T> &content_ptr,
                     std::shared_ptr<zmq::socket_t> &socket,
                     bool wait_response) {
    if (!content_ptr) {
      std::cerr << "空指针无法序列化发送!!";
      return false;
    }
    return zmqSendStruct(*content_ptr, socket, wait_response);
  }

  template <typename T>
  bool zmqSendStruct(const std::shared_ptr<T> &content_ptr,
                     std::shared_ptr<zmq::socket_t> &socket,
                     bool wait_response) {
    if (!content_ptr) {
      std::cerr << "空指针无法序列化发送!!";
      return false;
    }
    return zmqSendStruct(*content_ptr, socket, wait_response);
  }

  template <typename RT>
  std::shared_ptr<RT> zmqRecvStruct(std::shared_ptr<zmq::socket_t> &socket,
                                    int timeout = 10000000) {
    // 创建消息对象并接收数据
    zmq::message_t message;
    try {
      // 设置接收超时
      socket->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

      auto result = socket->recv(message, zmq::recv_flags::none);

      if (!result) {
        if (errno == EAGAIN) {
          std::cout << "接收超时";
        } else {
          std::cerr << "接收错误: " << zmq_errno();
        }
        return nullptr;
      }
    } catch (const zmq::error_t &e) {
      std::cerr << "ZMQ错误: " << e.what() << e.num();
      return nullptr;
    } catch (const std::exception &e) {
      // LOG_ERROR("未知异常: %s", e.what());
      std::cerr << "未知异常: " << e.what();
      return nullptr;
    }

    if (std::is_pod<RT>::value) {
      std::cout << "类型为 POD, 可直接解码";
      if (message.size() != sizeof(RT)) {
        std::cerr << "消息大小不匹配，期望: " << sizeof(RT)
                     << " , 实际: " << message.size();
        return nullptr;
      }

      // 分配内存并复制接收到的数据
      auto data = std::make_shared<RT>();
      memcpy(data.get(), message.data(), sizeof(RT));

      return data;

    } else {
      std::cout << "为非POD数据, 尝试调用类内解码程序";
      auto buffer = static_cast<const uint8_t *>(message.data());
      size_t offset = 0;
      auto msg_struct_inst = protocol_common::msg_parse<RT>(buffer, offset);
      return std::make_shared<RT>(std::move(msg_struct_inst));
    }
  }

  bool zmqRecvMsg2Json(nlohmann::json &target,
                       std::shared_ptr<zmq::socket_t> &socket, bool wait);
};

//}  // namespace collaborationSys
