#pragma once

#include <unistd.h>

#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>

#include "OSMAstar.h"
#include "coord.h"
#include "geotool.h"
#include "osmhandle.h"
#include "path_smoothing.h"

namespace mapplan {
    class GlobalPlanner {
    public:
        GlobalPlanner() = default;
        ~GlobalPlanner() = default;

        int PlanBaseSearch(std::vector<latlon>& targetpoints, const std::vector<latlon>& block_points,
                           std::vector<latlon>& result, bool is_return = false) {
            int rtvalue = -1;

            for (size_t i = 0; i < targetpoints.size(); ++i) {
                if (i == 0) {
                    targetpoints.at(i).type = ePointType::START;
                    continue;
                }
                if (i == targetpoints.size() - 1) {
                    targetpoints.at(i).type = ePointType::END;
                    continue;
                }
                targetpoints.at(i).type = ePointType::MISSION;
                ;
            }

            std::vector<latlon> path_vec;
            if (astar_->SearchBasedPoint(targetpoints, block_points, path_vec) == -1) {
                std::cout << "INFO: search path failed" << '\n';
                return is_return ? 1 : -1; //一键返航，如果当前是一键返航搜索失败返回1，使用之前正向搜索的路径
            }
            result.clear();
            for (const auto& e : path_vec) {
                latlon ll;
                ll.lat = e.lat;
                ll.lon = e.lon;
                result.emplace_back(ll);
            }

            std::cout << " result size " << result.size() << '\n';
            if(result.size() <= 0){
                return -1;
            }

            // //转换坐标系
            // int                        zoom = GetLongZone(result.back().lon);
            // auto                       path_process_ptr = std::make_shared<PathProcessing>();
            // SiteList                   pathtemp;
            // avos::vectormap::GeoTool   geotool_;
            // avos::vectormap::PointGCCS ego;
            // avos::vectormap::PointGPS  first_gps_point{
            //     .lon = result.front().lon, .lat = result.front().lat, .heading = .0};
            // geotool_.SetUtmZone(zoom);
            // geotool_.GPS2GCCS(first_gps_point, ego);

            // for (const auto& p : result) {
            //     avos::vectormap::PointGCCS gccs_point_node;
            //     avos::vectormap::PointGPS  gps_point_node{.lon = p.lon, .lat = p.lat, .heading = 0.0};
            //     avos::vectormap::PointVCS  vcs;
            //     geotool_.GPS2GCCS(gps_point_node, gccs_point_node);
            //     geotool_.GCCS2VCS(ego, gccs_point_node, vcs);
            //     Site site;
            //     site.x = vcs.x;
            //     site.y = vcs.y;
            //     site.angle = vcs.angle;
            //     pathtemp.emplace_back(site);
            // }
            // //插值
            // auto start_iter = pathtemp.begin();
            // auto end_iter = std::next(start_iter);
            // while (end_iter != pathtemp.end()) {
            //     path_process_ptr->LinearInterpolation(start_iter, end_iter, 0.5, pathtemp);
            //     start_iter = end_iter;
            //     end_iter = std::next(start_iter);
            // }
            // //补全angle
            // auto iter = pathtemp.begin();
            // for (; next(iter) != pathtemp.end(); iter++) {
            //     iter->angle = std::atan2((next(iter)->y - iter->y), (next(iter)->x - iter->x)) * 180.0 / M_PI;
            //     if (iter->reverse) {
            //         iter->angle += 180.0;
            //     }
            //     if (iter->angle > 180.0)
            //         iter->angle -= 360.0;
            //     if (iter->angle < -180.0)
            //         iter->angle += 360.0;
            // }
            // //平滑处理
            // path_process_ptr->BezierSmooth(pathtemp);
            // //转换回经纬度
            // result.clear();
            // for (const auto& p : pathtemp) {
            //     avos::vectormap::PointVCS  vcs;
            //     avos::vectormap::PointGCCS gccs;
            //     avos::vectormap::PointGPS  gps;
            //     vcs.x = p.x;
            //     vcs.y = p.y;
            //     vcs.angle = p.angle;
            //     geotool_.VCS2GCCS(ego, vcs, gccs);
            //     geotool_.GCCS2GPS(gccs, gps);
            //     latlon ll;
            //     ll.lat = gps.lat;
            //     ll.lon = gps.lon;
            //     result.emplace_back(ll);
            // }

            // std::cout << "zzzz  GlobalPlanner::PlanBaseSearch 2 size = " <<
            // result.size()  << std::endl;

            rtvalue = 0;
            return rtvalue;
        }

        int PlanBaseRoadId(const std::list<int>& road, std::vector<latlon>& result) {
            std::vector<OSMId> road_ids;
            for (const auto& r : road) {
                road_ids.emplace_back(r);
            }

            if (astar_->SearchBaseRoadId(road_ids, result) == -1) {
                std::cout << "INFO: Generate path failed" << '\n';
                return -1;
            }
            return 0;
        }

        int GetRoadInfo(std::vector<RoadInfo>& roadInfo) {
            roadInfo.clear();
            astar_->OutputRoadInfo(roadInfo);
            if (roadInfo.empty())
                return -1;
            return 0;
        }

        int Init(const char* osmFilePath) {
            int rtvalue = -1;
            oh_.Init(osmFilePath);

            if (oh_.parse() == -1)
                return rtvalue;

            astar_ = std::make_shared<OSMAstar>();

            return 0;
        }

        int Reset(const char* osmFilePath) {
            int rtvalue = -1;
            oh_.Reset();

            oh_.Init(osmFilePath);

            if (oh_.parse() == -1)
                return rtvalue;

            if (astar_ == nullptr) {
                astar_ = std::make_shared<OSMAstar>();
            }
            astar_->Reset();

            return 0;
        }

        int GetLongZone(double longitude) {
            double longZone = 0.0;
            if (longitude < 0.0) {
                longZone = ((180.0 + longitude) / 6.0) + 1;
            } else {
                longZone = (longitude / 6.0) + 31;
            }
            return static_cast<int>(longZone);
        }

    private:
        OSMHandle                 oh_;
        std::shared_ptr<OSMAstar> astar_;
    };
} // namespace mapplan