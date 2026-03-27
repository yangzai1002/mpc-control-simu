#ifndef COGNITION_BASE_H_
#define COGNITION_BASE_H_

#include "xkit/planning/toolbox/geometry/geoheader.h"
// #include <sensor_msgs/PointCloud2.h>
// #include "geometry_msgs/Polygon.h"

//pcl lib
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
// #include <pcl_conversions/pcl_conversions.h>
// #include <pcl_ros/point_cloud.h>

using geometry::Site;
namespace avos{
namespace planning {

class CognitionBase {
  public:
    CognitionBase() {};
    virtual ~CognitionBase() {};

  public:

    virtual void Process() = 0;
    virtual void PullData() = 0;
    virtual void PushData() = 0;
};

} // namespace planning
} // namespace avos


#endif