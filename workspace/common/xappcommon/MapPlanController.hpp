#ifndef MAP_PLAN_CONTROLLER_HPP
#define MAP_PLAN_CONTROLLER_HPP

#include <openssl/md5.h>

#include <cstdint>
#include <filesystem>
#include <vector>

#include "GpxFileHandler.hpp"
#include "HashUtils.hpp"
#include "JsonUtils.hpp"
#include "MapFileHandler.hpp"
#include "Waypoint.h"
#include "adsfi/impl_type_locationinfo.h"
#include "cppcodec/base64_rfc4648.hpp"
#include "custom_stack.h"
#include "log/log.h"
#include "osmplanner/coord.h"
#include "osmplanner/planner.hpp"

namespace app_common {
    class MapPlanController {
    public:
        MapPlanController(const std::string& base_dir_gpx, const std::string& base_dir_map)
            : _map_file_handler(base_dir_map)
            , _gpx_file_handler(base_dir_gpx, "default_track.txt")
            , _current_slice(0)
            , _task_status(false)
            , _plan_sn(0) {
            k_working_dir = std::filesystem::path(base_dir_gpx).parent_path().string();
            std::filesystem::path global_osm(k_working_dir);
            global_osm.append(k_global_osm);
            if (std::filesystem::exists(global_osm)) {
                auto r = _path_planner.Init(global_osm.c_str());
                if (0 == r) {
                    AINFO << "path planner init with " << global_osm.string() << " ok!";
                } else {
                    AERROR << "path planner init with " << global_osm.string() << " error: " << r;
                    throw std::runtime_error("global osm format error");
                }
            } else {
                AINFO << "global osm " << global_osm.string() << " does not exist!";
            }
        }

        MapPlanController(const MapPlanController&) = delete;

        MapPlanController(MapPlanController&&) = delete;

    private:
        struct PointDef {
            double      lat;
            double      lon;
            double      yaw;
            bool        yaw_valid;
            std::string type;
        };

        struct RoadInfo {
            std::int32_t road_id;
            std::int32_t road_direction;
        };

        // 必经点
        static constexpr const char* THROUGH = "THROUGH";
        // 禁行点
        static constexpr const char* FORBIDDEN = "FORBIDDEN";

        std::string                  k_working_dir;
        static constexpr const char* k_global_osm = "global.osm";
        static constexpr const char* k_global_compress = "compress.tar.xz";

    public:
        void upload(const nlohmann::json& req, nlohmann::json& res) {
            std::string  req_filename;
            std::string  req_filetype;
            std::string  req_md5;
            std::int32_t req_size;
            std::int16_t req_slice_index;
            std::int16_t req_slice_total;
            std::int32_t req_slice_size;
            std::string  req_content;

            app_common::JsonUtils::valueFromJson(req_filename, req, "filename", false);
            app_common::JsonUtils::valueFromJson(req_filetype, req, "filetype", false);
            app_common::JsonUtils::valueFromJson(req_md5, req, "md5", true);
            app_common::JsonUtils::valueFromJson(req_size, req, "size", true);
            app_common::JsonUtils::valueFromJson(req_slice_index, req, "slice_index", false);
            app_common::JsonUtils::valueFromJson(req_slice_total, req, "slice_total", false);
            app_common::JsonUtils::valueFromJson(req_slice_size, req, "slice_size", false);

            AINFO << "upload, filename: " << req_filename << ", filetype: " << req_filetype << ", md5: " << req_md5
                  << ", size: " << req_size << ", slice_size: " << req_slice_size
                  << ", slice_total: " << req_slice_total << ", slice_index: " << req_slice_index;

            if (0 == req_slice_index) {
                _current_slice = 0;
                _buffer.clear();
            } else if (req_slice_index != _current_slice) {
                _current_slice = 0;
                _buffer.clear();
                throw std::runtime_error("上传失败，当前序号错误");
            }

            if (req_filename.empty()) { // MRCT 2.6 版本的行为
                app_common::JsonUtils::valueFromJson(req_content, req, "data", true);
                if (req_md5 != app_common::HashUtils::MD5(req_content)) {
                    throw std::runtime_error("上传失败，校验字不");
                }

                _buffer += req_content;
                _current_slice += 1;

                if (_current_slice < req_slice_total) {
                    return;
                }

                if (_buffer.size() != req_size) {
                    throw std::runtime_error("上传失败，文件大小不匹配");
                }

                std::filesystem::path global_osm(k_working_dir);
                global_osm.append(k_global_osm);
                std::ofstream os(global_osm);
                if (!os.is_open()) {
                    throw std::runtime_error("上传失败，打开文件失败");
                }
                os << _buffer;

                if (_path_planner.Reset(global_osm.c_str()) != 0) {
                    throw std::runtime_error("上传失败，上传内容解析失败");
                }
            } else if ("raw" == req_filetype) { // MRCT 2.6-Xpatrol-E 版本的行为
                app_common::JsonUtils::valueFromJson(req_content, req, "content", true);
                if (req_md5 != app_common::HashUtils::MD5(req_content)) {
                    throw std::runtime_error("上传失败，校验字不");
                }

                _buffer += req_content;
                _current_slice += 1;

                if (_current_slice < req_slice_total) {
                    return;
                }

                if (_buffer.size() != req_size) {
                    throw std::runtime_error("上传失败，文件大小不匹配");
                }

                std::filesystem::path global_osm(k_working_dir);
                global_osm.append(k_global_osm);
                std::ofstream os(global_osm);
                if (!os.is_open()) {
                    throw std::runtime_error("上传失败，打开文件失败");
                }
                os << _buffer;

                if (_path_planner.Reset(global_osm.c_str()) != 0) {
                    throw std::runtime_error("上传失败，上传内容解析失败");
                }
            } else { // MRCT 2.7 版本的行为
                app_common::JsonUtils::valueFromJson(req_content, req, "content", true);
                if (req_md5 != app_common::HashUtils::MD5(req_content)) {
                    throw std::runtime_error("上传失败，校验字不");
                }

                std::vector<uint8_t> decodedData;
                try {
                    decodedData = cppcodec::base64_rfc4648::decode(req_content);
                } catch (const std::exception& e) {
                    throw std::runtime_error("上传失败，解码失败");
                }

                // 将解码后的数据转换为字符串
                std::string decodedString(decodedData.begin(), decodedData.end());
                // 将转换后的字符串赋值给 req.content
                req_content = decodedString;

                _buffer += req_content;
                _current_slice += 1;
                if (_current_slice < req_slice_total) {
                    return;
                }

                if (_buffer.size() != req_size) {
                    throw std::runtime_error("上传失败，文件大小不匹配");
                }

                std::filesystem::path compressed(k_working_dir);
                compressed.append(k_global_compress);
                {
                    std::ofstream os(compressed);
                    if (!os.is_open()) {
                        throw std::runtime_error("上传失败，打开文件 " + std::string(k_global_compress) + " 失败");
                    }
                    os << _buffer;
                    _current_slice = 0;
                    _buffer.clear();
                }

                auto command1 = "tar -xJf " + compressed.string() + " -C " + k_working_dir;
                AINFO << command1;
                system(command1.c_str());
                std::filesystem::path global_osm(k_working_dir);
                std::filesystem::path uploaded_osm(k_working_dir);
                global_osm.append(k_global_osm);
                uploaded_osm.append(req_filename);
                std::filesystem::rename(uploaded_osm, global_osm);
                _map_file_handler.map_file_content(req_filename, global_osm);
                if (_path_planner.Reset(global_osm.c_str()) != 0) {
                    throw std::runtime_error("上传失败，文件解析失败");
                }
            }
        }

        void plan(const nlohmann::json& req, const adsfi::LocationInfo& location, nlohmann::json& res) {
            res["data"] = nlohmann::json::object({});

            if (!app_common::GeoUtils::is_location_ok(location)) {
                res["data"]["result_type"] = "PLAN_FAILED";
                res["message"] = "路径规划失败，定位状态异常";
                return;
                // throw std::runtime_error("路径规划失败，定位状态异常");
            }

            std::filesystem::path global_osm(k_working_dir);
            global_osm.append(k_global_osm);
            if (!std::filesystem::exists(global_osm)) {
                res["data"]["result_type"] = "NO_MAP";
                res["message"] = "路径规划失败，路网文件不存在";
                return;
                // throw std::runtime_error("路径规划失败，路网文件不存在");
            }

            std::string md5;
            app_common::JsonUtils::valueFromJson(md5, req, "md5", true);

            app_common::JsonUtils::field_should_be_array(req, "waypoints");
            std::vector<PointDef> defined_points;
            for (auto& element : req["waypoints"]) { // type array
                PointDef p;
                app_common::JsonUtils::valueFromJson(p.lat, element, "lat", true);
                app_common::JsonUtils::valueFromJson(p.lon, element, "lon", true);
                app_common::JsonUtils::valueFromJson(p.yaw, element, "yaw", true);
                app_common::JsonUtils::valueFromJson(p.yaw_valid, element, "yaw_valid", true);
                app_common::JsonUtils::valueFromJson(p.type, element, "type", true);
                defined_points.emplace_back(std::move(p));
            }
            AINFO << "plan, md5: " << md5 << ", point_size: " << defined_points.size();
            { // 为了尽快释放内存，所以写到一个块中
                auto c = read_file(global_osm.string());
                if (app_common::HashUtils::MD5(c) != md5) {
                    res["data"]["result_type"] = "NO_MAP";
                    res["message"] = "路径规划失败，路网文件校验失败";
                    return;
                    // throw std::runtime_error("路径规划失败，路网文件校验失败");
                }
            }

            // MRCT 2.6&2.7 的行为不太一样。
            // res["data"]["result_type"] = "PLAN_OK";
            // return;

            PointDef wp;
            wp.lat = location.pose.pose.position.x;
            wp.lon = location.pose.pose.position.y;
            wp.yaw = location.pose.pose.orientation.z;
            wp.yaw_valid = true;
            wp.type = THROUGH;
            _passing_points = defined_points;
            defined_points.insert(defined_points.begin(), wp);
            AINFO << "---------- defined points: start ----------";
            for (auto& item : defined_points) {
                AINFO << "item, lat: " << item.lat << ", lon: " << item.lon << ", yaw: " << item.yaw
                      << ", yaw_valid: " << item.yaw_valid << ", type: " << item.type;
            }
            AINFO << "---------- defined points: end   ----------";
            AINFO << "passing_points_size: " << _passing_points.size();
            AINFO << "waypoints_size: " << defined_points.size();
            _task_status = false;

            std::int32_t                 planned_sn;
            std::vector<mapplan::latlon> planned_points;
            std::vector<mapplan::OSMId>  planned_road_ids;
            std::vector<RoadInfo>        planned_road_infos;
            _plan(defined_points, planned_sn, planned_points, planned_road_ids, planned_road_infos);

            auto& res_obj = res["data"];
            for (auto& roads_item : planned_road_ids) { // pri array
                res_obj["roads"].push_back(roads_item);
            }

            for (auto& road_info_item : planned_road_infos) {
                nlohmann::json temp_item;
                temp_item["road_id"] = std::abs(road_info_item.road_id);
                // AINFO << "temp_item["road_id"]" << temp_item["road_id"];
                // spdlog::error("temp_item[: {}", temp_item["road_id"]);
                temp_item["road_direction"] = road_info_item.road_direction;
                res_obj["road_infos"].push_back(temp_item);
            }

            nlohmann::json        res_path_obj; // type prev
            std::vector<Waypoint> planned_waypoints;
            for (auto& res_path_item_msg : planned_points) { // type array
                nlohmann::json res_path_item_obj;
                res_path_item_obj["lat"] = res_path_item_msg.lat; // pri
                res_path_item_obj["lon"] = res_path_item_msg.lon; // pri
                res_path_obj.push_back(res_path_item_obj);

                Waypoint wp{};
                wp.lat = res_path_item_msg.lat;
                wp.lon = res_path_item_msg.lon;
                planned_waypoints.push_back(wp);
            }
            AINFO << "planned points size: " << planned_points.size();
            // TODO: 暂时先保存到一个文件中，后续再优化
            _gpx_file_handler.gpx_file_remove("GLOBALOSM");
            // 要求全局规划的路径是加密过的
            _gpx_file_handler.gpx_file_new("GLOBALOSM", "q", "z", planned_waypoints, true);

            res_obj["plan_sn"] = planned_sn; // pri
            res_obj["path"] = res_path_obj;  // type post
            res_obj["result_type"] = "PLAN_OK";
        }

        void md5(const nlohmann::json& req, nlohmann::json& res) {
            std::string md5_value;
            md5_value = CalculateMD5(std::string(k_working_dir) + "/" + k_global_osm);
            if (md5_value != "") {
                res["data"]["md5"] = md5_value;
                res["message"] = "获取md5成功";
                return;
            } else {
                res["data"]["md5"] = md5_value;
                res["message"] = "获取md5失败";
            }
        }

    private:
        static std::string read_file(std::string path) {
            std::ifstream     s(path);
            std::stringstream buffer;
            buffer << s.rdbuf();
            return buffer.str();
        }

        std::string CalculateMD5(const std::string& filename) {
            unsigned char digest[MD5_DIGEST_LENGTH];
            MD5_CTX       ctx;
            MD5_Init(&ctx);

            std::ifstream file(filename, std::ifstream::binary);
            if (!file) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return "";
            }

            char buffer[1024];
            while (file.good()) {
                file.read(buffer, sizeof(buffer));
                MD5_Update(&ctx, buffer, file.gcount());
            }
            MD5_Final(digest, &ctx);

            std::ostringstream oss;
            for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
            }

            return oss.str();
        }

        bool _plan(const std::vector<PointDef> defined_points, std::int32_t planned_sn,
                   std::vector<mapplan::latlon>& planned_points, std::vector<mapplan::OSMId>& planned_road_ids,
                   std::vector<RoadInfo>& planned_road_infos) {
            // 初始化目标点和禁止点的向量
            std::vector<mapplan::latlon> points_target;
            std::vector<mapplan::latlon> points_forbidden;

            for (auto& wp : defined_points) {
                mapplan::latlon ll;
                ll.lat = wp.lat;
                ll.lon = wp.lon;
                ll.yaw = wp.yaw;
                ll.yaw_valid = wp.yaw_valid;
                if (THROUGH == wp.type) {
                    points_target.push_back(ll);
                } else if (FORBIDDEN == wp.type) {
                    points_forbidden.push_back(ll);
                } else {
                    AERROR << "unsupported point type: " << wp.type;
                }
            }

            int plan_res = _path_planner.PlanBaseSearch(points_target, points_forbidden, _plan_result);
            if (plan_res != 0) {
                AERROR << "global plan PlanBaseSearch return -1";
                throw std::runtime_error("路径规划失败，PlanBaseSearch");
            }

            AINFO << "plan result size: " << _plan_result.size();
            _plan_sn += 1;

            std::vector<Waypoint> temp_waypoints;
            for (auto& wp : _plan_result) {
                mapplan::latlon latlon;
                latlon.lat = wp.lat;
                latlon.lon = wp.lon;
                planned_points.push_back(latlon);

                Waypoint waypoint;
                waypoint.lat = wp.lat;
                waypoint.lon = wp.lon;
                waypoint.speed = 10;
                waypoint.dis_start = 2;
                waypoint.fence_dis = 0;
                temp_waypoints.push_back(waypoint);
            }

            std::vector<mapplan::RoadInfo> roadinfos;
            if (_path_planner.GetRoadInfo(roadinfos) != 0) {
                AERROR << "global plan getRoadInfo return -1";
                throw std::runtime_error("路径规划失败，GetRoadInfo");
            }
            AINFO << "road info: ";
            for (auto& ri : roadinfos) {
                AINFO << ri.edgeId;
                AINFO << ri.direction;
                planned_road_ids.push_back(ri.edgeId);

                RoadInfo road_info;
                road_info.road_id = ri.edgeId;
                if (ri.edgeId < 0) {
                    ri.direction = 0;
                } else {
                    ri.direction = 1;
                }
                road_info.road_direction = ri.direction;
                planned_road_infos.push_back(road_info);
            }

            _matching_passing_point_to_road();

            planned_sn = _plan_sn;

            // res.result_type = mugv_api::GlobalplanPlanResponse::PLAN_OK;
            // res.success = true;

            // kiwi::GlobalToTask temp_task;
            // temp_task.task_status = true;
            // if (task_status == true) {
            // adapter_pub_.publish(temp_task);
            // }

            return true;
        }

        void _matching_passing_point_to_road() {
            _passing_point_indexes.clear();
            int front_idx = 0;
            for (auto& pp : _passing_points) {
                double min_dis = std::numeric_limits<double>::max();
                int    nearest_idx = 0;
                AINFO << "pp: " << pp.lat << ", " << pp.lon;
                for (int i = front_idx; i != _plan_result.size(); i++) {
                    auto&  wp = _plan_result[i];
                    double dis = app_common::GeoUtils::gps_distance(pp.lat, pp.lon, wp.lat, wp.lon);
                    // double angle = std::fmod(std::fabs(pp.yaw - wp.yaw), 360);
                    // AINFO << "idx: " << i << ", dis: " << dis;
                    // if (dis <= min_dis && (angle > 270 || angle < 90)) {
                    if (dis <= min_dis) {
                        min_dis = dis;
                        nearest_idx = i;
                        front_idx = nearest_idx;
                    }
                }
                AINFO << "nearest_idx: " << nearest_idx;
                _passing_point_indexes.push_back(nearest_idx);
            }
            AINFO << "matching done";
        }

    private:
        app_common::MapFileHandler _map_file_handler;
        app_common::GpxFileHandler _gpx_file_handler;

        std::string _buffer;
        int         _current_slice;

        std::vector<PointDef> _passing_points;
        bool                  _task_status;

        mapplan::GlobalPlanner       _path_planner;
        std::vector<mapplan::latlon> _plan_result;
        std::int32_t                 _plan_sn;
        std::vector<std::int32_t>    _passing_point_indexes; // 必经点在返回路径点中的位置
    };
} // namespace app_common

#endif
