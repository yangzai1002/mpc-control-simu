#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "ap_log/ap_log_interface.h"

// 地形图参数配置（启动初始化阶段加载，运行期只读缓存，避免每帧 IO）
struct TerrainMapConfig {
    // 图像参数
    int width = 500;
    int height = 500;
    int channel = 3;
    // 分辨率（cm/pixel）：20 => 0.2m/pixel
    int resolution_cm_per_pixel = 20;
    // 障碍物通道索引（0-based），默认 2 表示第三通道
    int obstacle_channel_index = 2;

    // 语义地图对齐时间窗口（秒）
    double semantic_time_tolerance_sec = 1.0;

    // 动态目标类型映射（Perception 内部类型 -> 障碍物通道像素值）
    // 默认与现有逻辑保持一致：人=3，车=4，正障碍=1，其它=0
    std::vector<uint8_t> dynamic_type_map = {3, 3, 4, 4, 4, 3, 4, 4, 3, 3, 1, 1, 1, 0};

    // 语义障碍物类型号映射（semantic_map.sem.type -> 正/负障碍）
    std::unordered_set<int> semantic_positive_types = {13, 40, 255};
    std::unordered_set<int> semantic_negative_types = {16};

    // 像素值保护表：这些类型一旦画上，后续不会被覆盖（默认：人(3)、车(4)）
    std::array<uint8_t, 256> protect_type_mask{};

    TerrainMapConfig() {
        protect_type_mask.fill(0);
        protect_type_mask[3] = 1;
        protect_type_mask[4] = 1;
    }
};

// 从项目配置加载地形图参数到配置结构体（返回加载后的配置）。
// 说明：
// - 缺少任一配置项时使用默认值（保持向后兼容）
// - 建议只在初始化阶段调用一次，运行期不做任何文件 IO
template <typename CustomStackPtr>
inline TerrainMapConfig load_terrain_map_config_from_config(const CustomStackPtr& ptr, const std::string& prefix,
                                                            const TerrainMapConfig& default_cfg = TerrainMapConfig{}) {
    if (!ptr) {
        ApError("zmq_notification") << "CustomStack 实例为空，无法加载地形图配置，将使用默认值";
        return default_cfg;
    }

    TerrainMapConfig  cfg = default_cfg;
    const std::string base = prefix + "hmi/zmq/terrain_map/";
    bool              has_any_config = false;

    auto read_int = [&](const std::string& key, int& out) {
        int tmp = out;
        if (ptr->GetProjectConfigValue(base + key, tmp)) {
            out = tmp;
            has_any_config = true;
        }
    };
    auto read_double = [&](const std::string& key, double& out) {
        double tmp = out;
        if (ptr->GetProjectConfigValue(base + key, tmp)) {
            out = tmp;
            has_any_config = true;
        }
    };

    read_int("width", cfg.width);
    read_int("height", cfg.height);
    read_int("channel", cfg.channel);
    read_int("resolution", cfg.resolution_cm_per_pixel);
    read_int("obstacle_channel_index", cfg.obstacle_channel_index);
    read_double("semantic_time_tolerance_sec", cfg.semantic_time_tolerance_sec);

    // dynamic_type_map
    {
        std::vector<int>  tmp;
        const std::string full_path = base + "dynamic_type_map";
        const bool        ok_read = ptr->GetProjectConfigArray(full_path, tmp);
        if (ok_read) {
            has_any_config = true;
        } else {
            ApError("zmq_notification") << "未配置 " << full_path << "，将使用默认 dynamic_type_map";
        }
        if (ok_read && !tmp.empty()) {
            std::vector<uint8_t> map;
            map.reserve(tmp.size());
            bool ok = true;
            for (const auto& v : tmp) {
                if (v < 0 || v > 255) {
                    ok = false;
                    ApError("zmq_notification")
                        << "地形图配置非法: " << base << "dynamic_type_map 存在越界值 " << v << "（期望 0~255）";
                    break;
                }
                map.push_back(static_cast<uint8_t>(v));
            }
            if (ok) {
                cfg.dynamic_type_map = std::move(map);
            }
        }
    }

    // semantic_positive_types / semantic_negative_types
    {
        std::vector<int> pos;
        if (ptr->GetProjectConfigArray(base + "semantic_positive_types", pos)) {
            has_any_config = true;
            if (!pos.empty()) {
                cfg.semantic_positive_types.clear();
                for (const auto& t : pos) {
                    cfg.semantic_positive_types.insert(t);
                }
            }
        }
        std::vector<int> neg;
        if (ptr->GetProjectConfigArray(base + "semantic_negative_types", neg)) {
            has_any_config = true;
            if (!neg.empty()) {
                cfg.semantic_negative_types.clear();
                for (const auto& t : neg) {
                    cfg.semantic_negative_types.insert(t);
                }
            }
        }
    }

    // protect_types
    {
        std::vector<int> protect;
        if (ptr->GetProjectConfigArray(base + "protect_types", protect)) {
            has_any_config = true;
            if (!protect.empty()) {
                cfg.protect_type_mask.fill(0);
                for (const auto& v : protect) {
                    if (v < 0 || v > 255) {
                        ApError("zmq_notification")
                            << "地形图配置非法: " << base << "protect_types 存在越界值 " << v << "（期望 0~255）";
                        continue;
                    }
                    cfg.protect_type_mask[static_cast<std::size_t>(v)] = 1;
                }
            }
        }
    }

    if (!has_any_config) {
        ApError("zmq_notification") << "未读取到任何地形图配置项: " << base << "（例如: " << base << "width / " << base
                                    << "height / " << base << "dynamic_type_map），将使用默认值";
    }

    // 参数校验（异常值回退到默认，避免运行期分配超大内存或协议字段非法）
    static constexpr int kMaxTerrainMapSide = 2000;
    if (cfg.width <= 0 || cfg.width > kMaxTerrainMapSide || cfg.height <= 0 || cfg.height > kMaxTerrainMapSide) {
        ApError("zmq_notification") << "地形图配置非法: width=" << cfg.width << ", height=" << cfg.height
                                    << "，回退到默认 500x500";
        cfg.width = 500;
        cfg.height = 500;
    }
    if (cfg.channel <= 0 || cfg.channel > 16) {
        ApError("zmq_notification") << "地形图配置非法: channel=" << cfg.channel << "，回退到默认 3";
        cfg.channel = 3;
    }
    if (cfg.resolution_cm_per_pixel <= 0 || cfg.resolution_cm_per_pixel > 1000) {
        ApError("zmq_notification") << "地形图配置非法: resolution=" << cfg.resolution_cm_per_pixel
                                    << "，回退到默认 20";
        cfg.resolution_cm_per_pixel = 20;
    }
    if (cfg.semantic_time_tolerance_sec <= 0.0 || cfg.semantic_time_tolerance_sec > 10.0) {
        ApError("zmq_notification") << "地形图配置非法: semantic_time_tolerance_sec=" << cfg.semantic_time_tolerance_sec
                                    << "，回退到默认 1.0";
        cfg.semantic_time_tolerance_sec = 1.0;
    }
    if (cfg.obstacle_channel_index < 0 || cfg.obstacle_channel_index >= cfg.channel) {
        const int fallback = std::min(2, cfg.channel - 1);
        ApError("zmq_notification") << "地形图配置非法: obstacle_channel_index=" << cfg.obstacle_channel_index
                                    << "（channel=" << cfg.channel << "），回退到默认 " << fallback;
        cfg.obstacle_channel_index = fallback;
    }

    ApError("zmq_notification") << "地形图配置加载完成: width=" << cfg.width << ", height=" << cfg.height
                                << ", channel=" << cfg.channel << ", resolution=" << cfg.resolution_cm_per_pixel
                                << ", obstacle_channel_index=" << cfg.obstacle_channel_index
                                << ", semantic_time_tolerance_sec=" << cfg.semantic_time_tolerance_sec
                                << ", dynamic_type_map_size=" << cfg.dynamic_type_map.size()
                                << ", semantic_positive_types_size=" << cfg.semantic_positive_types.size()
                                << ", semantic_negative_types_size=" << cfg.semantic_negative_types.size();

    return cfg;
}
