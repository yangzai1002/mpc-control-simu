#ifndef WAYPOINT_H
#define WAYPOINT_H

struct Waypoint {
    double lat;       // latitude
    double lon;       // longitude
    double speed;     // speed
    double dis_start; // distance from start point
    double fence_dis; // distance from fence

    Waypoint(double lat_, double lon_, double speed_, double dis_start_, double dis_fence_)
        : lat(lat_), lon(lon_), speed(speed_), dis_start(dis_start_), fence_dis(dis_fence_) {
    }

    Waypoint(double lat_, double lon_) : Waypoint(lat_, lon_, 0.0, 0.0, 0.0) {
    }

    Waypoint() : Waypoint(0.0, 0.0, 0.0, 0.0, 0.0) {
    }
};

#endif
