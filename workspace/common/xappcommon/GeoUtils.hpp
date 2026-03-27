#ifndef GEO_UTILS_HPP
#define GEO_UTILS_HPP

#include <time.h>

#include <cmath>
#include <filesystem>
#include <optional>

#include "PatrolType.h"
#include "Waypoint.h"
#include "ara/adsfi/impl_type_businesspoint.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "config_loader/custom_stack.h"
#include "geometry_tools.hpp"
#include "log/log.h"
#include "math.h"

namespace app_common {
    class GeoUtils {
    public:
        GeoUtils() = delete;

    public:
        static constexpr double pi = 3.1415926535897932384626433832795;
#define EARTH_RADIUS 6378.137 // 地球半径 KM
#define NEAR_POINT_SIZE 2     // 任务点滤除用于判断当前点位置是里起点或终点两点以内

        static double rad(double d) {
            return d * pi / 180.0;
        }

        static inline GeoPointGCCS get_gccs(double lat, double lon) {
            GeoPointGPS  gps{};
            GeoPointGCCS gccs{};
            gps.lat = lat;
            gps.lon = lon;
            GeometryTool::GPS2GCCS(gps, gccs);
            return gccs;
        }

        static inline double gccs_distance(double x1, double y1, double x2, double y2) {
            double x = (x1 - x2);
            double y = (y1 - y2);
            return std::sqrt(x * x + y * y);
        }

        template <typename T> static inline double gccs_distance(const T &t1, const T &t2) {
            double x = (t1.xg - t2.xg);
            double y = (t1.yg - t2.yg);
            return std::sqrt(x * x + y * y);
        }

        template <typename T> static inline double gccs_distance(const std::vector<T> &gccs_points) {
            double distance = 0.0;
            if (gccs_points.size() < 2) {
                return distance;
            }
            for (size_t i = 1; i < gccs_points.size(); ++i) {
                distance += gccs_distance(gccs_points[i - 1], gccs_points[i]);
            }
            return distance;
        }

        static inline double gps_distance(double lat1, double lon1, double lat2, double lon2) {
            double p = 0.017453292519943295; // Math.PI / 180
            double a = 0.5 - std::cos((lat2 - lat1) * p) / 2 +
                       std::cos(lat1 * p) * std::cos(lat2 * p) * (1 - std::cos((lon2 - lon1) * p)) / 2;
            return 12742 * std::asin(std::sqrt(a)) * 1000; // 2 * R; R = 6371 km
        }

        template <typename T> static double gps_distance(const std::vector<T> &gps_points) {
            double distance = 0.0;
            if (gps_points.size() < 2) {
                return distance;
            }
            for (size_t i = 1; i < gps_points.size(); ++i) {
                distance += gps_real_distance(gps_points[i - 1].lat, gps_points[i - 1].lon, gps_points[i].lat,
                                              gps_points[i].lon);
            }
            return distance;
        }

        // lat1第一个点纬度,lng1第一个点经度,lat2第二个点纬度,lng2第二个点经度
        static double gps_real_distance(double lat1, double lng1, double lat2, double lng2) {
            double a;
            double b;
            double radLat1 = rad(lat1);
            double radLat2 = rad(lat2);
            a = radLat1 - radLat2;
            b = rad(lng1) - rad(lng2);
            double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
            s = s * EARTH_RADIUS;
            s = s * 1000;
            return s;
        }

        template <typename GPS, typename GCCS> static std::vector<GCCS> gps_to_gccs(const std::vector<GPS> &points) {
            std::vector<GCCS> l;
            l.reserve(points.size());
            for (const auto &x : points) {
                GeoPointGPS  pgps(x.lon, x.lat);
                GeoPointGCCS pgccs{};
                GeometryTool::GPS2GCCS(pgps, pgccs);
                l.emplace_back(std::move(pgccs));
            }
            return l;
        }

        template <typename T> static void gps_heading(T &p1, const T &p2) {
            const double DEG2RAD = M_PI / 180.0;
            const double RAD2DEG = 180.0 / M_PI;

            double lat1 = p1.lat * DEG2RAD;
            double lat2 = p2.lat * DEG2RAD;
            double dlon = (p2.lon - p1.lon) * DEG2RAD;

            double y = sin(dlon) * cos(lat2);
            double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);

            p1.heading = atan2(y, x) * RAD2DEG;
            if (p1.heading < 0)
                p1.heading += 360.0;
        }

        template <typename T> static std::size_t nearest_index(const T &gccs_points, double xg, double yg) {
            std::size_t nearest_index = 0;

            double min_distance = std::numeric_limits<double>::max();
            for (decltype(gccs_points.size()) i = 0; i < static_cast<int>(gccs_points.size()); ++i) {
                const auto &pt = gccs_points[i];
                double      dist = gccs_distance(pt.xg, pt.yg, xg, yg);
                if (dist < min_distance) {
                    min_distance = dist;
                    nearest_index = i;
                }
            }

            return nearest_index;
        }

        template <typename T>
        static std::size_t nearest_index(const T &gccs_points, double xg, double yg, double speed, double seconds) {
            std::size_t nearest_index = 0;
            double      min_distance = std::numeric_limits<double>::max();
            double      total_distance = 0;
            double      distance_limit = speed * seconds;

            if (gccs_points.size() < 2) {
                return 0;
            }
            for (decltype(gccs_points.size()) i = 1; i < gccs_points.size(); ++i) {
                const auto &_this_pt = gccs_points[i];
                const auto &_prev_pt = gccs_points[i - 1];
                auto        inter_dist = gccs_distance(_this_pt.xg, _this_pt.yg, _prev_pt.xg, _prev_pt.yg);
                total_distance += inter_dist;
                if (total_distance > distance_limit) {
                    break;
                }

                auto dist = gccs_distance(_this_pt.xg, _this_pt.yg, xg, yg);
                if (dist < min_distance) {
                    min_distance = dist;
                    nearest_index = i;
                }
            }

            return nearest_index;
        }

        template <typename T> static inline T location_target(const T &src, double angle, double distance) {
            double rad = angle * M_PI / 180.0;
            double factor = cos(src.pose.pose.position.x * M_PI / 180.0);

            // 纬度/经度偏移
            double dLat = (distance * cos(rad)) / 111320.0;
            double dLon = (distance * sin(rad)) / (111320.0 * factor);

            T target{};
            target.pose.pose.position.x = src.pose.pose.position.x + dLat;
            target.pose.pose.position.y = src.pose.pose.position.y + dLon;

            return target;
        }

        template <typename T> static void update_length_to_start_point(T &points) {
            double length = 0;
            auto   iter = points.begin();
            auto   last = iter;
            for (; iter != points.end(); ++iter) {
                length += gps_real_distance(iter->lat, iter->lon, last->lat, last->lon);
                iter->dis_start = length;
                last = iter;
            }
        }

        template <typename T> static int GetNearPointSiteByLL(const T &waypoints, double lat, double lon) {
            int    cur_point_site = -1;
            double cur_distance = -1;
            int    i = 0;

            // AINFO << "GetNearPointSiteByLL-1";
            for (const auto &item : waypoints) {
                // 访问 road_id 和 road_direction 字段
                double obj_lat = item.lat;
                double obj_lon = item.lon;
                if (cur_distance == -1) {
                    cur_distance = gps_real_distance(obj_lat, obj_lon, lat, lon);
                }
                // AINFO << "cur_distance" << cur_distance;
                if (cur_distance > gps_real_distance(obj_lat, obj_lon, lat, lon)) {
                    cur_distance = gps_real_distance(obj_lat, obj_lon, lat, lon);
                    cur_point_site = i;
                }
                i++;
            }

            return cur_point_site;
        }

        // 基于 UTM (xg, yg) 的等距离线性插值
        // 返回闭区间 [start, end]
        template <typename T>
        static void gccs_interpolate(const T &start, const T &end, double resolution, std::vector<T> &result_path) {
            result_path.clear();
            if (resolution <= 0.0) {
                throw std::runtime_error("invalid resolution: " + std::to_string(resolution));
            }

            const double dx = end.xg - start.xg;
            const double dy = end.yg - start.yg;
            const double length = std::hypot(dx, dy);
            // 退化情况：两点重合或距离极短
            if (length < 1e-6) {
                result_path.push_back(start);
                result_path.push_back(end);
                return;
            }

            // 至少保证 start 和 end
            const int    count = std::max(1, static_cast<int>(std::ceil(length / resolution)));
            const double step_x = dx / static_cast<double>(count);
            const double step_y = dy / static_cast<double>(count);
            for (int i = 0; i <= count; ++i) {
                T p{};
                p.xg = start.xg + step_x * i;
                p.yg = start.yg + step_y * i;
                result_path.push_back(p);
            }
        }

        template <typename T> static void gccs_interpolation(const std::vector<T> &old_points,
                                                             std::vector<T> &new_points, double resolution = 0.1) {
            new_points.clear();

            if (old_points.empty()) {
                return;
            }

            if (old_points.size() == 1) {
                new_points.push_back(old_points.front());
                return;
            }

            std::vector<T> temp_points;
            temp_points.reserve(32); // 避免频繁分配

            for (auto iter = old_points.begin(); iter + 1 != old_points.end(); ++iter) {
                const T &start = *iter;
                const T &end = *(iter + 1);

                temp_points.clear();
                gccs_interpolate(start, end, resolution, temp_points);

                if (temp_points.empty()) {
                    continue;
                }

                auto insert_begin = temp_points.begin();
                if (iter != old_points.begin() && temp_points.size() > 1) {
                    ++insert_begin; // 避免重复点
                }

                new_points.insert(new_points.end(), insert_begin, temp_points.end());
            }
        }

        // 返回的是闭区间
        static void LinearInterpolation(const Waypoint &start, const Waypoint &end, const double resolution,
                                        std::vector<Waypoint> &result_path) {
            result_path.clear();
            if (resolution <= 0) {
                throw std::runtime_error("invalid resolution: " + std::to_string(resolution));
            }

            double length = gps_real_distance(start.lat, start.lon, end.lat, end.lon);
            int    count = static_cast<int>(std::ceil(length / resolution));
            if (count < 2) {
                result_path.push_back(start);
                result_path.push_back(end);
                return;
            }

            double detal_lat = (end.lat - start.lat) / static_cast<double>(count);
            double detal_lon = (end.lon - start.lon) / static_cast<double>(count);
            double detal_dis = (end.dis_start - start.dis_start) / static_cast<double>(count);

            Waypoint position;
            for (int index = 0; index < count; index++) {
                position.lat = start.lat + detal_lat * index;
                position.lon = start.lon + detal_lon * index;
                position.dis_start = start.dis_start + detal_dis * index;
                position.speed = start.speed;
                position.fence_dis = start.fence_dis;
                result_path.push_back(position);
            }
        }

        static void WaypointsInterpolation(const std::vector<Waypoint> &old_waypoints,
                                           std::vector<Waypoint> &      new_waypoints) {
            // 遍历相邻的 Waypoint 进行插值
            for (auto iter = old_waypoints.begin(); iter != old_waypoints.end(); ++iter) {
                // 如果是最后一个 Waypoint，则无法再进行插值，跳出循环
                if (iter + 1 == old_waypoints.end()) {
                    break;
                }

                const Waypoint &      start = *iter;
                const Waypoint &      end = *(iter + 1);
                std::vector<Waypoint> temp_waypoints;

                // 调用自己实现的 LinearInterpolation 函数进行插值
                LinearInterpolation(start, end, 0.1, temp_waypoints);
                // 将临时的插值路径添加到结果路径中
                auto mystart = temp_waypoints.begin();
                if (iter != old_waypoints.begin()) {
                    std::advance(mystart, 1);
                }
                new_waypoints.insert(new_waypoints.end(), mystart, temp_waypoints.end());
            }
        }

        template <typename T> static T GetWaypointsBack(const T &waypoints, double lat, double lon) {
            // 找到当前点在路点上的最近点
            double                                   min_distance = std::numeric_limits<double>::max();
            std::map<double, typename T::value_type> dis_map;
            for (auto &wp : waypoints) {
                double dis = gps_real_distance(lat, lon, wp.lat, wp.lon);
                dis_map.emplace(dis, wp);
            }
            min_distance = dis_map.begin()->first;
            if (min_distance > 5) {
                AERROR << "taskpoint to wp too long.";
            }

            auto curwp = dis_map.begin()->second;
            auto waypscur_itr =
                find_if(waypoints.begin(), waypoints.end(), [&curwp](const typename T::value_type &waypoint) -> bool {
                    return (waypoint.lat == curwp.lat && waypoint.lon == curwp.lon && waypoint.speed == curwp.speed);
                });
            T new_waypoints;
            std::copy(waypoints.begin(), waypscur_itr + 1, std::back_inserter(new_waypoints));
            std::reverse(new_waypoints.begin(), new_waypoints.end());
            return new_waypoints;
        }

        static void AddVirtualWaypoint(std::vector<Waypoint> &waypoints) {
            if (waypoints.size() < 2) {
                AERROR << "waypoints number less than 2";
                return;
            }
            // 补一个点，以免在后续提路的时候最后一个点丢失 -- start。
            const auto   end = waypoints.back();
            const auto   prev_end = std::prev(waypoints.end(), 2);
            GeoPointGPS  end_pgps(end.lon, end.lat);
            GeoPointGPS  prev_end_pgps(prev_end->lon, prev_end->lat);
            GeoPointGCCS end_pgccs;
            GeoPointGCCS prev_end_pgccs;
            GeometryTool::GPS2GCCS(end_pgps, end_pgccs);
            GeometryTool::GPS2GCCS(prev_end_pgps, prev_end_pgccs);
            AINFO << "end pgps: " << end_pgps.lat << ", " << end_pgps.lon;
            AINFO << "end pgccs: " << end_pgccs.yg << ", " << end_pgccs.xg;
            AINFO << "prev end pgps: " << prev_end_pgps.lat << ", " << prev_end_pgps.lon;
            AINFO << "prev end pgccs: " << prev_end_pgccs.yg << ", " << prev_end_pgccs.xg;

            auto         heading = std::atan2(end_pgccs.yg - prev_end_pgccs.yg, end_pgccs.xg - prev_end_pgccs.xg);
            GeoPointGPS  extend_pgps;
            GeoPointGCCS extend_pgccs;
            extend_pgccs.xg = end_pgccs.xg + 5.0 * std::cos(heading);
            extend_pgccs.yg = end_pgccs.yg + 5.0 * std::sin(heading);
            extend_pgccs.angle = heading;
            GeometryTool::GCCS2GPS(extend_pgccs, extend_pgps, GeometryTool::GetLongZone(end_pgps.lon));
            Waypoint tmp{};
            tmp.lat = extend_pgps.lat;
            tmp.lon = extend_pgps.lon;
            tmp.speed = end.speed;
            tmp.dis_start =
                gps_real_distance(extend_pgps.lat, extend_pgps.lon, waypoints.front().lat, waypoints.front().lon);
            tmp.fence_dis = waypoints.front().dis_start;
            waypoints.emplace_back(std::move(tmp));

            AINFO << std::setprecision(7) << "end_point, "
                  << "lat: " << end.lat << ", lng: " << end.lon << ", dis_start: " << end.dis_start
                  << ", fence_dis: " << end.fence_dis;
            const auto &extend = waypoints.back();
            AINFO << std::setprecision(7) << "extend_point, "
                  << "lat: " << extend.lat << ", lng: " << extend.lon << ", dis_start: " << extend.dis_start
                  << ", fence_dis: " << extend.fence_dis;
            // 补一个点，以免在后续提路的时候最后一个点丢失 -- end。
        } // namespace app_common

        static void Waypoints2Trackpoints(const std::vector<Waypoint> &           waypoints,
                                          std::vector<ara::adsfi::BusinessPoint> &trackpoints,
                                          // std::optional<double> speed = std::optional<double>{},
                                          std::optional<std::int32_t> start_index = std::optional<std::int32_t>{},
                                          std::optional<bool>         is_backward = std::optional<bool>{}) {
            auto iter = waypoints.begin();
            auto next = std::next(iter);
            auto index = start_index.has_value() ? start_index.value() : 0;
            auto direction = (is_backward.has_value() ? is_backward.value() : false) ? 1 : 0;
            for (; iter != waypoints.end() && next != waypoints.end(); ++iter, next = std::next(iter)) {
                GeoPointGPS  this_pgps(iter->lon, iter->lat);
                GeoPointGPS  next_pgps(next->lon, next->lat);
                GeoPointGCCS this_pgccs;
                GeoPointGCCS next_pgccs;
                GeometryTool::GPS2GCCS(this_pgps, this_pgccs);
                GeometryTool::GPS2GCCS(next_pgps, next_pgccs);
                double heading =
                    std::atan2(next_pgccs.yg - this_pgccs.yg, next_pgccs.xg - this_pgccs.xg) / M_PI * 180.0;
                if (heading > 180) {
                    heading -= 360;
                } else if (heading < -180) {
                    heading += 360;
                }
                ara::adsfi::BusinessPoint tmp{};
                tmp.index = index++;
                tmp.xg = this_pgccs.xg;
                tmp.yg = this_pgccs.yg;
                // tmp.lon = iter->lon;
                // tmp.lat = iter->lat;
                tmp.heading = heading;
                // tmp.length = iter->dis_start;
                // .avoid = true;
                // .speed = speed.is_initialized() ? speed.value() : iter->speed;
                tmp.direction = direction;
                // .dis_fence = 0,
                // AINFO << tmp.angle << ", " << tmp.xg << ", " << tmp.yg << ", " << tmp.heading << ", " <<
                // tmp.direction;
                trackpoints.emplace_back(tmp);
            }
        }

        static std::vector<ara::adsfi::BusinessPoint> Waypoints2Trackpoints(
            const std::vector<Waypoint> &waypoints,
            // std::optional<double> speed = std::optional<double>{},
            std::optional<std::int32_t> start_index = std::optional<std::int32_t>{},
            std::optional<bool>         is_backward = std::optional<bool>{}) {
            std::vector<ara::adsfi::BusinessPoint> trackpoints;
            // Waypoints2Trackpoints(waypoints, trackpoints, speed, start_index,
            // is_backward);
            Waypoints2Trackpoints(waypoints, trackpoints, start_index, is_backward);
            return std::move(trackpoints);
        }

        static std::vector<GeoPointGCCS> Waypoints2PointGCCS(const std::vector<Waypoint> &waypoints) {
            std::vector<GeoPointGCCS> l;
            for (const auto &x : waypoints) {
                GeoPointGPS  pgps(x.lon, x.lat);
                GeoPointGCCS pgccs;
                GeometryTool::GPS2GCCS(pgps, pgccs);
                l.push_back(pgccs);
            }

            return l;
        }

        static std::vector<Waypoint> PointGCCS2Waypoints(const std::vector<GeoPointGCCS> &pgccs, int zone) {
            std::vector<Waypoint> l;
            for (const auto &x : pgccs) {
                GeoPointGPS pgps{};
                GeometryTool::GCCS2GPS(x, pgps, zone);
                Waypoint w{};
                w.lon = pgps.lon;
                w.lat = pgps.lat;
                l.push_back(w);
            }

            return l;
        }

        /*
        要求 T1 和 T2 必须有 double 类型的 lat, lon 成员
        */
        template <typename T1, typename T2>
        static std::vector<std::vector<T1>> split_waypoints(const std::vector<T1> &waypoints,
                                                            const std::vector<T2> &split_points) {
            std::vector<std::vector<T1>> result;
            if (waypoints.empty()) {
                return result;
            }

            // 对每个 split_point 找到在 waypoints 中最近的索引
            std::vector<size_t> split_indices;
            for (const auto &sp : split_points) {
                double min_dist = std::numeric_limits<double>::max();
                size_t min_idx = 0;
                for (size_t i = 0; i < waypoints.size(); ++i) {
                    const auto &w = waypoints[i];
                    double      d = gps_real_distance(w.lat, w.lon, sp.lat, sp.lon);
                    if (d < min_dist) {
                        min_dist = d;
                        min_idx = i;
                    }
                }
                split_indices.push_back(min_idx);
            }

            // 对 split_indices 排序（以防无序）
            std::sort(split_indices.begin(), split_indices.end());

            // 划分段，即使 split_indices 有两个或多个同样的值，也会返回多个段。
            size_t start = 0;
            for (size_t idx : split_indices) {
                result.emplace_back(waypoints.begin() + start, waypoints.begin() + idx + 1);
                start = idx + 1;
            }

            // 如果剩余的 waypoints 还没放进来
            if (start < waypoints.size()) {
                result.emplace_back(waypoints.begin() + start, waypoints.end());
            }

            return result;
        }
        template <typename T> static inline bool is_location_ok(const T &location, bool is_must) {
            if (!is_must) {
                bool enable_bad_gps_tasking = false;
                try {
                    enable_bad_gps_tasking =
                        CustomStack::Instance()->GetProjectConfigValue<bool>("enable_bad_gps_tasking");
                } catch (const std::exception &e) {
                    AWARN << "cannot read enable_bad_gps_tasking";
                }
                if (enable_bad_gps_tasking) {
                    return true;
                }
                AINFO << "lcoation.locationState" << (int)location.locationState;
            }
            return (4 == location.locationState || 5 == location.locationState);
        }

        template <typename T> static inline bool is_location_ok(const T &location) {
            return is_location_ok(location, false);
        }

        template <typename T> inline static std::string str_gps(const T &t) {
            std::ostringstream ss;
            ss << std::setprecision(12) << "(" << t.lat << ", " << t.lon << ")";
            return ss.str();
        }

        inline static std::string str_gps(double lat, double lon) {
            std::ostringstream ss;
            ss << std::setprecision(12) << "(" << lat << ", " << lon << ")";
            return ss.str();
        }

        template <typename T> inline static std::string str_gccs(const T &t) {
            std::ostringstream ss;
            ss << std::setprecision(12) << "(" << t.xg << ", " << t.yg << ")";
            return ss.str();
        }

        inline static std::string str_gccs(double xg, double yg) {
            std::ostringstream ss;
            ss << std::setprecision(12) << "(" << xg << ", " << yg << ")";
            return ss.str();
        }

        template <typename T> inline static std::string str_relative(const T &t) {
            std::ostringstream ss;
            ss << std::setprecision(12) << "(" << t.x << ", " << t.y << ")";
            return ss.str();
        }

        static std::string str_patrol_type(PatrolType t) {
            std::string x;

            if (PatrolType::Oneway == t) {
                x = "ONEWAY";
            } else if (PatrolType::TurnAround == t) {
                x = "TURN_AROUND";
            } else if (PatrolType::Reverse == t) {
                x = "REVERSE";
            }

            return x;
        }

        static std::pair<std::int32_t, std::int32_t> gps_week_second() {
            time_t current_time = time(nullptr);
            tm *   gps_epoch = gmtime(&current_time);
            // 将当前时间设置为GPS时间的起点
            gps_epoch->tm_year = 1980 - 1900; // 从1980年开始
            gps_epoch->tm_mon = 0;            // 1月
            gps_epoch->tm_mday = 6;           // 日期为6号
            gps_epoch->tm_hour = 0;           // 小时
            gps_epoch->tm_min = 0;            // 分钟
            gps_epoch->tm_sec = 0;            // 秒

            // 计算当前时间与GPS时间起点的差值（秒）
            time_t time_difference = current_time - mktime(gps_epoch);
            if (time_difference < 0) {
                return std::make_pair(0, 0);
            }

            // 计算GPS周数和周内秒数
            auto gps_week = (std::int32_t)(time_difference / 604800); // 604800秒 = 7天
            auto gps_second = (std::int32_t)(time_difference % 604800);
            return {gps_week, gps_second};
        }

        static double gps_time_to_second(std::uint32_t utc_w, double utc_s) {
            constexpr double UUDT = 315964800 - 18;
            constexpr double WEEKS = 7 * 24 * 3600.0;
            return UUDT + utc_w * WEEKS + utc_s;
        }
    }; // namespace app_common

    template <> inline bool GeoUtils::is_location_ok(const ara::adsfi::MsgHafLocation &location, bool is_must) {
        if (!is_must) {
            bool enable_bad_gps_tasking = false;
            try {
                enable_bad_gps_tasking = CustomStack::Instance()->GetProjectConfigValue<bool>("enable_bad_gps_tasking");
            } catch (const std::exception &e) {
                AWARN << "cannot read enable_bad_gps_tasking";
            }
            if (enable_bad_gps_tasking) {
                return true;
            }
            AINFO << "lcoation.status" << static_cast<std::int32_t>(location.locationState);
        }
        return (4 == location.locationState || 5 == location.locationState);
    }
    template <> inline bool GeoUtils::is_location_ok(const ara::adsfi::MsgHafLocation &location) {
        return is_location_ok(location, false);
    }

    template <> inline std::string GeoUtils::str_gps(const ara::adsfi::MsgHafLocation &t) {
        std::ostringstream ss;
        ss << std::setprecision(12) << "(" << t.llh.x << ", " << t.llh.y << ")";
        return ss.str();
    }

} // namespace app_common

#endif // UTILS_HPP
