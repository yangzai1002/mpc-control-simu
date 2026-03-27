#ifndef XTASK_POINT_MANAGER_HPP
#define XTASK_POINT_MANAGER_HPP

#include <glog/logging.h>

#include <filesystem>
#include <memory>
#include <mutex>

#include "BusinessTaskPoints.h"
#include "GeoUtils.hpp"
#include "JsonUtils.hpp"
#include "TaskPoint.hpp"
#include "TaskPointFileTransmit.hpp"
#include "adsfi/impl_type_locationinfo.h"
#include "custom_stack.h"
#include "log/log.h"
#include "tinyxml2.h"

namespace app_common {
    class TaskPointFileHandler {
    public:
        TaskPointFileHandler()
            : filetransmit("taskpoint", ".gpx")
            , kDefaultTaskPointFilePath("default_taskpoints.txt")
            , kGlobalTaskPointsFilePath(
                  CustomStack::Instance()->GetProjectConfigValue<std::string>("keyfile_storage_path") + "taskpoint") {
            // load default taskpoints
            std::error_code ec;
            std::filesystem::create_directories(filetransmit.GetFilePath(), ec);
            {
                std::ifstream is(kDefaultTaskPointFilePath);
                std::string   taskpointsfile;
                std::getline(is, taskpointsfile);
                if (!taskpointsfile.empty()) {
                    kTaskPointsFilePath = filetransmit.GetFileName(taskpointsfile);
                    filetransmit.SetCurFile(taskpointsfile);
                    UpdateTaskPoint();
                }
            }
            if (filetransmit.GetCurFile().empty()) {
                LOG(WARNING) << "the default taskpoints is empty.";
            } else {
                LOG(INFO) << "the default taskpoints: " << filetransmit.GetCurFile();
            }
        }

        void FileList(const nlohmann::json& req, nlohmann::json& res) {
            filetransmit.ListCb(req, res);
        }

        void FileDownload(const nlohmann::json& req, nlohmann::json& res) {
            filetransmit.DownloadCb(req, res);
        }

        void FileUpload(const nlohmann::json& req, nlohmann::json& res) {
            filetransmit.UploadCb(req, res);
        }

        void FileDelete(const nlohmann::json& req, nlohmann::json& res) {
            filetransmit.DeleteCb(req, res);
        }

        void FileCreate(const nlohmann::json& req, nlohmann::json& res) {
            std::string name;
            app_common::JsonUtils::valueFromJson(name, req, "name", true);

            LOG(INFO) << "service: task_point/file/create: " << name;

            res["data"] = nlohmann::json::object({});

            if (std::filesystem::exists(filetransmit.GetFileName(name))) {
                throw std::runtime_error("已存在该名称的任务点文件");
            }

            if (!CreateFile(filetransmit.GetFileName(name))) {
                throw std::runtime_error("创建任务点文件失败");
            }
        }

        void FileGetDefault(const nlohmann::json& req, nlohmann::json& res) {
            std::string   res_filename;
            std::ifstream is(kDefaultTaskPointFilePath);
            std::getline(is, res_filename);

            std::string           filename = filetransmit.GetFileName(res_filename);
            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(filename.c_str())) {
                throw std::runtime_error("默认任务点文件未设置");
            }

            res["data"] = nlohmann::json::object({{"file_name", res_filename}});
        }

        void FileSetDefault(const nlohmann::json& req, nlohmann::json& res) {
            std::string file_name;
            app_common::JsonUtils::valueFromJson(file_name, req, "file_name", true);

            res["data"] = nlohmann::json::object({});

            if (!std::filesystem::exists(filetransmit.GetFileName(file_name))) {
                throw std::runtime_error("不存在该名称的任务点文件");
            }

            std::lock_guard<std::mutex> lck(mutex);
            std::ofstream               os(kDefaultTaskPointFilePath);
            os << file_name << std::endl;
            kTaskPointsFilePath = filetransmit.GetFileName(file_name);
            filetransmit.SetCurFile(file_name);
            UpdateTaskPoint();
        }

        void List(const nlohmann::json& req, nlohmann::json& res) {
            std::string type;
            app_common::JsonUtils::valueFromJson(type, req, "type", false, "");

            LOG(INFO) << "List: " << kTaskPointsFilePath;
            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(kTaskPointsFilePath.c_str())) {
                throw std::runtime_error("未设置默认任务点文件");
            }

            TaskPoints points;
            auto       gpx = doc.FirstChildElement("gpx");
            auto       wpt = gpx->FirstChildElement("wpt");
            while (wpt != nullptr) {
                TaskPoint p(wpt);

                // 满足类型就push进去，ALL就是全进去
                std::uint32_t mask = 0;
                if (type == TaskPoint::PointTypeString::TASK_POINT) {
                    mask = (int)TaskPoint::PointType::TASK_POINT;
                } else if (type == TaskPoint::PointTypeString::NARROW_IN) {
                    mask = (int)TaskPoint::PointType::NARROW_IN;
                } else if (type == TaskPoint::PointTypeString::NARROW_OUT) {
                    mask = (int)TaskPoint::PointType::NARROW_OUT;
                } else if (type == TaskPoint::PointTypeString::SPECIAL_TAG) {
                    mask = (int)TaskPoint::PointType::SPECIAL_TAG;
                } else if (type == TaskPoint::PointTypeString::POSTURE_POINT) {
                    mask = (int)TaskPoint::PointType::POSTURE_POINT;
                } else if (type == TaskPoint::PointTypeString::TRAFFIC_LIGHT_POINT) {
                    mask = (int)TaskPoint::PointType::TRAFFIC_LIGHT_POINT;
                } else if (type == TaskPoint::PointTypeString::PROPERTY_POINT) {
                    mask = (int)TaskPoint::PointType::PROPERTY_POINT;
                } else {
                    points.push_back(p);
                }
                if ((std::uint32_t)p.type & mask) {
                    points.push_back(p);
                }

                wpt = wpt->NextSiblingElement();
            }

            res["data"] = nlohmann::json::object({{"points", nlohmann::json::array()}});

            for (auto& p : points) {
                res["data"]["points"].push_back(nlohmann::json({{"id", p.id},
                                                                {"name", p.name},
                                                                {"high", p.high},
                                                                {"pitch", p.pitch},
                                                                {"yaw", p.yaw},
                                                                {"zoom", p.zoom},
                                                                {"lat", p.lat},
                                                                {"lon", p.lon},
                                                                {"type", TaskPoint::TypeIntToStringVec(p.type)},
                                                                {"startend_set", p.startend_set},
                                                                {"heading", p.heading},
                                                                {"speed", p.speed},
                                                                {"avoid_status", p.avoid_status},
                                                                {"perception_status", p.perception_status},
                                                                {"car_type", p.car_type},
                                                                {"task_type", p.task_type},
                                                                {"task_area", p.task_area},
                                                                {"is_stop", p.is_stop},
                                                                {"task_time", p.task_time},
                                                                {"end_point", p.end_point},
                                                                {"travel_mode", p.travel_mode}}));
            }
        }

        void Update(const adsfi::LocationInfo& location, const nlohmann::json& req, nlohmann::json& res) {
            std::int32_t req_id;
            std::string  req_name;
            bool         req_update;
            std::int8_t  req_startend_set;
            app_common::JsonUtils::valueFromJson(req_id, req, "id", true);
            app_common::JsonUtils::valueFromJson(req_name, req, "name", true);
            app_common::JsonUtils::valueFromJson(req_update, req, "update", true);
            app_common::JsonUtils::valueFromJson(req_startend_set, req, "startend_set", true);

            res["data"] = nlohmann::json::object({});

            if (req_name == "") {
                throw std::runtime_error("名称不能为空");
            }
            if (!app_common::GeoUtils::is_location_ok(location)) {
                throw std::runtime_error("定位状态异常");
            }

            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(kTaskPointsFilePath.c_str())) {
                throw std::runtime_error("未设置默认任务点文件");
            }
            auto gpx = doc.FirstChildElement("gpx");
            auto wpt = gpx->FirstChildElement("wpt");
            while (wpt != nullptr) {
                int id = wpt->IntAttribute("id");

                if (id != req_id) {
                    wpt = wpt->NextSiblingElement();
                    continue;
                }
                wpt->FirstChildElement("name")->SetText(req_name.c_str());

                auto startend_set = wpt->FirstChildElement("startend_set");
                if (startend_set == nullptr) {
                    auto startend_set_add = doc.NewElement("startend_set");
                    startend_set_add->SetText(req_startend_set);
                    wpt->InsertEndChild(startend_set_add);
                } else {
                    startend_set->SetText(req_startend_set);
                }

                if (req_update) {
                    // TODO: AA 移植注释掉了 -- start
                    // auto ptz_status = PayloadManager::Instance().status();
                    // TODO: AA 移植注释掉了 -- end
                    wpt->SetAttribute("lat", location.pose.pose.position.x);
                    wpt->SetAttribute("lon", location.pose.pose.position.y);
                    wpt->SetAttribute("heading", location.pose.pose.orientation.z);
                    // TODO: AA 移植注释掉了 -- start
                    // wpt->FirstChildElement("high")->SetText(ptz_status.rod_high);
                    // wpt->FirstChildElement("pitch")->SetText(ptz_status.pitch);
                    // wpt->FirstChildElement("yaw")->SetText(ptz_status.yaw);
                    // wpt->FirstChildElement("zoom")->SetText(ptz_status.zoom);
                    // TODO: AA 移植注释掉了 -- end
                }
                tinyxml2::XMLError err = doc.SaveFile(kTaskPointsFilePath.c_str());
                if (err == tinyxml2::XML_SUCCESS) {
                    UpdateTaskPoint();
                    // TODO: AA 移植注释掉了 -- start
                    // PayloadManager::Instance().Reset();
                    // TODO: AA 移植注释掉了 --end
                } else {
                    throw std::runtime_error("保存任务点失败");
                }
                return;
            }
            throw std::runtime_error("未找到对应的任务点");
        }

        void Delete(const nlohmann::json& req, nlohmann::json& res) {
            std::int32_t req_id;
            app_common::JsonUtils::valueFromJson(req_id, req, "id", true);

            res["data"] = nlohmann::json::object({});

            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(kTaskPointsFilePath.c_str())) {
                throw std::runtime_error("未设置默认任务点文件");
            }
            auto gpx = doc.FirstChildElement("gpx");
            auto wpt = gpx->FirstChildElement("wpt");
            while (wpt != nullptr) {
                int id = wpt->IntAttribute("id");

                if (id == req_id) {
                    gpx->DeleteChild(wpt);
                    tinyxml2::XMLError err = doc.SaveFile(kTaskPointsFilePath.c_str());
                    if (err == tinyxml2::XML_SUCCESS) {
                        UpdateTaskPoint();
                    } else {
                        throw std::runtime_error("保存任务点失败");
                    }
                    return;
                }
                wpt = wpt->NextSiblingElement("wpt");
            }
            throw std::runtime_error("未找到对应的任务点");
        }

        void Save(const adsfi::LocationInfo& location, const nlohmann::json& req, nlohmann::json& res) {
            std::string              req_name;
            std::vector<std::string> req_types;
            std::int8_t              req_startend_set;
            app_common::JsonUtils::valueFromJson(req_name, req, "name", true);
            for (auto& item : req["type"]) {
                req_types.push_back(item.get<std::string>());
            }
            app_common::JsonUtils::valueFromJson(req_startend_set, req, "startend_set", true);

            res["data"] = nlohmann::json::object({});

            if (req_name == "") {
                throw std::runtime_error("名称不能为空");
            }
            if (!app_common::GeoUtils::is_location_ok(location)) {
                throw std::runtime_error("定位状态异常");
            }

            if (kTaskPointsFilePath.empty()) {
                throw std::runtime_error("未设置默认任务点文件");
            }

            // TODO: AA 移植注释掉了 -- start
            // auto ptz_status = PayloadManager::Instance().status();
            // TODO: AA 移植注释掉了 -- end
            tinyxml2::XMLDocument doc;
            auto                  ret = doc.LoadFile(kTaskPointsFilePath.c_str());
            if (tinyxml2::XML_SUCCESS != ret) {
                LOG(WARNING) << "TaskPoint: SaveCb: default taskpointsfile wrong";
                throw std::runtime_error("默认任务点文件不存在，请创建或指定");
            }

            tinyxml2::XMLElement* gpx;
            tinyxml2::XMLElement* metadata;
            tinyxml2::XMLElement* max_id;
            if ((nullptr == (gpx = doc.FirstChildElement("gpx"))) ||
                (nullptr == (metadata = gpx->FirstChildElement("metadata"))) ||
                (nullptr == (max_id = metadata->FirstChildElement("max_id")))) {
                throw std::runtime_error("默认任务点文件格式不正确");
            }
            int new_id = max_id->IntText() + 1;
            max_id->SetText(new_id);

            LOG(INFO) << "req_type = " << req_types[0];
            auto type = TaskPoint::TypeStringVecToInt(req_types);
            if ((std::uint32_t)type == 0) {
                LOG(WARNING) << "TaskPoint: SaveCb: req_type is undefined";
                throw std::runtime_error("任务点类型未知");
            }

            // 遍历查询name是否重复，重复则删掉之前的wpt
            auto fgpx = doc.FirstChildElement("gpx");
            if (nullptr == fgpx) {
                LOG(ERROR) << "file format error: " << kTaskPointsFilePath;
                throw std::runtime_error("文件格式错误");
            }

            auto fwpt = fgpx->FirstChildElement("wpt");
            while (fwpt != nullptr) {
                auto fname = fwpt->FirstChildElement("name");
                if (fname != nullptr) {
                    if (req_name == fname->GetText()) {
                        LOG(WARNING) << "TaskPoint: SaveCb: the same name , delete last";
                        fgpx->DeleteChild(fwpt);
                        break;
                    }
                    fwpt = fwpt->NextSiblingElement();
                } else {
                    break;
                }
            }

            TaskPoint tp;
            tp.name = req_name;
            tp.id = new_id;
            tp.type = type;
            tp.lat = location.pose.pose.position.x;
            tp.lon = location.pose.pose.position.y;
            tp.heading = location.pose.pose.orientation.z;
            auto wpt = tp.to_xml_element(doc, new_id);
            if (nullptr == wpt) {
                throw std::runtime_error("增加任务点失败");
            }
            gpx->InsertEndChild(wpt);
            if (doc.SaveFile(kTaskPointsFilePath.c_str()) != tinyxml2::XML_SUCCESS) {
                throw std::runtime_error("保存任务点文件失败");
            }
            LOG(INFO) << "saved: " << kTaskPointsFilePath;

            UpdateTaskPoint();
            // TODO: AA 移植注释掉了 -- start
            // PayloadManager::Instance().Reset();
            // TODO: AA 移植注释掉了 -- end
            return;
        }

        void GlobalExport(const nlohmann::json& req, nlohmann::json& res) {
            std::string filename;
            app_common::JsonUtils::valueFromJson(filename, req, "name", true);
            if (!CreateFile(filetransmit.GetFileName(filename))) {
                throw std::runtime_error("创建任务点文件失败");
            }
            app_common::JsonUtils::field_should_be_array(req, "waypoints");

            res["data"] = nlohmann::json::object({});

            auto& items = req["waypoints"];
            for (auto item : items) {
                TaskPoint tp(item);
                if (tp.name == "") {
                    throw std::runtime_error("名称不能为空");
                }

                LOG(INFO) << "GlobalExport_path_item:" << tp.name;
                LOG(INFO) << "GlobalExport_path_item_task_type:" << tp.task_type;

                tinyxml2::XMLDocument doc;
                auto                  ret = doc.LoadFile(filetransmit.GetFileName(filename).c_str());
                if (tinyxml2::XML_SUCCESS != ret) {
                    throw std::runtime_error("默认任务点文件不存在，请创建或指定");
                }

                tinyxml2::XMLElement* gpx = nullptr;
                tinyxml2::XMLElement* metadata = nullptr;
                tinyxml2::XMLElement* max_id = nullptr;

                if ((nullptr == (gpx = doc.FirstChildElement("gpx"))) ||
                    (nullptr == (metadata = gpx->FirstChildElement("metadata"))) ||
                    (nullptr == (max_id = metadata->FirstChildElement("max_id")))) {
                    throw std::runtime_error("默认任务点文件格式错误");
                }
                auto new_id = max_id->IntText() + 1;
                max_id->SetText(new_id);

                // TODO: AA 移植注释掉了 -- start
                // auto ptz_status = PayloadManager::Instance().status();
                // tp.high = ptz_status.rod_high;
                // tp.pitch = ptz_status.pitch;
                // tp.yaw = ptz_status.yaw;
                // tp.zoom = ptz_status.zoom;
                // TODO: AA 移植注释掉了 -- end

                // 遍历查询name是否重复，重复则删掉之前的wpt
                auto fgpx = doc.FirstChildElement("gpx");
                auto fwpt = fgpx->FirstChildElement("wpt");
                while (fwpt != nullptr) {
                    auto fname = fwpt->FirstChildElement("name");
                    if (fname != nullptr) {
                        if (tp.name == fname->GetText()) {
                            LOG(WARNING) << "TaskPoint: SaveCb: the same name , delete last";
                            fgpx->DeleteChild(fwpt);
                            break;
                        }
                        fwpt = fwpt->NextSiblingElement();
                    } else {
                        break;
                    }
                }

                auto e = tp.to_xml_element(doc, new_id);
                if (nullptr == e) {
                    throw std::runtime_error("保存任务点失败");
                }
                gpx->InsertEndChild(e);
                //  spdlog::error("req_type = {}", 699);
                if (doc.SaveFile(filetransmit.GetFileName(filename).c_str()) != tinyxml2::XML_SUCCESS) {
                    throw std::runtime_error("保存任务点文件失败");
                }
            }
            // spdlog::error("req_type = {}", 706);
            UpdateTaskPoint();

            return;
        }

        bool GetCurTaskPointData(BusinessTaskPoints& task_points) {
            task_points = task_points_;
        }

    private:
        void UpdateTaskPoint() {
            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(kTaskPointsFilePath.c_str())) {
                LOG(INFO) << "TaskPoint: task_points.gpx not exist, create!";
                if (!CreateFile(kTaskPointsFilePath, doc)) {
                    return;
                }
            }
            auto       gpx = doc.FirstChildElement("gpx");
            auto       wpt = gpx->FirstChildElement("wpt");
            TaskPoints task_points;

            task_points_.task_points.clear();
            while (wpt != nullptr) {
                TaskPoint item(wpt);
                task_points.push_back(item);
                wpt = wpt->NextSiblingElement();

                ara::adsfi::IdpBusinessTaskPoint track_task_points_item;
                track_task_points_item.name = item.name;
                track_task_points_item.lat = item.lat;
                track_task_points_item.lon = item.lon;
                track_task_points_item.type = int(item.type);
                task_points_.task_points.push_back(track_task_points_item);
            }

            AINFO << "task_points_.size()-------------------------------------------------"
                  << task_points_.task_points.size();

            // TODO: AA 移植注释掉了 -- start
            // Scheduler::Instance().UpdateTaskPoints(task_points);
            // TODO: AA 移植注释掉了 -- end

            // Waypoints nowWaypoints;
            // SCH->GetWaypoints(nowWaypoints);
            // 将任务点设置为起始结束点需求
            // SCH->UpdateWpsByTps(nowWaypoints, task_points);
        }

        bool CreateFile(const std::string& file_path) {
            tinyxml2::XMLDocument doc;

            return CreateFile(file_path, doc);
        }

        bool CreateFile(const std::string& file_path, tinyxml2::XMLDocument& doc) {
            LOG(INFO) << "create task point file: " << file_path;
            auto declaration = doc.NewDeclaration();
            doc.InsertFirstChild(declaration);
            auto gpx = doc.NewElement("gpx");
            gpx->SetAttribute("version", 1.0f);
            auto metadata = doc.NewElement("metadata");
            auto name = doc.NewElement("name");
            name->SetText("task_points");
            auto max_id = doc.NewElement("max_id");
            max_id->SetText(10);

            metadata->InsertEndChild(name);
            metadata->InsertEndChild(max_id);
            gpx->InsertFirstChild(metadata);
            doc.InsertEndChild(gpx);

            if (doc.SaveFile(file_path.c_str()) != tinyxml2::XML_SUCCESS) {
                return false;
            }

            return true;
        }

    private:
        const std::filesystem::path kDefaultTaskPointFilePath;
        const std::string           kGlobalTaskPointsFilePath;

        std::mutex mutex;
        // TODO: AA 移植注释掉了 -- start
        // ros::NodeHandle nh_;
        // TODO: AA 移植注释掉了 -- end
        TaskPointFileTransmit filetransmit;
        std::string           kTaskPointsFilePath; // 默认任务点文件路径
        BusinessTaskPoints    task_points_;        // 当前任务点
    };
} // namespace app_common
#endif
