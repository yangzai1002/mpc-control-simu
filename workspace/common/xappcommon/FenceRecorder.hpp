#ifndef FENCE_RECORDER_HPP
#define FENCE_RECORDER_HPP

#include "CommonStatus.hpp"
#include "GpxFileHandler.hpp"
#include "JsonUtils.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "ara/adsfi/impl_type_appstatusmessage.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "log/log.h"
#include "tinyxml2.h"

namespace app_common {
    class FenceRecorder {
    public:
        explicit FenceRecorder(const std::string &base_dir)
            : _gpx_file_handler(base_dir, "default_fence.txt")
            , _status(app_common::CommonStatus::IDLE)
            , _seg(nullptr)
            , _last_lat(0)
            , _last_lon(0) {
        }

        FenceRecorder(const FenceRecorder &) = delete;

        void Start(const std::string &file, const std::string &section, double offset) noexcept(false) {
            if (file.empty()) {
                throw std::runtime_error("文件名不能为空");
            }
            if (section.empty()) {
                throw std::runtime_error("分段名不能为空");
            }

            if (app_common::CommonStatus::IDLE != _status) {
                throw std::runtime_error("文件 " + file + " 忙，请稍后");
            }
            if (!_gpx_file_handler.gpx_file_exist(file)) {
                _gpx_file_handler.gpx_file_new(file, "", "");
            }

            _doc.Clear();
            auto content = _gpx_file_handler.gpx_file_content(file);
            if (tinyxml2::XML_SUCCESS != _doc.Parse(content.c_str())) {
                throw std::runtime_error("文件 " + file + " 解析失败");
            }

            tinyxml2::XMLElement *gpx = nullptr;
            tinyxml2::XMLElement *trk = nullptr;
            if (nullptr == (gpx = _doc.FirstChildElement("gpx")) || nullptr == (trk = gpx->FirstChildElement("trk"))) {
                throw std::runtime_error("文件 " + file + " 格式错误");
            }

            auto trkseg = trk->FirstChildElement("trkseg");
            while (trkseg != nullptr) {
                auto next = trkseg->NextSiblingElement();
                auto att = trkseg->Attribute("name");
                if (nullptr == att || att == section) {
                    trk->DeleteChild(trkseg);
                }
                trkseg = next;
            }

            _seg = _doc.NewElement("trkseg");
            if (nullptr == _seg) {
                throw std::runtime_error("文件 " + file + " 新增分段失败");
            }
            _seg->SetAttribute("name", section.c_str());
            _seg->SetAttribute("offset", offset);
            trk->InsertEndChild(_seg);

            // 状态变更
            _status = app_common::CommonStatus::RUNNING;
            _error_msg.clear();
            _file = file;
            _section = section;
            _offset = offset;
            _last_lat = _last_lon = 0;
        }

        void Append(const adsfi::LocationInfo &location) {
            if (app_common::CommonStatus::RUNNING != _status) {
                return;
            }
            if (!app_common::GeoUtils::is_location_ok(location)) {
                AERROR << "invalid location: "; // Error("定位异常");
                return;
            }

            auto distance = app_common::GeoUtils::gps_distance(_last_lat, _last_lon, location.pose.pose.position.x,
                                                               location.pose.pose.position.y);
            if (distance < 0.5) {
                AERROR << fmt::format("distance between last({:.8f},{:.8f}) and this({:.8f},{:.8f}) is {:.8f}",
                                      _last_lat, _last_lon, location.pose.pose.position.x,
                                      location.pose.pose.position.y, distance);
                return;
            }
            _last_lat = location.pose.pose.position.x;
            _last_lon = location.pose.pose.position.y;

            if (nullptr == _seg) {
                AERROR << "current segment is null";
                Error("分段异常");
                return;
            }

            auto left_heading = location.pose.pose.orientation.z - 90;
            if (left_heading < 0) {
                left_heading += 360;
            }
            auto offset_loc = app_common::GeoUtils::location_target(location, left_heading, _offset);
            auto trkpt = _doc.NewElement("trkpt");
            if (nullptr == trkpt) {
                AERROR << "cannot create trkpt";
                Error("创建记录点异常");
                return;
            }
            trkpt->SetAttribute("lat", offset_loc.pose.pose.position.x);
            trkpt->SetAttribute("lon", offset_loc.pose.pose.position.y);
            _seg->InsertEndChild(trkpt);
        }

        void Pause() {
            if (app_common::CommonStatus::ERROR == _status) {
                throw std::runtime_error("录制错误，请退出");
            }
            if (app_common::CommonStatus::RUNNING == _status) {
                _status = app_common::CommonStatus::PAUSED;
            }
        }

        void Continue() {
            if (app_common::CommonStatus::ERROR == _status) {
                throw std::runtime_error("录制错误，请退出");
            }

            if (app_common::CommonStatus::PAUSED == _status) {
                _status = app_common::CommonStatus::RUNNING;
            }
        }

        void Finish() {
            if (app_common::CommonStatus::ERROR == _status) {
                throw std::runtime_error("录制错误，请退出");
            }
            if (app_common::CommonStatus::IDLE == _status) {
                throw std::runtime_error("当前不在录制状态");
            }

            _gpx_file_handler.gpx_file_content(_file, _doc);

            Abort();
        }

        void Abort() {
            _status = app_common::CommonStatus::IDLE;
            _error_msg.clear();
            _file.clear();
            _section.clear();
            _offset = 0;
            _doc.Clear();
            _seg = nullptr;
            _last_lat = 0;
            _last_lon = 0;
        }

        void Delete(const std::string &file, const std::string &section) {
            if (_gpx_file_handler.gpx_file_using(file)) {
                throw std::runtime_error("文件 " + file + " 正在使用，不能修改");
            }
            if (app_common::CommonStatus::IDLE != _status && file == _file) {
                throw std::runtime_error("文件 " + file + " 正在录制，不能修改");
            }

            _gpx_file_handler.gpx_file_delete_segment(file, section);
        }

        ara::adsfi::AppStatusMessage status() {
            ara::adsfi::AppStatusMessage m{};
            m.status = app_common::common_status_str(_status);
            m.error_msg = _error_msg;
            return m;
        }

    private:
        void Error(const std::string &msg) {
            _status = app_common::CommonStatus::ERROR;
            _error_msg = msg;
        }

    private:
        GpxFileHandler _gpx_file_handler;

        CommonStatus _status;
        std::string  _error_msg;

        std::string           _file;
        std::string           _section;
        double                _offset;
        tinyxml2::XMLDocument _doc;
        tinyxml2::XMLElement *_seg;

        double _last_lat;
        double _last_lon;
    };
} // namespace app_common
#endif // X_TRACK_RECORD_CONTROLLER_HPP
