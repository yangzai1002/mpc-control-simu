#pragma once
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "JsonUtils.hpp"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_appvideoprofile.h"
#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_taskmanagestatus.h"
#include "ara/adsfi/impl_type_videortpparameter.h"
#include "ap_log/ap_log_interface.h"

// 定义返航点状态结构
struct ReturnPointStatus {
    bool return_point_flag = false;
};

struct PersistenceConfig {
    static std::string &GetPath() {
        static std::string path = "./persistence/";
        return path;
    }
};

inline void mkdir_recursive(const std::string &path) {
    if (path.empty()) {
        return;
    }
    std::string current_path;
    std::string path_copy = path;
    if (path_copy.back() != '/') {
        path_copy += "/";
    }

    std::stringstream ss(path_copy);
    std::string       segment;
    if (path_copy[0] == '/') {
        current_path = "/";
    }

    while (std::getline(ss, segment, '/')) {
        if (segment.empty()) {
            continue;
        }
        current_path += segment + "/";
        if (mkdir(current_path.c_str(), 0755) != 0 && errno != EEXIST) {
            ApError("zmq_task") << "[Persistence] Failed to create directory: " << current_path
                                << ", Error: " << strerror(errno);
            return;
        }
    }
}

inline void InitPersistence(const std::string &path) {
    if (path.empty()) {
        return;
    }
    std::string &g_path = PersistenceConfig::GetPath();
    g_path = path;
    if (g_path.back() != '/') {
        g_path += "/";
    }

    mkdir_recursive(g_path);
    ApInfo("zmq_task") << "[Persistence] Storage path initialized to: " << g_path;
}

inline std::string GetFilePath(const std::string &filename) {
    return PersistenceConfig::GetPath() + filename;
}

inline void WriteJsonFile(const std::string &filename, const nlohmann::json &j) {
    std::string target_path = GetFilePath(filename);
    std::string temp_path = target_path + ".tmp";

    std::string data;
    try {
        data = j.dump(4);
    } catch (const nlohmann::json::exception &e) {
        ApError("zmq_task") << "[Persistence] JSON dump failed for " << filename << ": " << e.what();
        return;
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Unexpected exception during dump for " << filename << ": " << e.what();
        return;
    }

    int fd = open(temp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1 && errno == ENOENT) {
        mkdir_recursive(PersistenceConfig::GetPath());
        fd = open(temp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    if (fd == -1) {
        ApError("zmq_task") << "[Persistence] Write failed (Open Error): " << strerror(errno) << " Path: " << temp_path;
        return;
    }

    bool    write_success = false;
    ssize_t written = write(fd, data.c_str(), data.size());
    if (written == static_cast<ssize_t>(data.size())) {
        if (fsync(fd) == 0) {
            write_success = true;
        } else {
            ApError("zmq_task") << "[Persistence] fsync failed for " << filename << ": " << strerror(errno);
        }
    } else {
        ApError("zmq_task") << "[Persistence] Write fail or partial write for " << filename
                            << ". Expected: " << data.size() << " Actual: " << written << " Error: " << strerror(errno);
    }

    close(fd);

    if (write_success) {
        if (rename(temp_path.c_str(), target_path.c_str()) != 0) {
            ApError("zmq_task") << "[Persistence] Rename failed for " << filename << ": " << strerror(errno);
            unlink(temp_path.c_str());
        }
    } else {
        unlink(temp_path.c_str());
    }
}

inline bool ReadJsonFile(const std::string &filename, nlohmann::json &j_out) {
    std::string full_path = GetFilePath(filename);
    try {
        std::ifstream ifs(full_path);
        if (!ifs.is_open()) {
            return false;
        }
        if (ifs.peek() == std::ifstream::traits_type::eof()) {
            return false;
        }

        ifs >> j_out;
        return true;
    } catch (const nlohmann::json::parse_error &e) {
        ApError("zmq_task") << "[Persistence] JSON Parse error in " << filename << ": " << e.what();
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Read exception in " << filename << ": " << e.what();
    } catch (...) {
        ApError("zmq_task") << "[Persistence] Unknown exception in ReadJsonFile for " << filename;
    }
    return false;
}

// ====================================================================
// 1. BusinessCommand
// ====================================================================
inline void state_write(const ara::adsfi::BusinessCommand &business_command) {
    try {
        nlohmann::json j;
        j["timestamp"] = std::chrono::steady_clock::now().time_since_epoch().count();
        j["value"] = {
            {"business_mode", business_command.business_mode},
            {"record_trigger_flag", business_command.record_trigger_flag},
            //   {"loc_control_mode", business_command.loc_control_mode},
            //   {"loc_strategy", business_command.loc_strategy},
            {"command", business_command.param.command},
            {"task_avoid_mode", business_command.param.task_avoid_mode},
            {"max_speed", business_command.param.max_speed},
            {"patrol_name", business_command.param.patrol_name},
            {"patrol_dest", business_command.param.patrol_dest},
            {"patrol_type", business_command.param.patrol_type},
            {"patrol_loop", business_command.param.patrol_loop},
            {"follow_min_dis", business_command.param.follow_min_dis},
            {"follow_thw", business_command.param.follow_thw},
            {"follow_mode", business_command.param.follow_mode},
            {"follow_x", business_command.param.follow_x},
            {"follow_y", business_command.param.follow_y},
            {"view_width", business_command.param.view_width},
            {"view_height", business_command.param.view_height},
            {"id", business_command.param.id},
            //   {"pose",
            //    {{"x", business_command.param.pose.x},
            //     {"y", business_command.param.pose.y},
            //     {"angle", business_command.param.pose.angle}}
            // }
        };
        WriteJsonFile("business_command.json", j);
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] State write BusinessCommand failed: " << e.what();
    }
}

inline void state_read(ara::adsfi::BusinessCommand &business_command) {
    nlohmann::json j;
    if (!ReadJsonFile("business_command.json", j)) {
        return;
    }

    try {
        if (j.contains("timestamp")) {
            auto   timestamp_count = j["timestamp"].get<std::int64_t>();
            auto   now = std::chrono::steady_clock::now().time_since_epoch().count();
            double diff_seconds = static_cast<double>(std::llabs(now - timestamp_count)) / 1e9;

            if (diff_seconds > 10.0) {
                ApInfo("zmq_task") << "[Persistence] BusinessCommand expired! Duration: " << diff_seconds
                                   << "s. Ignore.";
                return;
            }
            ApInfo("zmq_task") << "[Persistence] BusinessCommand valid. Duration: " << diff_seconds << "s.";
        } else {
            ApInfo("zmq_task") << "[Persistence] BusinessCommand missing timestamp. Ignore.";
            return;
        }

        if (j.contains("value")) {
            auto &v = j["value"];
            business_command.business_mode = v.value("business_mode", 0);
            business_command.record_trigger_flag = v.value("record_trigger_flag", 0);
            // business_command.loc_control_mode = v.value("loc_control_mode", 0);
            // business_command.loc_strategy = v.value("loc_strategy", 0);
            business_command.param.command = v.value("command", 0);
            business_command.param.task_avoid_mode = v.value("task_avoid_mode", 0);
            business_command.param.max_speed = v.value("max_speed", 0.0);
            business_command.param.patrol_name = v.value("patrol_name", "");
            business_command.param.patrol_dest = v.value("patrol_dest", "");
            business_command.param.patrol_type = v.value("patrol_type", 0);

            if (v.contains("patrol_loop")) {
                if (v["patrol_loop"].is_boolean()) {
                    business_command.param.patrol_loop = v["patrol_loop"].get<bool>();
                } else {
                    business_command.param.patrol_loop = (v.value("patrol_loop", 0) != 0);
                }
            }

            business_command.param.follow_min_dis = v.value("follow_min_dis", 0.0);
            business_command.param.follow_thw = v.value("follow_thw", 0.0);
            business_command.param.follow_mode = v.value("follow_mode", 0);
            business_command.param.follow_x = v.value("follow_x", 0);
            business_command.param.follow_y = v.value("follow_y", 0);
            business_command.param.view_width = v.value("view_width", 1920);
            business_command.param.view_height = v.value("view_height", 1080);
            business_command.param.id = v.value("id", -1);

            // if (v.contains("pose")) {
            //     auto &p = v["pose"];
            //     business_command.param.pose.x = p.value("x", 0.0);
            //     business_command.param.pose.y = p.value("y", 0.0);
            //     business_command.param.pose.angle = p.value("angle", 0.0);
            // }
        }
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Parse BusinessCommand Error: " << e.what();
    } catch (...) {
        ApError("zmq_task") << "[Persistence] Parse BusinessCommand Error: Unknown exception";
    }
}

// ====================================================================
// 2. AppVideoProfile
// ====================================================================
inline void state_write(const ara::adsfi::AppVideoProfile &video_profile) {
    try {
        nlohmann::json j;
        j["timestamp"] = std::chrono::steady_clock::now().time_since_epoch().count();
        j["value"] = {{"manual_bps", video_profile.manual_bps},
                      {"manual_width", video_profile.manual_width},
                      {"manual_height", video_profile.manual_height},
                      {"remote_control_assist", video_profile.remote_control_assist},
                      {"remote_control_alarm", video_profile.remote_control_alarm},
                      {"auto_control_assist", video_profile.auto_control_assist},
                      {"video_third_person", video_profile.video_third_person}};

        nlohmann::json split_arr = nlohmann::json::array();
        for (const auto &split : video_profile.split_area_parames) {
            nlohmann::json s;
            s["channel_name"] = split.channel_name;
            s["view_area_points"] = {{"left_top_point_x", split.view_area_points.left_top_point_x},
                                     {"left_top_point_y", split.view_area_points.left_top_point_y},
                                     {"right_bottom_point_x", split.view_area_points.right_bottom_point_x},
                                     {"right_bottom_point_y", split.view_area_points.right_bottom_point_y}};
            s["crop_area_points"] = {{"left_top_point_x", split.crop_area_points.left_top_point_x},
                                     {"left_top_point_y", split.crop_area_points.left_top_point_y},
                                     {"right_bottom_point_x", split.crop_area_points.right_bottom_point_x},
                                     {"right_bottom_point_y", split.crop_area_points.right_bottom_point_y}};
            split_arr.push_back(s);
        }
        j["value"]["split_area_parames"] = split_arr;
        WriteJsonFile("video_profile.json", j);
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] State write AppVideoProfile failed: " << e.what();
    }
}

inline void state_read(ara::adsfi::AppVideoProfile &video_profile) {
    nlohmann::json j;
    if (!ReadJsonFile("video_profile.json", j))
        return;

    try {
        if (j.contains("timestamp")) {
            auto   timestamp_count = j["timestamp"].get<std::int64_t>();
            auto   now = std::chrono::steady_clock::now().time_since_epoch().count();
            double diff_seconds = static_cast<double>(std::llabs(now - timestamp_count)) / 1e9;

            if (diff_seconds > 10.0) {
                ApInfo("zmq_task") << "[Persistence] AppVideoProfile expired! Duration: " << diff_seconds
                                   << "s. Ignore.";
                return;
            }
            ApInfo("zmq_task") << "[Persistence] AppVideoProfile valid. Duration: " << diff_seconds << "s.";
        } else {
            ApInfo("zmq_task") << "[Persistence] AppVideoProfile missing timestamp. Ignore.";
            return;
        }

        if (j.contains("value")) {
            auto &v = j["value"];
            video_profile.manual_bps = v.value("manual_bps", 256);
            video_profile.manual_width = v.value("manual_width", 1920.0);
            video_profile.manual_height = v.value("manual_height", 1080.0);
            video_profile.remote_control_assist = v.value("remote_control_assist", 0);
            video_profile.remote_control_alarm = v.value("remote_control_alarm", 0);
            video_profile.auto_control_assist = v.value("auto_control_assist", 0);
            video_profile.video_third_person = v.value("video_third_person", 0);

            if (v.contains("split_area_parames") && v["split_area_parames"].is_array()) {
                auto & split_arr = v["split_area_parames"];
                size_t arr_size = split_arr.size();

                video_profile.split_area_parames.clear();
                video_profile.split_area_parames.reserve(arr_size);

                for (const auto &split_json : split_arr) {
                    ara::adsfi::AppVideoSplitParam split_param;
                    split_param.channel_name = split_json.value("channel_name", "");
                    if (split_json.contains("view_area_points")) {
                        auto &vp = split_json["view_area_points"];
                        split_param.view_area_points.left_top_point_x = vp.value("left_top_point_x", 0.0);
                        split_param.view_area_points.left_top_point_y = vp.value("left_top_point_y", 0.0);
                        split_param.view_area_points.right_bottom_point_x = vp.value("right_bottom_point_x", 0.0);
                        split_param.view_area_points.right_bottom_point_y = vp.value("right_bottom_point_y", 0.0);
                    }
                    if (split_json.contains("crop_area_points")) {
                        auto &cp = split_json["crop_area_points"];
                        split_param.crop_area_points.left_top_point_x = cp.value("left_top_point_x", 0.0);
                        split_param.crop_area_points.left_top_point_y = cp.value("left_top_point_y", 0.0);
                        split_param.crop_area_points.right_bottom_point_x = cp.value("right_bottom_point_x", 0.0);
                        split_param.crop_area_points.right_bottom_point_y = cp.value("right_bottom_point_y", 0.0);
                    }
                    video_profile.split_area_parames.push_back(split_param);
                }
            }
        }
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Parse VideoProfile Error: " << e.what();
    } catch (...) {
        ApError("zmq_task") << "[Persistence] Parse VideoProfile Error: Unknown exception";
    }
}

// ====================================================================
// 3. VideoRTP
// ====================================================================
inline void state_write(const ara::adsfi::VideoRTPParameter &video_rtp) {
    try {
        nlohmann::json j;
        j["timestamp"] = std::chrono::steady_clock::now().time_since_epoch().count();
        j["value"] = {{"ssrc", video_rtp.ssrc},
                      {"peer_host", video_rtp.peer_host},
                      {"peer_port", video_rtp.peer_port},
                      {"push_enabled", video_rtp.push_enabled}};
        WriteJsonFile("video_rtp.json", j);
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] State write VideoRTP failed: " << e.what();
    }
}

inline void state_read(ara::adsfi::VideoRTPParameter &video_rtp) {
    nlohmann::json j;
    if (!ReadJsonFile("video_rtp.json", j))
        return;

    try {
        if (j.contains("timestamp")) {
            auto   timestamp_count = j["timestamp"].get<std::int64_t>();
            auto   now = std::chrono::steady_clock::now().time_since_epoch().count();
            double diff_seconds = static_cast<double>(std::llabs(now - timestamp_count)) / 1e9;

            if (diff_seconds > 10.0) {
                ApInfo("zmq_task") << "[Persistence] VideoRTPParameter expired! Duration: " << diff_seconds
                                   << "s. Ignore.";
                return;
            }
            ApInfo("zmq_task") << "[Persistence] VideoRTPParameter valid. Duration: " << diff_seconds << "s.";
        } else {
            ApInfo("zmq_task") << "[Persistence] VideoRTPParameter missing timestamp. Ignore.";
            return;
        }

        if (j.contains("value")) {
            video_rtp.ssrc = j["value"].value("ssrc", 0);
            video_rtp.peer_host = j["value"].value("peer_host", "");
            video_rtp.peer_port = j["value"].value("peer_port", 0);
            video_rtp.push_enabled = j["value"].value("push_enabled", 0);
        }
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Parse VideoRTP Error: " << e.what();
    } catch (...) {
        ApError("zmq_task") << "[Persistence] Parse VideoRTP Error: Unknown exception";
    }
}

// ====================================================================
// 4. ReturnPointStatus
// ====================================================================
inline void state_write(const ReturnPointStatus &state) {
    try {
        nlohmann::json j;
        j["timestamp"] = std::chrono::steady_clock::now().time_since_epoch().count();
        j["value"] = {{"return_point_flag", state.return_point_flag}};
        WriteJsonFile("return_point_status.json", j);
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] State write ReturnPointStatus failed: " << e.what();
    }
}

inline void state_read(ReturnPointStatus &state) {
    nlohmann::json j;
    if (!ReadJsonFile("return_point_status.json", j)) {
        return;
    }

    try {
        if (j.contains("timestamp")) {
            auto   timestamp_count = j["timestamp"].get<std::int64_t>();
            auto   now = std::chrono::steady_clock::now().time_since_epoch().count();
            double diff_seconds = static_cast<double>(std::llabs(now - timestamp_count)) / 1e9;

            if (diff_seconds > 10.0) {
                ApInfo("zmq_task") << "[Persistence] ReturnPointStatus expired! Duration: " << diff_seconds
                                   << "s. Ignore.";
                return;
            }
            ApInfo("zmq_task") << "[Persistence] ReturnPointStatus valid. Duration: " << diff_seconds << "s.";
        } else {
            ApInfo("zmq_task") << "[Persistence] ReturnPointStatus missing timestamp. Ignore.";
            return;
        }

        if (j.contains("value")) {
            state.return_point_flag = j["value"].value("return_point_flag", false);
        }
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Parse ReturnPointStatus Error: " << e.what();
    } catch (...) {
        ApError("zmq_task") << "[Persistence] Parse ReturnPointStatus Error: Unknown exception";
    }
}

// ====================================================================
// 5. TaskManageStatus
// ====================================================================
inline void state_write(const ara::adsfi::TaskManageStatus &tm) {
    try {
        nlohmann::json j;
        j["timestamp"] = std::chrono::steady_clock::now().time_since_epoch().count();
        j["value"] = {{"action_id", static_cast<int>(tm.action_id)},
                      {"task_status", static_cast<int>(tm.task_status)},
                      {"task_type", static_cast<int>(tm.task_type)},
                      //   {"task_progress", static_cast<int>(tm.task_progress)},
                      {"distance_to_nearest", tm.distance_to_nearest}};
        //   {"progress", static_cast<int>(tm.progress)},
        //   {"total_mileage", tm.total_mileage},
        //   {"current_mileage", tm.current_mileage}};
        WriteJsonFile("task_manage_status.json", j);
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] State write TaskManageStatus failed: " << e.what();
    }
}

inline void state_read(ara::adsfi::TaskManageStatus &tm) {
    nlohmann::json j;
    if (!ReadJsonFile("task_manage_status.json", j))
        return;

    try {
        if (j.contains("timestamp")) {
            auto   timestamp_count = j["timestamp"].get<std::int64_t>();
            auto   now = std::chrono::steady_clock::now().time_since_epoch().count();
            double diff_seconds = static_cast<double>(std::llabs(now - timestamp_count)) / 1e9;

            if (diff_seconds > 10.0) {
                ApInfo("zmq_task") << "[Persistence] TaskManageStatus expired! Duration: " << diff_seconds
                                   << "s. Ignore.";
                return;
            }
            ApInfo("zmq_task") << "[Persistence] TaskManageStatus valid. Duration: " << diff_seconds << "s.";
        } else {
            ApInfo("zmq_task") << "[Persistence] TaskManageStatus missing timestamp. Ignore.";
            return;
        }

        if (j.contains("value")) {
            auto &v = j["value"];
            tm.action_id = static_cast<unsigned char>(v.value("action_id", 0));
            tm.task_status = static_cast<unsigned char>(v.value("task_status", 0));
            tm.task_type = static_cast<unsigned char>(v.value("task_type", 0));
            tm.distance_to_nearest = v.value("distance_to_nearest", 0.0);
        }
    } catch (const std::exception &e) {
        ApError("zmq_task") << "[Persistence] Parse TaskManageStatus Error: " << e.what();
    } catch (...) {
        ApError("zmq_task") << "[Persistence] Parse TaskManageStatus Error: Unknown exception";
    }
}