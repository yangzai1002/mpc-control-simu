#pragma once

#include <unordered_map>

#include "GeoUtils.hpp"
#include "Waypoint.h"
#include "json.hpp"
#include "log/log.h"
#include "osmplanner/OSMDataBase.h"
#include "osmplanner/OSMHeader.h"
#include "osmplanner/osmhandle.h"

namespace app_common {
    class GlobalPathGeneration {
        typedef std::vector<Waypoint> Waypoints;

    public:
        GlobalPathGeneration() {
            oh_ = std::make_shared<mapplan::OSMHandle>();
        }

        /**
         * @brief osm路网初始化
         * @author zengweiyi
         */
        void Init(const std::string& path) {
            int rtvalue = -1;
            oh_.get()->Init(path.c_str());
            AINFO << "GlobalPathGeneration:" << path;
            if (oh_.get()->parse() == -1)
                return;

            mapplan::Nodes osm_nodes = mapplan::OSMDataBase::Instance().getNodes();
            mapplan::Ways  osm_ways = mapplan::OSMDataBase::Instance().getWays();

            // nlohmann::json roadInfosArray =nlohmann::json::parse(way_json_str);
            // // 使用哈希表来加速元素查找
            std::unordered_map<int, std::shared_ptr<mapplan::OSMWay>>  wayMap;
            std::unordered_map<int, std::shared_ptr<mapplan::OSMNode>> nodeMap;
            for (const auto& way : osm_ways) {
                wayMap[way->getId()] = way;
                static int test = 0;
                AINFO << test++ << std::endl;
            }
            for (const auto& node : osm_nodes) {
                nodeMap[node->getId()] = node;
            }
        }

        /**
         * @brief 通过路线信息获取路点数据
         *  @author zengweiyi
         */
        std::vector<Waypoint> GetWayPointsByRoadsInfo(const std::string way_json_str, bool const& use_circling = false) {
            Waypoints new_waypoints;

            if (way_json_str == "") {
                return new_waypoints;
            }

            mapplan::Nodes osm_nodes = mapplan::OSMDataBase::Instance().getNodes();
            mapplan::Ways  osm_ways = mapplan::OSMDataBase::Instance().getWays();

            nlohmann::json road_infos_obj = json::parse(way_json_str);

            nlohmann::json roadInfosArray = road_infos_obj["roads"];
            nlohmann::json last_point = road_infos_obj["last_point"];
            AINFO << roadInfosArray.dump();
            AINFO << last_point.dump();
            //  nlohmann::json roadInfosArray = json::parse(way_json_str);
            // 使用哈希表来加速元素查找
            std::unordered_map<int, std::shared_ptr<mapplan::OSMWay>>  wayMap;
            std::unordered_map<int, std::shared_ptr<mapplan::OSMNode>> nodeMap;
            for (const auto& way : osm_ways) {
                wayMap[way->getId()] = way;
            }
            for (const auto& node : osm_nodes) {
                nodeMap[node->getId()] = node;
            }

            // json waypoint_data;
            json waypoint_data_array = json::array();
            // Waypoint temp;
            // 遍历数组元素
            for (const auto& roadInfo : roadInfosArray) {
                // 访问 road_id 和 road_direction 字段
                //int id = roadInfo["road_id"];
                int signedId = roadInfo["road_id"];
                int unsignedId = std::abs(signedId);
                //int direction = roadInfo["road_direction"];
                if (roadInfo.count("road_points") > 0) {
                    json task_point = roadInfo["road_points"];

                    AINFO << "current_id:" << signedId;
                    AINFO << "(task_point.size()" << task_point.size();
                    if (task_point.size() > 0) {
                        for (const auto& task_item : roadInfo["road_points"]) {
                            double lat = task_item["lat"];
                            double lon = task_item["lon"];
                            // AINFO << "task_item_lat" << task_item["lat"];
                            // AINFO << "task_item_lon" << task_item["lon"];
                            waypoint_data_array.push_back({{"lat", lat}, {"lon", lon}});
                        }
                        continue;
                    }
                }

                AINFO << "roadInfo_id----------------------:" << signedId;

                auto wayIt = wayMap.find(unsignedId);
                if (wayIt != wayMap.end()) {
                    std::vector<Waypoint> waypoint_data_array_item;
                    const auto&           way = wayIt->second;
                    for (auto waypoint_id : way->getNdRefs()) {
                        auto nodeIt = nodeMap.find(waypoint_id);
                        if (nodeIt != nodeMap.end()) {
                            const auto& node = nodeIt->second;
                            // AINFO << "lat" << node->getLat();
                            // AINFO << "lon" << node->getLon();

                            if (signedId < 0) {
                                Waypoint temp_point;
                                temp_point.lat = node->getLat();
                                temp_point.lon = node->getLon();
                                waypoint_data_array_item.insert(waypoint_data_array_item.begin(), temp_point);
                            } else {
                                waypoint_data_array.push_back({{"lat", node->getLat()}, {"lon", node->getLon()}});
                            }
                        }
                    }
                    if (signedId < 0) {
                        for (const auto& waypoint : waypoint_data_array_item) {
                            double lat = waypoint.lat;
                            double lon = waypoint.lon;
                            ;

                            waypoint_data_array.push_back({{"lat", lat}, {"lon", lon}});
                        }
                    }
                }
            }
            AINFO << "GetWayPointsByRoadsInfo--------------------------------";

            Waypoints cur_waypoints;

            for (auto point_item : waypoint_data_array) {
                double lat = point_item["lat"];
                double lon = point_item["lon"];

                Waypoint temp_item;
                temp_item.lat = lat;
                temp_item.lon = lon;

                cur_waypoints.push_back(temp_item);
            }

            AINFO << "cur_waypoints size" << cur_waypoints.size();
            WaypointsInterpolation(cur_waypoints, new_waypoints);
            AINFO << "new_waypoints size" << new_waypoints.size();

            waypoint_data_array.clear();
            for (const auto& waypoint : new_waypoints) {
                double lat = waypoint.lat;
                double lon = waypoint.lon;
                ;

                waypoint_data_array.push_back({{"lat", lat}, {"lon", lon}});
            }

            double cur_last_point_lat = 0;
            double cur_last_point_lon = 0;

            if (last_point.contains("lat")) {
                cur_last_point_lat = last_point["lat"];
            }

            if (last_point.contains("lon")) {
                cur_last_point_lon = last_point["lon"];
            }
            // cur_last_point_lat = last_point["lat"].
            if ((!cur_last_point_lat == 0 || !cur_last_point_lon == 0) && !use_circling) {
                int site = GetNearPointSiteByLL(waypoint_data_array, cur_last_point_lat, cur_last_point_lon);
                int tempi = 0;
                int size = waypoint_data_array.size();
                AINFO << "cur_last_point_lat" << cur_last_point_lat;
                AINFO << "GetWayPointsByRoadsInfo--------------------------------site" << site;
                AINFO << "GetWayPointsByRoadsInfo--------------------------------total_size" << size;

                if (site != -1) {
                    waypoint_data_array.erase(waypoint_data_array.begin() + site, waypoint_data_array.end());
                }

                AINFO << "GetWayPointsByRoadsInfo--------------------------------total_size2"
                      << waypoint_data_array.size();
            }

            std::string test_str;
            test_str = waypoint_data_array.dump();
            AINFO << waypoint_data_array;

            // nlohmann::json roadInfosArray = json::parse(waypoints);

            // global_waypoint_info_.waypoints.clear();

            Waypoints res_points;
            // 遍历数组元素
            for (const auto& roadInfo : waypoint_data_array) {
                // 访问 road_id 和 road_direction 字段
                double   lat = roadInfo["lat"];
                double   lon = roadInfo["lon"];
                Waypoint waypoint;
                waypoint.lat = lat;
                waypoint.lon = lon;
                waypoint.speed = 5;
                waypoint.dis_start = 2;
                waypoint.fence_dis = 0;
                res_points.push_back(waypoint);
            }
            // UpdateWaypoints(global_waypoint_info_.waypoints,
            //                                     global_waypoint_info_.start,
            //                                     global_waypoint_info_.end,
            //                                     global_waypoint_info_.length);
            // dest_name = "z";

            return res_points;
        }

    private:
        /**
         * @brief 获取路网中相近位置序号
         *  @author zengweiyi
         */
        int GetNearPointSiteByLL(nlohmann::json point_array, double lat, double lon) {
            int    cur_point_site = -1;
            double cur_distance = -1;
            int    i = 0;

            // AINFO << "GetNearPointSiteByLL-1";
            for (const auto& item : point_array) {
                // 访问 road_id 和 road_direction 字段
                double obj_lat = item["lat"];
                double obj_lon = item["lon"];
                // AINFO << "GetNearPointSiteByLL-2";
                // AINFO << "GetWayPointsByRoadsInfo--------------------------------lat" << lat;
                // AINFO << "GetWayPointsByRoadsInfo--------------------------------lon" << lon;
                // AINFO << "lat:" << lat;
                // AINFO << "lon:" << lon;
                if (cur_distance == -1) {
                    cur_distance = app_common::GeoUtils::gps_distance(obj_lat, obj_lon, lat, lon);
                }
                // AINFO << "cur_distance" << cur_distance;
                if (cur_distance > app_common::GeoUtils::gps_distance(obj_lat, obj_lon, lat, lon)) {
                    cur_distance = app_common::GeoUtils::gps_distance(obj_lat, obj_lon, lat, lon);
                    cur_point_site = i;
                }
                i++;
            }

            return cur_point_site;
        }

        /**
         * @brief 给定指定的两个路点，获取路点之间的加密数据
         * @author zengweiyi
         */
        bool LinearInterpolation(const Waypoint& start, const Waypoint& end, const double resolution,
                                 Waypoints& result_path) {
            if (resolution < 0.1 || resolution > 5)
                return false;
            double length = app_common::GeoUtils::gps_distance(start.lat, start.lon, end.lat, end.lon);
            if (length < 1.6 * resolution) {
                return true;
            }
            int count = static_cast<int>((length + resolution / 2) / resolution);
            if (count < 2)
                return false;
            double detal_lat = (end.lat - start.lat) / static_cast<double>(count);
            double detal_lon = (end.lon - start.lon) / static_cast<double>(count);
            double detal_dis = (end.dis_start - start.dis_start) / static_cast<double>(count);
            result_path.clear();
            Waypoint position;
            for (int index = 1; index < count; index++) {
                position.lat = start.lat + detal_lat * index;
                position.lon = start.lon + detal_lon * index;
                position.dis_start = start.dis_start + detal_dis * index;
                position.speed = start.speed;
                position.fence_dis = start.fence_dis;
                result_path.push_back(position);
            }
            return true;
        }

        /**
         * @brief 循迹路线插值
         * @param item  :(out) waypoints
         * @author zengweiyi
         */
        void WaypointsInterpolation(const Waypoints& old_waypoints, Waypoints& new_waypoints) {
            // 遍历相邻的 Waypoint 进行插值
            for (auto iter = old_waypoints.begin(); iter != old_waypoints.end(); ++iter) {
                // 如果是最后一个 Waypoint，则无法再进行插值，跳出循环
                if (iter + 1 == old_waypoints.end()) {
                    break;
                }

                const Waypoint& start = *iter;
                const Waypoint& end = *(iter + 1);
                Waypoints       temp_waypoints;

                // 调用自己实现的 LinearInterpolation 函数进行插值
                bool interpolation_success = LinearInterpolation(start, end, 0.1, temp_waypoints);
                if (interpolation_success == false) {
                    AERROR << "插值失败---";
                    continue;
                }
                // 将临时的插值路径添加到结果路径中
                new_waypoints.insert(new_waypoints.end(), temp_waypoints.begin(), temp_waypoints.end());
            }
        }

    private:
        std::shared_ptr<mapplan::OSMHandle> oh_;
    };
} // namespace app_common
