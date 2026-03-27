#include "ZmqConstruct.h"

// using namespace collaborationSys;

std::shared_ptr<zmq::socket_t> ZmqConstruct::createPublisher(
    const std::string &endpoint, bool bind) {
  auto publisher =
      std::make_shared<zmq::socket_t>(context_, zmq::socket_type::pub);

  if (bind) {  // 绑定端口
    publisher->bind(endpoint);
  } else {
    publisher->connect(endpoint);
  }
  return publisher;
}


std::shared_ptr<zmq::socket_t> ZmqConstruct::createRouter(const std::string &endpoint,
                                                 bool bind){
      auto publisher =
            std::make_shared<zmq::socket_t>(context_, zmq::socket_type::router);

        if (bind) {  // 绑定端口
          publisher->bind(endpoint);
        } else {
          publisher->connect(endpoint);
        }
        return publisher;
}

std::shared_ptr<zmq::socket_t> ZmqConstruct::createSubscriber(
    const std::string &endpoint, const std::vector<std::string> &filters,
    bool bind) {
  // 实例化接收者套接字
  auto subscriber =
      std::make_shared<zmq::socket_t>(context_, zmq::socket_type::pull);

  // 设置是否绑定
  if (bind) {
    subscriber->bind(endpoint);
  } else {
    subscriber->connect(endpoint);
  }
  // 设置过滤器
  for (const auto &filter : filters) {
    // subscriber->setsockopt(ZMQ_SUBSCRIBE, filter);
    subscriber->setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
  }

  return subscriber;
}

zmq::socket_t ZmqConstruct::createSubscriber(
    const std::string &endpoint, const std::vector<std::string> &filters,
    bool bind, bool direct_construct) {
  zmq::socket_t subscriber(context_, zmq::socket_type::sub);

  // 设置是否绑定
  if (bind) {
    subscriber.bind(endpoint);
  } else {
    subscriber.connect(endpoint);
  }
  // 设置过滤器
  for (const auto &filter : filters) {
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
  }
  return subscriber;
}

std::shared_ptr<zmq::socket_t> ZmqConstruct::createService(
    const std::string &endpoint, bool bind) {
  auto service =
      std::make_shared<zmq::socket_t>(context_, zmq::socket_type::rep);

  if (bind) {
    service->bind(endpoint);
  } else {
    service->connect(endpoint);
  }
  return service;
}

std::shared_ptr<zmq::socket_t> ZmqConstruct::createClient(
    const std::string &endpoint, bool bind) {
  auto client = std::make_shared<zmq::socket_t>(context_, ZMQ_REQ);

  if (bind) {
    client->bind(endpoint);
  } else {
    client->connect(endpoint);
  }
  return client;
}

bool ZmqConstruct::zmqMsgToJson(const zmq::message_t &msg,
                                nlohmann::json &result) {
  if (msg.size() == 0) {
    std::cerr << "错误: 消息为空" << std::endl;
    return false;
  }

  try {
    const char *data = static_cast<const char *>(msg.data());
    result = nlohmann::json::parse(data, data + msg.size());
    return true;
  } catch (const nlohmann::json::parse_error &e) {
    std::cerr << "JSON 解析错误: " << e.what() << std::endl;
    std::cerr << "错误位置: " << e.byte << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "转换错误: " << e.what() << std::endl;
    return false;
  }
}

std::string ZmqConstruct::zmqMsgToStr(const zmq::message_t &msg) {
  auto data = static_cast<const char *>(msg.data());
  size_t size = msg.size();

  if (size == 0) {
    std::cerr << "错误: 消息为空" << std::endl;
    return "";
  } else {
    std::string result(data, size);
    return result;
  }
}

bool ZmqConstruct::zmqSendMsg(const std::string &content,
                              std::shared_ptr<zmq::socket_t> &socket,
                              bool wait_response) {
  if (!wait_response) {
        std::string topic = "topic_message";
        zmq::message_t topic_msg(topic.c_str(), topic.size());
        socket->send(topic_msg);
    auto sent_result =
        socket->send(zmq::buffer(content), zmq::send_flags::dontwait);
    if (!sent_result) {
      std::cerr << "發送失敗！！" << zmq_errno() << std::endl;
      return false;
    } else {
      return true;
    }
  } else {
    auto sent_result =
        socket->send(zmq::buffer(content), zmq::send_flags::none);
    if (!sent_result) {
      std::cerr << "發送失敗！！" << zmq_errno() << std::endl;
      return false;
    } else {
      return true;
    }
  }
}

bool ZmqConstruct::zmqRecvMsg2Json(nlohmann::json &target,
                                   std::shared_ptr<zmq::socket_t> &socket,
                                   bool wait) {
  zmq::message_t msg;

  if (!wait) {
    socket->recv(msg, zmq::recv_flags::dontwait);
  } else {
    socket->recv(msg, zmq::recv_flags::none);
  }

  zmqMsgToJson(msg, target);
}