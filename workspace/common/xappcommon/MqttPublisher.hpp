#ifndef COMMON_UTILS_MQTT_PUBLISHER_HPP
#define COMMON_UTILS_MQTT_PUBLISHER_HPP

#include <glog/logging.h>
#include <mosquitto.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "SafeQueue.hpp"
#include "json.hpp"

namespace app_common {
    class MqttPublisher {
    public:
        static MqttPublisher& Instance() {
            static MqttPublisher inst;
            return inst;
        }

    public:
        MqttPublisher() : _stopped(true) {
            m_queue.set_max_num_items(50);
        }

        ~MqttPublisher() {
            stop();
        }

        bool ready() {
            return !_stopped;
        }

        void pub(const std::string& topic, const std::string& data) {
            if (!m_queue.push(std::make_pair(topic, data))) {
                LOG(ERROR) << "add MQTT msg to publish for " << topic << " failed!";
                return;
            }
        }

        bool start(const std::string& broker_ip, std::int32_t broker_port) {
            std::lock_guard<std::mutex> guard(_mtx);
            if (_stopped) {
                _broker_ip = broker_ip;
                _broker_port = broker_port;

                _mosq = mosquitto_new(nullptr, true, this);
                if (nullptr == _mosq) {
                    LOG(ERROR) << "mosquitto_new error";
                    return false;
                }

                auto r = mosquitto_connect(_mosq, _broker_ip.c_str(), _broker_port, 60);
                if (r != MOSQ_ERR_SUCCESS) {
                    LOG(ERROR) << "mosquitto_connect error: " << mosquitto_strerror(r);
                    mosquitto_destroy(_mosq);
                    _mosq = nullptr;
                    return false;
                }

                _thd = std::thread([this]() { work(); });
                _stopped = false;
            }

            return true;
        }

        void stop() {
            std::lock_guard<std::mutex> guard(_mtx);
            _stopped = true;
            if (_thd.joinable()) {
                _thd.join();
            }

            if (_mosq != nullptr) {
                mosquitto_disconnect(_mosq);
                mosquitto_destroy(_mosq);
                _mosq = nullptr;
            }
        }

    private:
        void work() {
            while (!_stopped) {
                auto ret = mosquitto_loop(_mosq, 10, 1);
                if (MOSQ_ERR_SUCCESS == ret) {
                    std::pair<std::string, std::string> to_pub;
                    if (m_queue.try_pop(to_pub)) {
                        auto r = mosquitto_publish(_mosq, nullptr, to_pub.first.c_str(), to_pub.second.length(),
                                                   to_pub.second.c_str(), 1, false);
                        if (r != MOSQ_ERR_SUCCESS) {
                            LOG(ERROR) << "mosquitto_publish error, topic: " << to_pub.first
                                       << ", payload: " << to_pub.second << ", error: " << mosquitto_strerror(r);
                        } else {
                            LOG(INFO) << "mosquitto_publish ok, topic: " << to_pub.first
                                      << ", payload: " << to_pub.second;
                        }
                    }
                } else if (MOSQ_ERR_CONN_LOST == ret || MOSQ_ERR_NO_CONN == ret) {
                    auto r = mosquitto_reconnect(_mosq);
                    if (r == MOSQ_ERR_SUCCESS) {
                        LOG(INFO) << "mosquitto_reconnect ok";
                    } else {
                        LOG(ERROR) << "mosquitto_reconnect error: " << mosquitto_strerror(r);
                    }
                }
            }
            return;
        }

    private:
        std::mutex   _mtx;
        std::string  _broker_ip;
        std::int32_t _broker_port;
        mosquitto*   _mosq = nullptr;

        std::atomic_bool                               _stopped;
        std::thread                                    _thd;
        SafeQueue<std::pair<std::string, std::string>> m_queue;
    };
}; // namespace app_common
#endif
