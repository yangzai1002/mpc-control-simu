#ifndef X_MAP_FILE_RECORDER_HPP
#define X_MAP_FILE_RECORDER_HPP

#include <glog/logging.h>
#include <tinyxml2.h>

#include <algorithm>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <vector>

#include "CommonStatus.hpp"
#include "Datetime14Util.hpp"
#include "Datetime19Util.hpp"
#include "GeoUtils.hpp"
#include "MapFileHandler.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_appstatusmessage.h"

namespace app_common {
    class MapRecorder {
    public:
        explicit MapRecorder(const std::string& base_dir)
            : _map_file_handler(base_dir)
            , _current_status(app_common::CommonStatus::IDLE)
            , _current_osm(nullptr)
            , _current_way(nullptr)
            , _current_node_id(0)
            , _current_way_id(0) {
        }

        MapRecorder(const MapRecorder&) = delete;

        void start(const std::string& file, const adsfi::LocationInfo& location) {
            if (_current_status != app_common::CommonStatus::IDLE) {
                AERROR << "路网采集进行中，请稍后进行";
                throw std::runtime_error("路网采集进行中，请稍后进行");
            }
            if (app_common::CommonStatus::ERROR == _current_status) {
                AERROR << "路网采集开始失败，请退出";
                throw std::runtime_error("路网采集失败，请退出");
            }
            if (file.empty()) {
                AERROR << "路网采集开始失败，路网名为空";
                throw std::runtime_error("路网采集失败，路网名为空");
            }
            // 取消地图采集时gps状态变化校验：28/11/2025 GY
            // if (!app_common::GeoUtils::is_location_ok(location)) {
            //     AERROR << "路网采集开始失败，定位状态异常";
            //     throw std::runtime_error("路网采集失败，定位状态异常");
            // }

            reset();

            _map_file_handler.map_file_load(file, _current_doc);

            _current_osm = _current_doc.FirstChildElement("osm");
            if (nullptr == _current_osm) {
                AERROR << "路网采集开始失败，文件格式异常";
                throw std::runtime_error("路网采集失败，文件格式异常");
                return;
            }

            auto last_node = _current_osm->LastChildElement("node");
            _current_node_id = 1;
            if (last_node != nullptr) {
                _current_node_id = last_node->IntAttribute("id") + 1;
            }

            auto last_way = _current_osm->LastChildElement("way");
            _current_way_id = 2;
            if (last_way != nullptr) {
                _current_way_id = last_way->IntAttribute("id") + 1;
            }

            _current_way = _current_doc.NewElement("way");
            if (nullptr == _current_way) {
                AERROR << "路网采集开始失败，不能创建新的路径";
                throw std::runtime_error("路网采集失败，不能创建新的路径");
            }
            _current_way->SetAttribute("id", _current_way_id);
            _current_way->SetAttribute("version", 1);

            _current_status = app_common::CommonStatus::RUNNING;
            _current_file = file;
            AINFO << "路网采集开始成功";
        }

        void pause() {
            if (app_common::CommonStatus::ERROR == _current_status) {
                throw std::runtime_error("路网采集错误，请退出");
            }
            if (app_common::CommonStatus::RUNNING == _current_status) {
                _current_status = app_common::CommonStatus::PAUSED;
            }
        }

        void resume() {
            if (app_common::CommonStatus::ERROR == _current_status) {
                throw std::runtime_error("路网采集错误，请退出");
            }
            if (app_common::CommonStatus::PAUSED == _current_status) {
                _current_status = app_common::CommonStatus::RUNNING;
            }
        }

        void abort() {
            reset();
        }

        // void finish(const adsfi::LocationInfo& location) {
        //     if (app_common::CommonStatus::ERROR == _current_status) {
        //         AERROR << "路网保存失败，任务状态异常";
        //         throw std::runtime_error("路网采集错误，请退出");
        //     }
        //     if (app_common::CommonStatus::RUNNING == _current_status ||
        //         app_common::CommonStatus::PAUSED == _current_status) {
        //         // if (!app_common::GeoUtils::is_location_ok(location)) {
        //         //     AERROR << "路网保存失败，定位状态异常";
        //         //     error("路网保存失败，定位状态异常");
        //         //     return;
        //         // }
        //         if (!_last_location.is_initialized()) {
        //             AERROR << "路网保存失败，新采集路径中位置点个数为 0";
        //             error("路网保存失败，新采集路径中位置点个数为 0");
        //             return;
        //         }

        //         //如果最后一个点错误，不进行保存
        //         // if (location.lat * 1e7 <= 90 * 1e7 && location.lon * 1e7 <= 180 * 1e7 &&
        //         //     location.lat * 1e7 >= -90 * 1e7 && location.lon * 1e7 >= -180 * 1e7 && location.lat != 0 &&
        //         //     location.lon != 0) {
        //         //     append(location);
        //         // }

        //         if (location.x * 1e7 <= 90 * 1e7 && location.y * 1e7 <= 180 * 1e7 && location.x * 1e7 >= -90 * 1e7 &&
        //             location.y * 1e7 >= -180 * 1e7 && location.y != 0 && location.x != 0) {
        //             append(location);
        //         }

        //         for (auto* node : _current_nodes) {
        //             _current_osm->InsertEndChild(node);
        //         }

        //         _current_osm->InsertEndChild(_current_way);
        //         try {
        //             _map_file_handler.map_file_content(_current_file, _current_doc);
        //             reset();
        //         } catch (const std::runtime_error& e) {
        //             error(e.what());
        //             AINFO << "路网保存失败，" << e.what();
        //             return;
        //         }
        //     } else {
        //         AERROR << "路网保存失败，status=" << (int)_current_status;
        //     }
        // }
        void finish(const adsfi::LocationInfo& location) {
            if (app_common::CommonStatus::ERROR == _current_status) {
                AERROR << "路网保存失败，任务状态异常";
                throw std::runtime_error("路网采集错误，请退出");
            }
            if (app_common::CommonStatus::RUNNING == _current_status ||
                app_common::CommonStatus::PAUSED == _current_status) {
                // 修改1：std::optional 的正确检查方式
                if (!_last_location.has_value()) { // 原来是 is_initialized()
                    AERROR << "路网保存失败，新采集路径中位置点个数为 0";
                    error("路网保存失败，新采集路径中位置点个数为 0");
                    return;
                }

                // 修改2：适配新结构体，手动检查位置是否有效
                const auto& pos = location.pose.pose.position;
                const bool  valid_location =
                    // 定位状态有效（locationState 通常 0=无效，非0=有效；如果项目约定不同，请改这里）
                    (location.locationState != 0U) &&
                    // 纬度范围：-90 ~ 90
                    (pos.y >= -90.0 && pos.y <= 90.0) &&
                    // 经度范围：-180 ~ 180
                    (pos.x >= -180.0 && pos.x <= 180.0) &&
                    // 排除无效原点 (0,0)
                    (pos.x != 0.0 || pos.y != 0.0);

                if (valid_location) {
                    append(location);
                } else {
                    // 位置无效时只记录日志，不保存（保持原逻辑不抛异常）
                    AERROR << "路网保存失败，当前位置无效（状态=" << location.locationState << ", lat=" << pos.y
                           << ", lon=" << pos.x << ")";
                    // 如果你想更严格，可以在这里 return;
                }

                for (auto* node : _current_nodes) {
                    _current_osm->InsertEndChild(node);
                }

                _current_osm->InsertEndChild(_current_way);
                try {
                    _map_file_handler.map_file_content(_current_file, _current_doc);
                    reset();
                } catch (const std::runtime_error& e) {
                    error(e.what());
                    AINFO << "路网保存失败，" << e.what();
                    return;
                }
            } else {
                AERROR << "路网保存失败，status=" << (int)_current_status;
            }
        }

        void append(const adsfi::LocationInfo& location) {
            if (app_common::CommonStatus::RUNNING == _current_status) {
                // if (!app_common::GeoUtils::is_location_ok(location)) {
                //     AERROR << "路网保存失败，定位状态异常";
                //     error("路网位置点保存失败，定位状态异常");
                //     return;
                // }

                if (!_last_location.has_value() ||
                    app_common::GeoUtils::gps_distance(location.pose.pose.position.x, location.pose.pose.position.y,
                                                       _last_location->pose.pose.position.x,
                                                       _last_location->pose.pose.position.y) >= 0.5) {
                    tinyxml2::XMLElement* node = nullptr;
                    tinyxml2::XMLElement* nd = nullptr;
                    if (nullptr == (node = _current_doc.NewElement("node")) ||
                        nullptr == (nd = _current_doc.NewElement("nd"))) {
                        AERROR << "路网保存失败，创建新的节点失败";
                        error("创建新的节点失败");
                        return;
                    }
                    AINFO << "路网保存，创建新的节点成功";
                    auto id = _current_node_id++;
                    node->SetAttribute("id", id);
                    node->SetAttribute("version", 1);
                    node->SetAttribute("lat", location.pose.pose.position.x);
                    node->SetAttribute("lon", location.pose.pose.position.y);
                    _current_nodes.push_back(node);
                    _current_osm->InsertEndChild(node);

                    nd->SetAttribute("ref", id);
                    _current_way->InsertEndChild(nd);

                    _last_location = location;
                }
            }
        }

        ara::adsfi::AppStatusMessage status() const {
            ara::adsfi::AppStatusMessage m{};

            m.status = app_common::common_status_str(_current_status);
            m.error_msg = _current_error;

            return m;
        }

    private:
        void error(const std::string& msg) {
            // _current_status = app_common::CommonStatus::ERROR;
            _current_error = msg;
        }

        void reset() {
            _current_status = app_common::CommonStatus::IDLE;
            _current_error.clear();
            _current_file.clear();
            _current_doc.Clear();
            _current_osm = nullptr;
            _current_way = nullptr;
            _current_nodes.clear();

            _last_location.reset();
            _current_node_id = 0;
            _current_way_id = 0;
        }

    private:
        app_common::MapFileHandler _map_file_handler;

        app_common::CommonStatus           _current_status;
        std::string                        _current_error;
        std::string                        _current_file;
        tinyxml2::XMLDocument              _current_doc;
        tinyxml2::XMLElement*              _current_osm;
        tinyxml2::XMLElement*              _current_way;
        std::vector<tinyxml2::XMLElement*> _current_nodes;

        std::optional<adsfi::LocationInfo> _last_location;
        std::uint32_t                      _current_node_id;
        std::uint32_t                      _current_way_id;
    };
} // namespace app_common
#endif
