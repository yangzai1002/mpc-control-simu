#ifndef MARKER_HELPER_HPP
#define MARKER_HELPER_HPP

//#include <visualization_msgs/MarkerArray.h>
#include "publisher.h"
#include <string>

namespace pure_pursuit
{

//
inline mdc::visual::Point createMarkerPosition(double x, double y, double z)
{
    mdc::visual::Point point;

    point.x = x;
    point.y = y;
    point.z = z;

    return point;
}

//
inline mdc::visual::Quaternion createMarkerOrientation(double x, double y,
                                                       double z, double w)
{
    mdc::visual::Quaternion quaternion;

    quaternion.x = x;
    quaternion.y = y;
    quaternion.z = z;
    quaternion.w = w;

    return quaternion;
}

//
inline mdc::visual::Vector3 createMarkerScale(double x, double y, double z)
{
    mdc::visual::Vector3 scale;

    scale.x = x;
    scale.y = y;
    scale.z = z;

    return scale;
}

//
inline mdc::visual::ColorRGBA createMarkerColor(float r, float g, float b,
                                                float a)
{
    mdc::visual::ColorRGBA color;

    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    return color;
}

//
inline mdc::visual::Marker createDefaultMarker(
        const std::string &frame_id, const std::string &ns, const int32_t id,
        const mdc::visual::MarkerType type, const mdc::visual::Vector3 &scale,
        const mdc::visual::ColorRGBA &color)
{
    mdc::visual::Marker marker;

    marker.header.frameId = frame_id;

    marker.header.stamp = pure_pursuit::planning_utils::getCurrentTimeVis();

    marker.ns = ns;
    marker.id = id;
    marker.type = type;
    marker.action = mdc::visual::MarkerAction::ADD;

    mdc::visual::Times life_t;
    life_t.sec = 0;
    life_t.nsec = 1e8;
    marker.lifetime = life_t;

    marker.pose.position = createMarkerPosition(0.0, 0.0, 0.0);
    marker.pose.orientation = createMarkerOrientation(0.0, 0.0, 0.0, 1.0);
    marker.scale = scale;
    marker.color = color;
    marker.frameLocked = true;

    return marker;
}

//
// inline void appendMarkerArray(
//         const visualization_msgs::MarkerArray &additional_marker_array,
//         visualization_msgs::MarkerArray *marker_array)
// {
//     for (const auto &marker : additional_marker_array.markers)
//     {
//         marker_array->markers.push_back(marker);
//     }
// }

}    // namespace

#endif  // MARKER_HELPER_HPP
