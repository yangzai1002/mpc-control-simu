#ifndef X_TRACK_FILE_RECORDER_HPP
#define X_TRACK_FILE_RECORDER_HPP

#include <tinyxml2.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>

#include "CommonStatus.hpp"
#include "Datetime14Util.hpp"
#include "Datetime19Util.hpp"
#include "GeoUtils.hpp"
#include "GpxFileHandler.hpp"
#include "TrackRecorder.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_appstatusmessage.h"
#include "log/log.h"

namespace app_common {
    typedef struct {
        std::string lat;
        std::string lon;
    } _Pos;

    class TrackRecorder {
    public:
        explicit TrackRecorder(const std::string& base_dir)
            : _gpx_file_handler(base_dir, "default_track.txt")
            , _current_status(app_common::CommonStatus::IDLE)
            , _current_speed(0)
            , _current_length(0) {
        }

        TrackRecorder(const TrackRecorder&) = delete;

        ~TrackRecorder() {
            clean();
        }

        bool exist(const std::string& name) {
            return _gpx_file_handler.gpx_file_exist(name);
        }

        void del(const std::string& name) {
            return _gpx_file_handler.gpx_file_remove(name);
        }

        void start(const std::string& file, const std::string& start, const std::string& end, double speed,
                   const adsfi::LocationInfo& location) {
            if (file.empty() || start.empty() || end.empty() || start == end) {
                throw std::runtime_error("路径采集请求非法");
            }
            if (!app_common::GeoUtils::is_location_ok(location)) {
                throw std::runtime_error("路径采集失败，定位状态异常");
            }
            if (_current_status != app_common::CommonStatus::IDLE) {
                throw std::runtime_error("路径采集进行中，请稍后进行");
            }

            _gpx_file_handler.gpx_file_new(_current_doc, file, start, end);
            tinyxml2::XMLElement* gpxElement = _current_doc.FirstChildElement("gpx");
            if (gpxElement != nullptr) {
                tinyxml2::XMLElement* trkElement = gpxElement->FirstChildElement("trk");
                if (trkElement != nullptr) {
                    _current_trkseg = trkElement->FirstChildElement("trkseg");
                }
            }
            if (nullptr == _current_trkseg) {
                throw std::runtime_error("路径采集失败，文件格式错误");
            }
            _current_status = app_common::CommonStatus::RUNNING;
            _current_file = file;
            _current_speed = speed;
            _current_length = 0;
            _last_location = location;
        }

        void pause() {
            if (app_common::CommonStatus::RUNNING == _current_status) {
                _current_status = app_common::CommonStatus::PAUSED;
            }
        }

        void resume() {
            if (app_common::CommonStatus::PAUSED == _current_status) {
                _current_status = app_common::CommonStatus::RUNNING;
            }
        }

        void abort() {
            clean();
        }

        void finish(const adsfi::LocationInfo& location) {
            if (app_common::CommonStatus::RUNNING == _current_status ||
                app_common::CommonStatus::PAUSED == _current_status) {
                // if (!app_common::is_location_ok(location)) {
                //     throw std::runtime_error("路径保存失败，定位状态异常");
                // }

                // double distance =
                //     app_common::distance(_last_location->lat, _last_location->lon, location.lat, location.lon);
                // _current_length += distance;
                // append(location);

                // 保存地图长度
                tinyxml2::XMLElement* root = nullptr;
                tinyxml2::XMLElement* metadata = nullptr;
                if (nullptr == (root = _current_doc.RootElement()) ||
                    nullptr == (metadata = root->FirstChildElement("metadata"))) {
                    throw std::runtime_error("路径保存失败，文件格式错误");
                }

                // 路线长度低于3米不保存
                if (_current_length < 3.0) {
                    throw std::runtime_error("路径保存失败，距离低于 3 米");
                }

                auto save_file = _current_file;
                if (_gpx_file_handler.gpx_file_exist(_current_file)) {
                    save_file += app_common::Datetime14Util::current_yyyymmddhhmi();
                }
                try {
                    _gpx_file_handler.gpx_file_length(_current_doc, _current_length);
                    _gpx_file_handler.gpx_file_content(save_file, _current_doc);
                    clean();
                } catch (const std::runtime_error& e) {
                    throw e;
                } catch (...) {
                    throw std::runtime_error("路径文件保存失败");
                }
            }
        }

        void append(const adsfi::LocationInfo& location) {
            if (_current_status != app_common::CommonStatus::RUNNING) {
                return;
            }
            if (!app_common::GeoUtils::is_location_ok(location)) {
                AERROR << "路径保存失败，定位状态异常";
                return;
            }

            double distance = app_common::GeoUtils::gps_distance(
                location.pose.pose.position.x, location.pose.pose.position.y, _last_location.pose.pose.position.x,
                _last_location.pose.pose.position.y);
            AINFO << "append point, last: (" << _last_location.pose.pose.position.x << ", "
                  << _last_location.pose.pose.position.y << "), new: (" << location.pose.pose.position.x << ", "
                  << location.pose.pose.position.y << "), distance: " << distance;
            if (distance < 0.5) {
                AERROR << "append point, last: (" << _last_location.pose.pose.position.x << ", "
                       << _last_location.pose.pose.position.y << "), new: (" << location.pose.pose.position.x << ", "
                       << location.pose.pose.position.y << "), distance: " << distance
                       << ", less than 0.5m then ignored";
                return;
            }
            _last_location = location;
            _current_length += distance;

            auto trkpt = _current_doc.NewElement("trkpt");
            if (nullptr == trkpt || nullptr == _current_trkseg) {
                AERROR << "路径保存失败，追加路径点失败";
                return;
            }
            std::ostringstream lat_os;
            lat_os << std::fixed << std::setprecision(7) << location.pose.pose.position.x;
            std::ostringstream lon_os;
            lon_os << std::fixed << std::setprecision(7) << location.pose.pose.position.y;
            trkpt->SetAttribute("lat", lat_os.str().c_str());
            trkpt->SetAttribute("lon", lon_os.str().c_str());
            trkpt->SetAttribute("speed", _current_speed);
            trkpt->SetAttribute("dis_start", _current_length);
            trkpt->SetAttribute("fence_dis", 0);

            _Pos pos;
            pos.lat = lat_os.str();
            pos.lon = lon_os.str();
            _points.push_back(pos);

            _current_trkseg->InsertEndChild(trkpt);
        }

        void speed(double speed) {
            _current_speed = speed;
        }

        nlohmann::json returnPos(int num) {
            nlohmann::json pos_json;
            // if (num >= 0 && num < static_cast<int>(_points.size()))
            // {
            //     for (int cnt = 0; cnt < num; ++cnt)
            //     {
            //         _Pos pos = _points[cnt];
            //         nlohmann::json pos_item = {
            //             {"lon", pos.lon},
            //             {"lat", pos.lat}
            //         };
            //         pos_json.push_back(pos_item);
            //     }
            //     return pos_json;
            // }
            // for (int cnt = _points.size() -1 -num; cnt < _points.size(); cnt++)
            // {
            //     _Pos pos = _points[cnt];
            //     nlohmann::json pos_item = {
            //         {"lon", pos.lon},
            //         {"lat", pos.lat}
            //     };
            //     pos_json.push_back(pos_item);
            // }

            for (int cnt = 0; cnt < _points.size(); cnt++) {
                _Pos           pos = _points[cnt];
                nlohmann::json pos_item = {{"lon", pos.lon}, {"lat", pos.lat}};
                pos_json.push_back(pos_item);
            }

            return pos_json;
        }

        ara::adsfi::AppStatusMessage status() const {
            ara::adsfi::AppStatusMessage m{};
            m.status = app_common::common_status_str(_current_status);
            m.error_msg = _current_error;
            return m;
        }

    private:
        void clean() {
            _current_status = app_common::CommonStatus::IDLE;
            _current_error.clear();
            _current_file.clear();
            _current_speed = 0;
            _current_length = 0;
            _current_doc.Clear();
            _current_trkseg = nullptr;
            _last_location = {};
            _points.clear();
        }

    private:
        app_common::GpxFileHandler _gpx_file_handler;

        app_common::CommonStatus _current_status;
        std::string              _current_error;
        std::string              _current_file;
        double                   _current_speed;
        double                   _current_length;
        tinyxml2::XMLDocument    _current_doc;
        tinyxml2::XMLElement*    _current_trkseg;
        adsfi::LocationInfo      _last_location;
        std::vector<_Pos>        _points;
    };
} // namespace app_common
#endif
