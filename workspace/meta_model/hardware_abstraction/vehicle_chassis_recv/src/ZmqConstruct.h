#pragma once
#include <protocol/Protocol.h>

#include <csignal>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#define ZMQ_CPP11
#include <zmq.hpp>
#include "common.h"


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
          std::cerr << "发送失败！!";
          return false;
        }
      } else {
        auto result = socket->send(zmq_msg, zmq::send_flags::dontwait);
        if (!result) {
          std::cerr << "发送失败！!";
          return false;
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
