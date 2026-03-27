#pragma once

#include <memory>    // std::shared_ptr, std::make_shared
#include <stdexcept> // std::runtime_error
#include <string>    // std::string
#include <vector>    // std::vector
#include <zmq.hpp>   // ZeroMQ 主头文件

/**
 * @class ZmqHelper
 * @brief ZeroMQ 辅助工具类（修复版）
 *
 * 该类提供 ZeroMQ 套接字创建、消息发送和接收的简化接口，
 * 支持多种套接字类型（发布者、订阅者、请求者、响应者等），
 * 并提供消息序列化和反序列化功能。
 */
class ZmqHelper {
private:
    zmq::context_t context_; ///< ZeroMQ 上下文对象，管理所有套接字资源

    /**
     * @brief 配置套接字端点（绑定或连接）
     *
     * @param endpoint 端点地址
     * @param bind 是否绑定端口
     * @param socket 目标套接字
     *
     * @throw zmq::error_t 如果操作失败
     */
    void configureSocket(const std::string& endpoint, bool bind, zmq::socket_t& socket) {
        if (bind) {
            socket.bind(endpoint); // 绑定到端口
        } else {
            socket.connect(endpoint); // 连接到端点
        }
    }

    /**
     * @brief 处理 ZeroMQ 错误
     *
     * @param e ZeroMQ 异常对象
     *
     * @throw std::runtime_error 转换后的标准异常
     */
    void handleZmqError(const zmq::error_t& e) {
        // 构建详细错误信息
        std::string error_msg = "ZeroMQ 错误: ";
        error_msg += e.what();
        error_msg += " (错误码: " + std::to_string(e.num()) + ")";

        // 抛出标准异常
        throw std::runtime_error(error_msg);
    }

public:
    /**
     * @brief 构造函数
     *
     * @param threading_pool_num IO 线程池大小，默认为 1
     *
     * @note 线程池大小应根据系统负载调整，一般设置为 CPU 核心数
     */
    explicit ZmqHelper(int threading_pool_num = 1) : context_(threading_pool_num) {
        // 构造函数无需额外操作
    }

    /**
     * @brief 析构函数
     *
     * 安全关闭 ZeroMQ 上下文，释放所有关联资源
     *
     * @note 会捕获并忽略所有异常，确保安全析构
     */
    ~ZmqHelper() {
        try {
            context_.close(); // 关闭上下文
        } catch (...) {
            // 安全忽略所有异常，防止析构函数抛出异常
        }
    }

    // ==================== 套接字创建方法 ====================

    /**
     * @brief 创建订阅者套接字
     *
     * @param endpoint 端点地址，格式如 "tcp://localhost:5050"
     * @param filters 订阅过滤器列表，支持多主题订阅
     * @param bind 是否绑定端口（true）或连接（false）
     * @param direct_construct 直接构造标志（保留参数，兼容旧接口）
     * @return zmq::socket_t 订阅者套接字对象
     *
     * @throw std::runtime_error 如果 ZeroMQ 操作失败
     *
     * @note 当 bind=true 时，套接字将绑定到指定端口；
     *       当 bind=false 时，套接字将连接到指定端点。
     */
    zmq::socket_t createSubscriber(const std::string& endpoint, const std::vector<std::string>& filters, bool bind,
                                   bool direct_construct = true) {
        // 创建 SUB 类型套接字
        zmq::socket_t socket(context_, ZMQ_SUB);

        try {
            // 配置套接字端点（绑定或连接）
            configureSocket(endpoint, bind, socket);

            // 设置订阅过滤器
            for (const auto& filter : filters) {
                socket.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
            }
        } catch (const zmq::error_t& e) {
            // 转换 ZeroMQ 异常为标准异常
            handleZmqError(e);
        }

        return socket;
    }

    /**
     * @brief 创建发布者套接字
     *
     * @param endpoint 端点地址，格式如 "tcp://*:5050"
     * @param bind 是否绑定端口（默认 true）
     * @return zmq::socket_t 发布者套接字对象
     *
     * @throw std::runtime_error 如果 ZeroMQ 操作失败
     *
     * @note 发布者通常绑定端口，等待订阅者连接
     */
    zmq::socket_t createPublisher(const std::string& endpoint, bool bind = true) {
        // 创建 PUB 类型套接字
        zmq::socket_t socket(context_, ZMQ_PUB);

        try {
            // 配置套接字端点
            configureSocket(endpoint, bind, socket);
        } catch (const zmq::error_t& e) {
            handleZmqError(e);
        }

        return socket;
    }

    /**
     * @brief 创建请求者套接字（REQ 模式）
     *
     * @param endpoint 端点地址
     * @param bind 是否绑定端口（默认 false）
     * @return zmq::socket_t 请求者套接字对象
     *
     * @throw std::runtime_error 如果 ZeroMQ 操作失败
     *
     * @note 请求者通常连接到响应者（REP）服务端
     */
    zmq::socket_t createRequester(const std::string& endpoint, bool bind = false) {
        // 创建 REQ 类型套接字
        zmq::socket_t socket(context_, ZMQ_REQ);

        try {
            configureSocket(endpoint, bind, socket);
        } catch (const zmq::error_t& e) {
            handleZmqError(e);
        }

        return socket;
    }

    /**
     * @brief 创建响应者套接字（REP 模式）
     *
     * @param endpoint 端点地址
     * @param bind 是否绑定端口（默认 true）
     * @return zmq::socket_t 响应者套接字对象
     *
     * @throw std::runtime_error 如果 ZeroMQ 操作失败
     *
     * @note 响应者通常绑定端口，等待请求者连接
     */
    zmq::socket_t createReplier(const std::string& endpoint, bool bind = true) {
        // 创建 REP 类型套接字
        zmq::socket_t socket(context_, ZMQ_REP);

        try {
            configureSocket(endpoint, bind, socket);
        } catch (const zmq::error_t& e) {
            handleZmqError(e);
        }

        return socket;
    }

    // ==================== 消息发送方法 ====================

    /**
     * @brief 发送字符串消息
     *
     * @param socket 目标套接字
     * @param content 字符串内容
     * @param wait_response 是否等待响应（默认 false）
     * @return true 发送成功
     * @return false 发送失败
     *
     * @note 当 wait_response=true 时，使用阻塞发送模式；
     *       当 wait_response=false 时，使用非阻塞发送模式。
     */
    bool sendMessage(zmq::socket_t& socket, const std::string& content, bool wait_response = false) {
        try {
            // 创建消息对象
            zmq::message_t message(content.size());
            memcpy(message.data(), content.data(), content.size());

            // 设置发送标志
            zmq::send_flags flags = wait_response ? zmq::send_flags::none : zmq::send_flags::dontwait;

            // 发送消息并检查结果
            auto result = socket.send(message, flags);
            return result.has_value(); // 修复：检查是否发送成功
        } catch (const zmq::error_t& e) {
            // 处理 ZeroMQ 错误
            handleZmqError(e);
            return false;
        }
    }

    /**
     * @brief 发送整数消息
     *
     * @param socket 目标套接字
     * @param value 整数值
     * @param wait_response 是否等待响应（默认 false）
     * @return true 发送成功
     * @return false 发送失败
     *
     * @note 内部将整数转换为字符串后发送
     */
    bool sendInt(zmq::socket_t& socket, int value, bool wait_response = false) {
        return sendMessage(socket, std::to_string(value), wait_response);
    }

    /**
     * @brief 发送长整数消息
     *
     * @param socket 目标套接字
     * @param value 长整数值
     * @param wait_response 是否等待响应（默认 false）
     * @return true 发送成功
     * @return false 发送失败
     *
     * @note 内部将长整数转换为字符串后发送
     */
    bool sendLong(zmq::socket_t& socket, long value, bool wait_response = false) {
        return sendMessage(socket, std::to_string(value), wait_response);
    }

    /**
     * @brief 发送结构体消息（POD 类型）
     *
     * @tparam T 结构体类型
     * @param socket 目标套接字
     * @param data 结构体实例
     * @param wait_response 是否等待响应（默认 false）
     * @return true 发送成功
     * @return false 发送失败
     *
     * @note 要求结构体必须是平凡可复制的（POD 类型）
     */
    template <typename T> bool sendStruct(zmq::socket_t& socket, const T& data, bool wait_response = false) {
        // 检查类型是否是平凡可复制的
        static_assert(std::is_trivially_copyable<T>::value, "类型必须为平凡可复制（POD）");

        try {
            // 创建消息对象
            zmq::message_t message(sizeof(T));
            memcpy(message.data(), &data, sizeof(T));

            // 设置发送标志
            zmq::send_flags flags = wait_response ? zmq::send_flags::none : zmq::send_flags::dontwait;

            // 发送消息并检查结果
            auto result = socket.send(message, flags);
            return result.has_value(); // 修复：检查是否发送成功
        } catch (const zmq::error_t& e) {
            handleZmqError(e);
            return false;
        }
    }

    /**
     * @brief 发送智能指针指向的结构体
     *
     * @tparam T 结构体类型
     * @param socket 目标套接字
     * @param data_ptr 结构体智能指针
     * @param wait_response 是否等待响应（默认 false）
     * @return true 发送成功
     * @return false 发送失败
     *
     * @note 自动检查指针有效性
     */
    template <typename T>
    bool sendStruct(zmq::socket_t& socket, const std::shared_ptr<T>& data_ptr, bool wait_response = false) {
        if (!data_ptr) {
            std::cerr << "错误：空指针无法发送" << std::endl;
            return false;
        }
        return sendStruct(socket, *data_ptr, wait_response);
    }

    // ==================== 消息接收方法 ====================

    /**
     * @brief 接收原始消息
     *
     * @param socket 源套接字
     * @param message 接收到的消息（输出参数）
     * @param timeout_ms 超时时间（毫秒），-1 表示无限等待
     * @return true 接收成功
     * @return false 接收失败或超时
     */
    bool receiveMessage(zmq::socket_t& socket, zmq::message_t& message, int timeout_ms = -1) {
        try {
            // 设置接收超时
            if (timeout_ms >= 0) {
                socket.setsockopt(ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
            }

            // 接收消息
            return socket.recv(&message);
        } catch (const zmq::error_t& e) {
            handleZmqError(e);
            return false;
        }
    }

    /**
     * @brief 接收字符串消息
     *
     * @param socket 源套接字
     * @param message 接收到的字符串（输出参数）
     * @param timeout_ms 超时时间（毫秒）
     * @return true 接收成功
     * @return false 接收失败或超时
     */
    bool receiveString(zmq::socket_t& socket, std::string& message, int timeout_ms = -1) {
        zmq::message_t zmq_msg;
        if (!receiveMessage(socket, zmq_msg, timeout_ms)) {
            return false;
        }

        // 将消息转换为字符串
        message.assign(static_cast<const char*>(zmq_msg.data()), zmq_msg.size());
        return true;
    }

    /**
     * @brief 接收结构体消息（POD 类型）
     *
     * @tparam T 结构体类型
     * @param socket 源套接字
     * @param result 接收结果结构体（输出参数）
     * @param timeout_ms 超时时间（毫秒）
     * @return true 接收成功
     * @return false 接收失败或超时
     */
    template <typename T> bool receiveStruct(zmq::socket_t& socket, T& result, int timeout_ms = -1) {
        // 检查类型是否是平凡可复制的
        static_assert(std::is_trivially_copyable<T>::value, "类型必须为平凡可复制（POD）");

        zmq::message_t message;
        if (!receiveMessage(socket, message, timeout_ms)) {
            return false;
        }

        // 检查消息大小是否匹配
        if (message.size() != sizeof(T)) {
            std::cerr << "错误：消息大小不匹配，期望 " << sizeof(T) << " 字节，实际 " << message.size() << " 字节"
                      << std::endl;
            return false;
        }

        // 复制数据到结构体
        memcpy(&result, message.data(), sizeof(T));
        return true;
    }

    /**
     * @brief 接收结构体消息（智能指针版本）
     *
     * @tparam T 结构体类型
     * @param socket 源套接字
     * @param timeout_ms 超时时间（毫秒）
     * @return std::shared_ptr<T> 结构体智能指针（nullptr 表示失败）
     */
    template <typename T> std::shared_ptr<T> receiveStructPtr(zmq::socket_t& socket, int timeout_ms = -1) {
        T result;
        if (receiveStruct(socket, result, timeout_ms)) {
            return std::make_shared<T>(result);
        }
        return nullptr;
    }
};