#ifndef __LOCATIONINFO_HANDLER_HPP__
#define __LOCATIONINFO_HANDLER_HPP__
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "adsfi/impl_type_locationinfo.h"
#include "geometry_tools.hpp"

namespace app_common {

    class LocationInfoHandler {
    public:
        LocationInfoHandler() = delete;

        static bool SyncLocationInfo(adsfi::LocationInfo &info) {
            GeoPointGPS gps;
            gps.lon = info.pose.pose.position.x;
            gps.lat = info.pose.pose.position.y;
            gps.heading = info.pose.pose.orientation.z;

            if (gps.Check() == false) {
                return false;
            }

            GeoPointGCCS gccs;
            GeometryTool::GPS2GCCS(gps, gccs);
            info.pose.covariance[0] = gccs.xg;
            info.pose.covariance[1] = gccs.yg;
            info.pose.covariance[2] = gccs.angle;
            return true;
        }

        static double Lon(const adsfi::LocationInfo &info) {
            return info.pose.pose.position.y;
        }

        static double Lat(const adsfi::LocationInfo &info) {
            return info.pose.pose.position.x;
        }

        static double Heading(const adsfi::LocationInfo &info) {
            return info.pose.pose.orientation.z;
        }

        static double Xg(const adsfi::LocationInfo &info) {
            return info.pose.covariance[0];
        }
        static double Yg(const adsfi::LocationInfo &info) {
            return info.pose.covariance[1];
        }

        static double Speed(const adsfi::LocationInfo &info) {
            return sqrt(info.velocity.twist.linear.x * info.velocity.twist.linear.x +
                        info.velocity.twist.linear.y * info.velocity.twist.linear.y +
                        info.velocity.twist.linear.z * info.velocity.twist.linear.z);
        }
    };

} // namespace app_common
#endif