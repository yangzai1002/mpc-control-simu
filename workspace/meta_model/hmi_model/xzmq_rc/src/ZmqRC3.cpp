#include "ZmqRC3.hpp"
#include "ap_log/ap_log_interface.h"
#include "boost/type_traits.hpp"
#include "config_loader/custom_stack.h"
#include "ap_log/ap_log_interface.h"
#include "monitor/faulthandle_api.hpp"
#include "shm_util/shm_param.h"
using json = nlohmann::json;

// #include "params/cam_shm_param.h"
static void EnsureDirectoryExists(const std::string &path) {
    if (path.empty())
        return;
    std::string path_copy = path;
    if (path_copy.back() == '/')
        path_copy.pop_back();

    std::string       current_path;
    std::stringstream ss(path_copy);
    std::string       segment;

    if (!path_copy.empty() && path_copy[0] == '/')
        current_path = "/";

    while (std::getline(ss, segment, '/')) {
        if (segment.empty())
            continue;
        current_path += segment + "/";
        mkdir(current_path.c_str(), 0755);
    }
}

ZmqRC3::ZmqRC3() : _zmq_construct(nullptr) {
    _remote_control = std::make_shared<ara::adsfi::AppRemoteControl>();

    auto        ptr = CustomStack::Instance();
    std::string prefix;

    auto full_path = prefix + "hmi/common/default/speed_limit";
    if (!ptr->GetProjectConfigValue(full_path, _speed_limit)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_rc") << full_path << "=" << _speed_limit;

    full_path = prefix + "hmi/common/default/over_time_tick";
    if (!ptr->GetProjectConfigValue(full_path, _over_time_tick)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_rc") << full_path << "=" << _over_time_tick;

    full_path = prefix + "hmi/common/default/reset_time_fault";
    if (!ptr->GetProjectConfigValue(full_path, _reset_time_fault)) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_rc") << full_path << "=" << _reset_time_fault;

    full_path = prefix + "hmi/zmq/remote_control_endpoint";
    if (!ptr->GetProjectConfigValue(full_path, _remote_control_endpoint) || _remote_control_endpoint.empty()) {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }
    ApInfo("zmq_rc") << full_path << "=" << _remote_control_endpoint;

    std::string persistence_dir;
    full_path = prefix + "hmi/zmq/persistence_path";
    if (ptr->GetProjectConfigValue(full_path, persistence_dir)) {
        app_common::StringUtils::trim(persistence_dir);
        if (!persistence_dir.empty() && persistence_dir.back() != '/') {
            persistence_dir += "/";
        }
        // 创建目录
        EnsureDirectoryExists(persistence_dir);
        // 拼接文件名
        _json_file_path = persistence_dir + "remote_control.json";
    } else {
        _ec.ec_add(Ec409::_ERRORCODE_CONFIG, full_path);
        throw std::runtime_error("read project config " + full_path + " failed");
    }

    _ec.ec_remove(Ec409::_ERRORCODE_CONFIG);

    state_read(*_remote_control);

    _last_parsed_state.gear = _remote_control->gear;
    _last_parsed_state.remotectrl_flag = _remote_control->remotectrl_flag;
    _last_parsed_state.estop_flag = _remote_control->estop_flag;
    _last_parsed_state.speed_limit = _remote_control->speed_limit;
    _last_parsed_state.security_assist_enabled = _remote_control->security_assist_enabled;

    _zmq_construct = std::make_shared<ZmqConstruct>(1);

    std::thread([this]() { remote_loop(); }).detach();
    std::thread([this]() { persistence_loop(); }).detach();
}

void ZmqRC3::values(ara::adsfi::AppRemoteControl &remote_control) {
    std::lock_guard<std::mutex> lck(_mtx);
    remote_control = *_remote_control;
}

void ZmqRC3::on_data_change() {
    std::lock_guard<std::mutex> lg(_mtx);
    if (_remote_control->remotectrl_flag != 0) {
        int64_t now =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count();
        if (now - _last_tick_time > _over_time_tick) {
            _remote_control->accelerator = 0;
            _remote_control->brake = 30000;

            if (!_over_time_tick_flag) {
                _over_time_tick_flag = true;
                const std::string error_msg =
                    "遥控数据接收超时(心跳断连时长大于阈值" + std::to_string(_over_time_tick) + "ms)";

                _ec.ec_add(Ec409::_ERRORCODE_REMOTE_CONTROL_RECV_TIMEOUT, error_msg);
                ApInfo("zmq_rc") << "上报故障：" << error_msg;
            }
            _timestamps_buffer.clear();
            _recover_start_time = 0;

        } else {
            if (_recover_start_time == 0) {
                _recover_start_time = now;
            } else if (now - _recover_start_time >= _reset_time_fault) {
                if (_over_time_tick_flag) {
                    _ec.ec_remove(Ec409::_ERRORCODE_REMOTE_CONTROL_RECV_TIMEOUT);
                    ApInfo("zmq_rc") << "遥控数据接收超时(心跳断连时长大于阈值)故障恢复---";
                    _over_time_tick_flag = false;
                    _remote_control->brake = 0;
                }
                _recover_start_time = 0;
            }
        }

        constexpr int MAX_COUNT = 5;
        constexpr int TIME_WINDOW = 1000;
        int           delay_count = 0;

        _timestamps_buffer.push_back(_last_tick_time);
        while (!_timestamps_buffer.empty() && _timestamps_buffer.front() < _last_tick_time - TIME_WINDOW) {
            _timestamps_buffer.erase(_timestamps_buffer.begin());
        }

        for (size_t i = 1; i < _timestamps_buffer.size(); ++i) {
            if (_timestamps_buffer[i] - _timestamps_buffer[i - 1] > 100) {
                delay_count++;
            }
        }

        if (delay_count >= MAX_COUNT) {
            if (_remote_control->speed_limit > 10) {
                _remote_control->speed_limit = 10;
                ApInfo("zmq_rc") << "[遥控超时] 1秒内连续" << delay_count << "次延迟>100ms，限速降至"
                                 << _remote_control->speed_limit << "km/h";
            }

            if (!_delay_flag) {
                _ec.ec_add(
                    Ec409::_ERRORCODE_REMOTE_CONTROL_DELAY_ACCUM,
                    std::string("遥控数据接收延迟累积(1秒内连续") + std::to_string(delay_count) + "次延迟>100ms)");
                ApInfo("zmq_rc") << "[遥控延迟累积] 上报故障，延迟次数:" << delay_count;
                _delay_flag = true;
            }
            _timestamps_buffer.clear();    // 清空时间窗
            _delay_recover_start_time = 0; // 重置恢复计时
        } else {
            bool has_bad_interval = false;
            for (size_t i = 1; i < _timestamps_buffer.size(); ++i) {
                if (_timestamps_buffer[i] - _timestamps_buffer[i - 1] > 100) {
                    has_bad_interval = true;
                    break;
                }
            }

            if (has_bad_interval) {
                _delay_recover_start_time = 0; // 中断恢复
            } else {
                if (_delay_recover_start_time == 0) {
                    _delay_recover_start_time = now;
                } else if (now - _delay_recover_start_time >= _reset_time_fault) {
                    if (_delay_flag) {
                        _ec.ec_remove(Ec409::_ERRORCODE_REMOTE_CONTROL_DELAY_ACCUM);
                        _delay_flag = false;
                    }
                    _delay_recover_start_time = 0;
                }
            }
        }
    } else {
        if (_over_time_tick_flag) {
            _ec.ec_remove(Ec409::_ERRORCODE_REMOTE_CONTROL_RECV_TIMEOUT);
            _over_time_tick_flag = false;

            _remote_control->brake = 0;
        }

        if (_delay_flag) {
            _ec.ec_remove(Ec409::_ERRORCODE_REMOTE_CONTROL_DELAY_ACCUM);
            _delay_flag = false;
        }
    }
}

void ZmqRC3::remote_loop() {
    const std::string kChassisTopic = "manager2chassis_control";

    while (true) {
        // 如果 socket 未创建，尝试重建
        if (_zmq_socket_sub == nullptr) {
            try {
                _zmq_socket_sub = _zmq_construct->createSubscriber(_remote_control_endpoint, {}, false);
                _zmq_socket_sub->setsockopt(ZMQ_SUBSCRIBE, "", 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                _ec.ec_remove(Ec409::_ERRORCODE_ZMQ_CONNECT);
            } catch (const std::exception &e) {
                _ec.ec_add(Ec409::_ERRORCODE_ZMQ_CONNECT, _remote_control_endpoint);
                _zmq_socket_sub.reset();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

        try {
            // poll 判断是否有数据可读（避免忙等）
            zmq::pollitem_t item{*(_zmq_socket_sub.get()), 0, ZMQ_POLLIN, 0};
            int             rc = zmq::poll(&item, 1, 500); // 等 500ms
            if (rc == 0) {
                continue;
            }

            // ---------- 读取所有 frames（multipart-safe） ----------
            std::vector<zmq::message_t> frames;
            zmq::message_t              frame;
            auto                        recv_res = _zmq_socket_sub->recv(frame, zmq::recv_flags::none);
            if (!recv_res.has_value()) {
                // 理论上不会发生（因 poll 已表明可读），但仍做保护
                ApError("zmq_rc") << "ryf--- recv failed unexpectedly (initial frame)";
                continue;
            }
            frames.push_back(std::move(frame));
            while (frames.back().more()) {
                zmq::message_t next;
                auto           r2 = _zmq_socket_sub->recv(next, zmq::recv_flags::none);
                if (!r2.has_value()) {
                    ApError("zmq_rc") << "ryf--- recv failed unexpectedly during multipart read";
                    break;
                }
                frames.push_back(std::move(next));
            }

            if (frames.size() != 2) {
                ApError("zmq_rc") << "ryf--- Unexpected frame count: " << frames.size()
                                  << " (expect exactly 2 frames: topic + payload). Discarding message.";
                for (size_t i = 0; i < frames.size(); ++i) {
                    size_t             sz = frames[i].size();
                    std::ostringstream oss;
                    oss << "ryf--- frame[" << i << "] size=" << sz << " preview=";
                    size_t         print_len = std::min(sz, size_t(16));
                    const uint8_t *p = static_cast<const uint8_t *>(frames[i].data());
                    for (size_t j = 0; j < print_len; ++j) {
                        oss << std::hex << std::setw(2) << std::setfill('0') << (int)p[j];
                    }
                    ApInfo("zmq_rc") << oss.str();
                }
                continue;
            }

            std::string topic_str(reinterpret_cast<const char *>(frames[0].data()), frames[0].size());
            bool        topic_printable = true;
            for (size_t i = 0; i < frames[0].size(); ++i) {
                unsigned char c = reinterpret_cast<const unsigned char *>(frames[0].data())[i];
                if (c < 32 || c > 126) {
                    topic_printable = false;
                    break;
                }
            }
            if (!topic_printable) {
                ApError("zmq_rc") << "ryf--- topic frame is not printable ASCII -> discard";
                continue;
            }

            if (topic_str != kChassisTopic) {
                ApInfo("zmq_rc") << "ryf--- ignoring topic: " << topic_str;
                continue;
            }

            // ---------- 提取 payload（第二帧） ----------
            const uint8_t *payload_buf = static_cast<const uint8_t *>(frames[1].data());
            size_t         payload_sz = frames[1].size();
            if (payload_sz == 0) {
                ApError("zmq_rc") << "ryf--- payload empty for topic " << topic_str << " -> discard";
                continue;
            }

            {
                std::lock_guard<std::mutex> lg(_mtx);
                size_t                      print_len = std::min(payload_sz, size_t(64));
                std::ostringstream          oss;
                oss << "ryf--- payload hex:";
                for (size_t i = 0; i < print_len; ++i) {
                    oss << ' ' << std::hex << std::setw(2) << std::setfill('0') << (int)payload_buf[i];
                }
                ApInfo("zmq_rc") << oss.str();

                try {
                    size_t offset = 0;
                    auto   obj = protocol_common::msg_parse<protocol_common::manager2chassis_control>(payload_buf,
                                                                                                    payload_sz, offset);
                    (void)offset;
                    auto sp = std::make_shared<protocol_common::manager2chassis_control>(std::move(obj));
                    parseRemoteControlData(sp);
                } catch (const std::exception &ex) {
                    ApError("zmq_rc") << "ryf---- parse manager2chassis_control failed: " << ex.what();
                }
            }
        } catch (const zmq::error_t &e) {
            ApError("zmq_rc") << "ryf--- ZMQ error in remote_loop: " << e.what() << " (reset socket)";
            _zmq_socket_sub.reset();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } catch (const std::exception &e) {
            ApError("zmq_rc") << "ryf--- exception in remote_loop: " << e.what();
        } catch (...) {
            ApError("zmq_rc") << "ryf--- unknown error in remote_loop (reset socket)";
            _zmq_socket_sub.reset();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

void ZmqRC3::parseRemoteControlData(std::shared_ptr<protocol_common::manager2chassis_control> struct_ptr) {
    if (!struct_ptr) {
        return;
    }

    const int RAW_NEUT = 100;
    const int RAW_MAX = 200;
    const int PROTO_MAX = 32767;

    // 仅在遥控干预(4)、主动应急避险(2)以及横纵向遥控量不为中心位(100)时进入遥控使能模式
    if ((struct_ptr->control_mode_req == 4 || struct_ptr->control_mode_req == 2) &&
        (struct_ptr->throttle_braking != 100 || struct_ptr->steering != 100)) {
        _remote_control->remotectrl_flag = 1;
    } else {
        _remote_control->remotectrl_flag = 0;
    }
    // 仅在主动应急避险(2)以及横纵向遥控量不为中心位(100)时进入主动应急避险模式
    if (struct_ptr->control_mode_req == 2 && (struct_ptr->throttle_braking != 100 || struct_ptr->steering != 100)) {
        _remote_control->security_assist_enabled = 1;
    } else {
        _remote_control->security_assist_enabled = 0;
    }

    uint16_t tb_raw = struct_ptr->throttle_braking;
    if (tb_raw > RAW_MAX) {
        tb_raw = RAW_MAX;
    }

    uint8_t gear = struct_ptr->gear;
    uint8_t heart_tick = struct_ptr->heart;
    int     steering_raw_in = static_cast<int>(struct_ptr->steering);

    int steering_center = RAW_NEUT - steering_raw_in; // 左正右负
    if (steering_center < -100) {
        steering_center = -100;
    }
    if (steering_center > 100) {
        steering_center = 100;
    }
    int    steering_mapped_int = steering_center * PROTO_MAX / 100;
    double steering_mapped = static_cast<double>(steering_mapped_int);

    int accelerator_int = 0;
    int brake_int = 0;
    if (tb_raw == RAW_NEUT) {
        accelerator_int = 0;
        brake_int = 0;
    } else if (tb_raw > RAW_NEUT) {
        int acc_pct = static_cast<int>(tb_raw) - RAW_NEUT;
        accelerator_int = acc_pct * PROTO_MAX / 100;
    } else {
        int brake_pct = RAW_NEUT - static_cast<int>(tb_raw);
        brake_int = brake_pct * PROTO_MAX / 100;
    }

    auto _gear = static_cast<int>(gear);
    if (_gear > 4) {
        _gear = 0;
    }
    int requested_speed = static_cast<int>(struct_ptr->speed_limit);
    if (requested_speed > _speed_limit) {
        _remote_control->speed_limit = _speed_limit;
    } else if (requested_speed < 0) {
        _remote_control->speed_limit = 0;
    } else {
        _remote_control->speed_limit = requested_speed;
    }
    _remote_control->gear = _gear;
    _remote_control->accelerator = static_cast<double>(accelerator_int);
    _remote_control->brake = static_cast<double>(brake_int);
    _remote_control->steering_wheel = -steering_mapped;
    _remote_control->tick = static_cast<int>(heart_tick);
    if (_last_tick_value != _remote_control->tick) {
        _last_tick_value = _remote_control->tick;
        _last_tick_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count();
    }
    check_and_notify_change(*_remote_control);
}

bool ZmqRC3::check_and_notify_change(const ara::adsfi::AppRemoteControl &current) {
    bool changed = false;
    if (current.gear != _last_parsed_state.gear)
        changed = true;
    else if (current.remotectrl_flag != _last_parsed_state.remotectrl_flag)
        changed = true;
    else if (current.estop_flag != _last_parsed_state.estop_flag)
        changed = true;
    else if (current.speed_limit != _last_parsed_state.speed_limit)
        changed = true;
    else if (current.security_assist_enabled != _last_parsed_state.security_assist_enabled)
        changed = true;

    if (changed) {
        // 更新缓存
        _last_parsed_state.gear = current.gear;
        _last_parsed_state.remotectrl_flag = current.remotectrl_flag;
        _last_parsed_state.estop_flag = current.estop_flag;
        _last_parsed_state.speed_limit = current.speed_limit;
        _last_parsed_state.security_assist_enabled = current.security_assist_enabled;

        // 唤醒线程
        _data_changed = true;
        _cv_persistence.notify_one();
    }
    return changed;
}

void ZmqRC3::state_read(ara::adsfi::AppRemoteControl &control) {
    if (_json_file_path.empty())
        return;

    std::ifstream ifs(_json_file_path);
    if (!ifs.is_open())
        return;

    try {
        json j;
        ifs >> j;
        ifs.close();

        if (j.contains("timestamp")) {
            auto   timestamp_count = j["timestamp"].get<std::int64_t>();
            auto   now = std::chrono::steady_clock::now().time_since_epoch().count();
            double diff_seconds = static_cast<double>(std::llabs(now - timestamp_count)) / 1e9;

            if (diff_seconds > 10.0) {
                ApInfo("zmq_rc") << "[Persistence] Data expired (>10s). Reset to default.";
                return;
            }
            ApInfo("zmq_rc") << "[Persistence] Data loaded successfully. Duration: " << diff_seconds << "s";
        } else {
            return;
        }

        if (j.contains("value")) {
            auto &v = j["value"];
            control.gear = v.value("gear", 0);
            control.remotectrl_flag = v.value("remotectrl_flag", 0);
            control.estop_flag = v.value("estop_flag", 0);
            control.speed_limit = v.value("speed_limit", 0);
            control.security_assist_enabled = v.value("security_assist_enabled", 0);
        }
    } catch (...) {
    }
}

void ZmqRC3::state_write(const ara::adsfi::AppRemoteControl &control) {
    if (_json_file_path.empty())
        return;

    try {
        json j;
        j["timestamp"] = std::chrono::steady_clock::now().time_since_epoch().count();

        j["value"] = {{"gear", control.gear},
                      {"remotectrl_flag", control.remotectrl_flag},
                      {"estop_flag", control.estop_flag},
                      {"speed_limit", control.speed_limit},
                      {"security_assist_enabled", control.security_assist_enabled}};

        std::string tmp_path = _json_file_path + ".tmp";
        std::string content = j.dump(4);

        // 第一次尝试打开
        int fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1 && errno == ENOENT) {
            size_t pos = _json_file_path.find_last_of('/');
            if (pos != std::string::npos) {
                std::string dir = _json_file_path.substr(0, pos);
                EnsureDirectoryExists(dir);
                // 重试打开
                fd = open(tmp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
        }

        if (fd != -1) {
            // 只有完全写入成功 (write) 且 刷盘成功 (fsync) 后，才标记为成功
            bool    write_ok = false;
            ssize_t written = write(fd, content.c_str(), content.size());

            if (written == static_cast<ssize_t>(content.size())) {
                if (fsync(fd) == 0) {
                    write_ok = true;
                }
            } else {
                ApError("zmq_rc") << "Partial write or write error. Expected: " << content.size()
                                  << ", Actual: " << written;
            }

            close(fd);

            // 只有写入完全成功才进行重命名覆盖
            if (write_ok) {
                if (rename(tmp_path.c_str(), _json_file_path.c_str()) != 0) {
                    ApError("zmq_rc") << "Rename failed: " << strerror(errno);
                }
            } else {
                // 如果写入失败，删除这个损坏的临时文件，以免残留
                unlink(tmp_path.c_str());
            }
        } else {
            ApError("zmq_rc") << "Failed to open persistence file: " << strerror(errno);
        }
    } catch (const std::exception &e) {
        ApError("zmq_rc") << "Exception in state_write: " << e.what();
    } catch (...) {
        ApError("zmq_rc") << "Unknown error in state_write.";
    }
}

void ZmqRC3::persistence_loop() {
    while (true) {
        try {
            {
                std::unique_lock<std::mutex> lk(_mtx_persistence);
                // 1秒超时（心跳） 或 被 notify 唤醒（立即保存）
                _cv_persistence.wait_for(lk, std::chrono::seconds(1), [this] { return _data_changed.load(); });
                _data_changed = false;
            }

            ara::adsfi::AppRemoteControl data_to_save;
            {
                std::lock_guard<std::mutex> business_lock(_mtx);
                data_to_save = *_remote_control;
            }

            state_write(data_to_save);

        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
