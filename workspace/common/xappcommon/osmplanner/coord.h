#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "OSMHeader.h"
#include "OSMNode.h"
#include "nanoflann.h"

namespace mapplan {
    enum class ePointType { START = 0, END = 1, MISSION = 2, DEFAULT = 3, BLOCK = 4 };

    struct latlon {
        double     lat = -999;
        double     lon = -999;
        double     yaw = -999;
        bool       yaw_valid = false;
        bool       is_reverse = false;
        ePointType type = ePointType::DEFAULT;

        bool operator==(const latlon& ll) const {
            return lat == ll.lat && lon == ll.lon && yaw == ll.yaw;
        }
    };

    struct hash_latlon {
        size_t operator()(const latlon& ll) const {
            return std::hash<double>()(ll.lat) ^ std::hash<double>()(ll.lon) ^ std::hash<double>()(ll.yaw) ^
                   std::hash<bool>()(ll.yaw_valid);
        }
    };

    class Coord {
    public:
        struct XY {
            /* data */
            double x;
            double y;
        };

        static double ComputeDist(double lat1, double lon1, double lat2, double lon2) {
            double dLat = lat2 * DEG2RAD - lat1 * DEG2RAD;
            double dLon = lon2 * DEG2RAD - lon1 * DEG2RAD;
            double a = sin(dLat / 2) * sin(dLat / 2) +
                       cos(lat1 * DEG2RAD) * cos(lat2 * DEG2RAD) * sin(dLon / 2) * sin(dLon / 2);
            double c = 2 * atan2(sqrt(a), sqrt(1 - a));
            return R * c;
        }
        // 计算两点之间的航向角
        static double ComputeAzimuth(double lat1, double lon1, double lat2, double lon2) {
            double a = (90 - lat2) * M_PI / 180;
            double b = (90 - lat1) * M_PI / 180;
            double AOC_BOC = (lon2 - lon1) * M_PI / 180;
            double cosc = cos(a) * cos(b) + sin(a) * sin(b) * cos(AOC_BOC);
            double sinc = sqrt(1 - cosc * cosc);
            double sinA = sin(a) * sin(AOC_BOC) / sinc;
            double A = asin(sinA) * 180 / M_PI;
            double res = 0;
            if (lon2 > lon1 && lat2 > lat1)
                res = A;
            else if (lon2 > lon1 && lat2 < lat1)
                res = 180 - A;
            else if (lon2 < lon1 && lat2 < lat1)
                res = 180 - A;
            else if (lon2 < lon1 && lat2 > lat1)
                res = 360 + A;
            else if (lon2 > lon1 && lat2 == lat1)
                res = 90;
            else if (lon2 < lon1 && lat2 == lat1)
                res = 270;
            else if (lon2 == lon1 && lat2 > lat1)
                res = 0;
            else if (lon2 == lon1 && lat2 < lat1)
                res = 180;

            return res;
        }

        static double ComputeAzimuth(const std::vector<latlon>& edgepoints, bool is_begin) {
            if (is_begin) {
                auto iter = edgepoints.begin();
                if (std::fabs(std::distance(iter, edgepoints.end())) < 11)
                    iter = edgepoints.end();
                else
                    std::advance(iter, 10);

                const auto& yaw_begin =
                    Coord::ComputeAzimuth(edgepoints.front().lat, edgepoints.front().lon, iter->lat, iter->lon);
                return yaw_begin;
            }

            auto iter = edgepoints.end();
            if (std::fabs(std::distance(iter, edgepoints.begin())) < 11)
                iter = edgepoints.begin();
            else
                std::advance(iter, -10);

            const auto& yaw_end =
                Coord::ComputeAzimuth(edgepoints.back().lat, edgepoints.back().lon, iter->lat, iter->lon);
            return yaw_end;
        }

        static double ComputeBearing(double lat1, double lon1, double lat2, double lon2) {
            double dLon = lon2 * DEG2RAD - lon1 * DEG2RAD;
            double y = sin(dLon) * cos(lat2 * DEG2RAD);
            double x =
                cos(lat1 * DEG2RAD) * sin(lat2 * DEG2RAD) - sin(lat1 * DEG2RAD) * cos(lon2 * DEG2RAD) * cos(dLon);
            return atan2(y, x);
        }

        static void AdjustCoordinates(bounds bounds_, const Node n, XY* xy) {
            const auto lat2ym = [&](double lat) { return log(tan(lat * DEG2RAD / 2 + M_PI / 4)) / 2 * R; };
            const auto lon2xm = [&](double lon) { return lon * DEG2RAD / 2 * R; };

            double m_MaxLon = bounds_.maxlon;
            double m_MaxLat = bounds_.maxlat;
            double m_MinLon = bounds_.minlon;
            double m_MinLat = bounds_.minlat;

            const auto dx = lon2xm(m_MaxLon) - lon2xm(m_MinLon);
            const auto dy = lat2ym(m_MaxLat) - lat2ym(m_MinLat);
            const auto min_y = lat2ym(m_MinLat);
            const auto min_x = lon2xm(m_MinLon);
            double     m_MetricScale = std::min(dx, dy);

            double lat = n->getLat();
            double lon = n->getLon();
            xy->x = (lon2xm(lon) - min_x) / m_MetricScale;
            xy->y = (lat2ym(lat) - min_y) / m_MetricScale;
        }

        static void AdjustCoordinates(bounds bounds_, latlon ll, XY* xy) {
            const auto lat2ym = [&](double lat) { return log(tan(lat * DEG2RAD / 2 + M_PI / 4)) / 2 * R; };
            const auto lon2xm = [&](double lon) { return lon * DEG2RAD / 2 * R; };

            double m_MaxLon = bounds_.maxlon;
            double m_MaxLat = bounds_.maxlat;
            double m_MinLon = bounds_.minlon;
            double m_MinLat = bounds_.minlat;

            const auto dx = lon2xm(m_MaxLon) - lon2xm(m_MinLon);
            const auto dy = lat2ym(m_MaxLat) - lat2ym(m_MinLat);
            const auto min_y = lat2ym(m_MinLat);
            const auto min_x = lon2xm(m_MinLon);
            double     m_MetricScale = std::min(dx, dy);

            double lat = ll.lat;
            double lon = ll.lon;
            xy->x = (lon2xm(lon) - min_x) / m_MetricScale;
            xy->y = (lat2ym(lat) - min_y) / m_MetricScale;
        }

    private:
        static constexpr double R = 6371e3;
        static constexpr double DEG2RAD = M_PI / 180;
        static constexpr double RAD2DEG = 180 / M_PI;
        static constexpr double OFFSET = M_PI / 2;
    };
} // namespace mapplan
