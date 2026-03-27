/******************************************************************************
* Copyright (C) 2015-2020, idriverplus(BeiJing ZhiXingZhe, Inc.)
*
* NodeName: vectormap
* FileName: geotool.h
*
* Description: fool AVOS geotool API like USB

*
* History:
* lbh         2018/05/21    1.0.0    build this module.
******************************************************************************/
#pragma once

#include <iostream>

#include "proj_api.h"

/**
 *@brief macros using by check functions in follow structs about point
 */
#define RETURN_EQ(val1, val2) \
    if ((val1) == (val2))     \
        return -1;
#define RETURN_NE(val1, val2) \
    if ((val1) != (val2))     \
        return -1;

#define BOOL_GE(val1, val2) ((val1) >= (val2)) ? 1 : 0

#define BOOL_IN(val1, val2, val3) (((val1) >= (val2)) && ((val1) <= (val3))) ? 1 : 0

/**
 *@brief the min and max values of xg yg in GeoPointGCCS
 */
constexpr double kXGMIN = 166021.443081, kXGMAX = 833978.556908, kYGMIN = -8881585.81599, kYGMAX = 9328093.83056;
/**
 *@brief the min and max values of lon lat in PointGPS
 */
constexpr double kLONMIN = -180, kLONMAX = 180, kLATMIN = -80, kLATMAX = 84;
/**
 *@brief define the input and output points using in the API
 *@func Check() check the members in point structs
 */
struct GeoPointGCCS {
    double xg;
    double yg;
    double angle;
    bool   Check() {
        bool b1 = BOOL_IN(xg, kXGMIN, kXGMAX);
        bool b2 = BOOL_IN(yg, kYGMIN, kYGMAX);
        return b1 && b2;
    }
    GeoPointGCCS() {
        xg = 0.0;
        yg = 0.0;
        angle = 0.0;
    }
    GeoPointGCCS(const double &_xg, const double &_yg, const double &_angle) {
        xg = _xg;
        yg = _yg;
        angle = _angle;
    }
};
struct GeoPointVCS {
    double x;
    double y;
    double angle;
    GeoPointVCS() {
        x = 0.0;
        y = 0.0;
        angle = 0.0;
    }

    GeoPointVCS(const double &_x, const double &_y, const double &_angle) {
        x = _x;
        y = _y;
        angle = _angle;
    }
};

struct GeoPointGPS {
    double lon;
    double lat;
    double heading;
    bool   Check() {
        bool b1 = BOOL_IN(lon, kLONMIN, kLONMAX);
        bool b2 = BOOL_IN(lat, kLATMIN, kLATMAX);
        return b1 && b2;
    }

    GeoPointGPS() {
        lon = 0.0;
        lat = 0.0;
        heading = 0.0;
    }

    GeoPointGPS(const double &_lon, const double &_lat, const double &_heading) {
        lon = _lon;
        lat = _lat;
        heading = _heading;
    }
};

class GeometryTool {
public:
    GeometryTool() {
    }

    static int GetLongZone(double longitude) {
        double longZone = 0.0;
        if (longitude < 0.0) {
            longZone = ((180.0 + longitude) / 6.0) + 1;
        } else {
            longZone = (longitude / 6.0) + 31;
        }
        return static_cast<int>(longZone);
    }
    static double Angle2Heading(double input) {
        double output = 90 - input;
        if (output < 0)
            output += 360;
        return output;
    }
    static double Heading2Angle(double input) {
        double output = 90 - input;
        if (output < -180)
            output += 360;
        if (output > 180)
            output -= 360;
        return output;
    }
    static int GeoTransform(const GeoPointGCCS &pgccs, const int utm_zone, GeoPointGPS &pgps) {
        RETURN_EQ(const_cast<GeoPointGCCS &>(pgccs).Check(), 0);
        double      x = pgccs.xg, y = pgccs.yg;
        double      z = 0.;
        std::string utmInit =
            "+proj=utm +zone=" + std::to_string(utm_zone) + "+ellps=WGS84 +datum=WGS84 +units=m +no_defs";
        projPJ lonlat = pj_init_plus(" +proj=longlat +datum=WGS84 +no_defs");
        projPJ utm = pj_init_plus(utmInit.c_str());
        RETURN_NE(pj_transform(utm, lonlat, 1, 1, &x, &y, &z), 0);
        x *= RAD_TO_DEG;
        y *= RAD_TO_DEG;
        pgps.lon = x;
        pgps.lat = y;
        pgps.heading = Angle2Heading(pgccs.angle);
        if (lonlat) {
            pj_free(lonlat);
        }
        if (utm) {
            pj_free(utm);
        }
        return 0;
    }

    static int GeoTransform(const GeoPointGPS &pgps, GeoPointGCCS &pgccs) {
        RETURN_EQ(const_cast<GeoPointGPS &>(pgps).Check(), 0);
        double      lon = pgps.lon, lat = pgps.lat;
        double      height = 0.;
        std::string utmInit =
            "+proj=utm +zone=" + std::to_string(GetLongZone(lon)) + "+ellps=WGS84 +datum=WGS84 +units=m +no_defs";
        projPJ lonlat = pj_init_plus(" +proj=longlat +datum=WGS84 +no_defs");
        projPJ utm = pj_init_plus(utmInit.c_str());
        lon *= DEG_TO_RAD;
        lat *= DEG_TO_RAD;
        RETURN_NE(pj_transform(lonlat, utm, 1, 1, &lon, &lat, &height), 0);
        pgccs.xg = lon;
        pgccs.yg = lat;
        pgccs.angle = Heading2Angle(pgps.heading);
        if (lonlat) {
            pj_free(lonlat);
        }
        if (utm) {
            pj_free(utm);
        }
        return 0;
    }

    static int GeoTransform(const GeoPointGCCS &car_pgccs, const GeoPointGCCS &target_pgccs, GeoPointVCS &output_pvcs) {
        RETURN_EQ(const_cast<GeoPointGCCS &>(car_pgccs).Check(), 0);
        RETURN_EQ(const_cast<GeoPointGCCS &>(target_pgccs).Check(), 0);
        double rad = car_pgccs.angle * DEG_TO_RAD;
        double dx = target_pgccs.xg - car_pgccs.xg;
        double dy = target_pgccs.yg - car_pgccs.yg;
        output_pvcs.x = +dx * cos(rad) + dy * sin(rad);
        output_pvcs.y = -dx * sin(rad) + dy * cos(rad);
        output_pvcs.angle = target_pgccs.angle - car_pgccs.angle;
        if (output_pvcs.angle >= 360)
            output_pvcs.angle -= 360;
        return 0;
    }

    static int GeoTransform(const GeoPointGCCS &car_pgccs, const GeoPointVCS &target_pvcs, GeoPointGCCS &output_pgccs) {
        // RETURN_EQ(const_cast<GeoPointGCCS &>(car_pgccs).Check(), 0);
        double rad = car_pgccs.angle * M_PI / 180.0;
        output_pgccs.xg = target_pvcs.x * cos(rad) - target_pvcs.y * sin(rad);
        output_pgccs.yg = target_pvcs.x * sin(rad) + target_pvcs.y * cos(rad);
        output_pgccs.xg += car_pgccs.xg;
        output_pgccs.yg += car_pgccs.yg;
        output_pgccs.angle = car_pgccs.angle + target_pvcs.angle;
        if (output_pgccs.angle >= 360)
            output_pgccs.angle -= 360;
        return 1;
    }

    static int GCCS2GPS(const GeoPointGCCS &pgccs, GeoPointGPS &pgps, int utm_zone = 50) {
        return GeoTransform(pgccs, utm_zone, pgps);
    }
    static int GPS2GCCS(const GeoPointGPS &pgps, GeoPointGCCS &pgccs) {
        return GeoTransform(pgps, pgccs);
    }

    static int GCCS2VCS(const GeoPointGCCS &car_pgccs, const GeoPointGCCS &target_pgccs, GeoPointVCS &output_pvcs) {
        return GeoTransform(car_pgccs, target_pgccs, output_pvcs);
    }
    static int VCS2GCCS(const GeoPointGCCS &car_pgccs, const GeoPointVCS &target_pvcs, GeoPointGCCS &output_pgccs) {
        return GeoTransform(car_pgccs, target_pvcs, output_pgccs);
    }
};
