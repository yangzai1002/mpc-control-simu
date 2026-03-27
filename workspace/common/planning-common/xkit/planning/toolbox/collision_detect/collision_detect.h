#ifndef COMMON_TOOBOX_COLLISION_DETECT_INCLUDE_COLLISION_DETECT_H_
#define COMMON_TOOBOX_COLLISION_DETECT_INCLUDE_COLLISION_DETECT_H_
#include <limits>
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/geometry/circular_point.h"
#include "xkit/planning/toolbox/geometry/pose.h"
#include "xkit/planning/toolbox/geometry/dwa_header.h"
// #include "base_log/log.h"
#include "xkit/planning/datapool/cognition_typedef.h"
#include "xkit/planning/datapool/common_typedef.h"
#include "xkit/planning/datapool/data_pool.h"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "xkit/planning/datapool/common_config.h"

#include "ara/adsfi/impl_type_msgpoint3dvec.h"

#include <locale>

using std::numeric_limits;
using namespace avos::planning;
// using namespace avos::dwa;
#define COLLISION_INNER 2
class ObjectCollisionDetect
{
public:
  ObjectCollisionDetect() {};
  ~ObjectCollisionDetect() {};

  // param:
       // direction: 0:front; 1:back; -1:default
  template <class T>
  static bool CollisionDetection(const T &point,
                                 SortedTree &obs,
                                 const avos::planning::CarModel &car_model,
                                 const int &direction,
                                 ara::adsfi::MsgPoint3D &collision_point,
                                 int half = 0) {

    //half 0:whole car, 1:left-half car, 2:right-half car

    ara::adsfi::MsgPoint3D p, q, tp;
    ara::adsfi::MsgPoint3DVec poly;
    collision_point.x = __DBL_MAX__;
    collision_point.y = __DBL_MAX__;

    // std::cout << "obs size::" << obs.size() << std::endl;

    tp.x = point.x;
    tp.y = point.y;
    // tp.z = point.angle;
    tp.z = point.angle / 180 * M_PI;
    // std::cout << "points::" << tp.x << "," << tp.y << "," << tp.z << "," << point.globalangle << std::endl;
    // std::cout << "car::" << car_model.front_over_hang << "," << car_model.back_over_hang << "," << car_model.half_wheel<< std::endl;
    double front_over_hang = car_model.front_over_hang;
    double back_over_hang = car_model.back_over_hang;
    if (direction == 0) {
      front_over_hang += 0.2;
    } else if(direction == 1) {
      back_over_hang += 0.2;
    } else {
      back_over_hang = back_over_hang;
      front_over_hang = front_over_hang;
    }
    if(half == 1)
    {
      std::tie(p, q) = carmodel_left(tp,
                                          poly,
                                          front_over_hang,
                                          back_over_hang,
                                          car_model.half_wheel);      
    }
    else if(half == 2)
    {
      std::tie(p, q) = carmodel_right(tp,
                                          poly,
                                          front_over_hang,
                                          back_over_hang,
                                          car_model.half_wheel);        
    }
    else
    {
      std::tie(p, q) = carmodel(tp,
                                          poly,
                                          front_over_hang,
                                          back_over_hang,
                                          car_model.half_wheel + 0.1);        
    }


    auto ha = obs.lower_bound(p.x);
    if (!obs.count(ha->first) || ha->first > q.x ) {
      return false;
    }
    for (; ha->first <= q.x && ha != obs.end(); ++ha) {
      float temp = ha->first;
      auto hi = obs[temp].lower_bound(p);
      if(hi == obs[temp].end()){
        //std::cout << "hi is end()" << std::endl;
        //std::cout << "hi is end()" << std::endl;
        //std::cout << "hi is end()" << std::endl;
        //return false;
        continue;
      }
      if (!obs[temp].count(*hi) || (*hi).y > q.y ) {
        // return false;
        continue;
      }
      for (; (*hi).y <= q.y && hi != obs[ha->first].end(); ++hi) {
        if (geometry::CheckPointInPolygon<ara::adsfi::MsgPoint3D>(*hi, poly)) {
          collision_point.x = hi->x;
          collision_point.y = hi->y;
          collision_point.z = hi->z;
          // AWARN << "collision_point: " << collision_point.x << "," << collision_point.y;
          return true;
        }
      }

    }
    return false;
  }

static std::tuple<ara::adsfi::MsgPoint3D, ara::adsfi::MsgPoint3D> carmodel_left(
      ara::adsfi::MsgPoint3D ego, ara::adsfi::MsgPoint3DVec &polygon,
      float headCarLength, float tailCarLength, float halfWheelTrack) 
{
  polygon.clear();
  ara::adsfi::MsgPoint3D pt, ld, ru;
  std::set<float> cmpx, cmpy;
  float vyaw = ego.z + M_PI / 2.0;
  pt.x = ego.x + headCarLength * cos(ego.z);
  pt.y = ego.y + headCarLength * sin(ego.z);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x + headCarLength * cos(ego.z) + halfWheelTrack * cos(vyaw);
  pt.y = ego.y + headCarLength * sin(ego.z) + halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x - tailCarLength * cos(ego.z) + halfWheelTrack * cos(vyaw);
  pt.y = ego.y - tailCarLength * sin(ego.z) + halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x - tailCarLength * cos(ego.z);
  pt.y = ego.y - tailCarLength * sin(ego.z);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  ld.x = *(cmpx.begin());
  ld.y = *(cmpy.begin());
  ru.x = *(cmpx.rbegin());
  ru.y = *(cmpy.rbegin());
  auto rtn = std::forward_as_tuple(ld, ru);
  return rtn;
}

static std::tuple<ara::adsfi::MsgPoint3D, ara::adsfi::MsgPoint3D> carmodel_right(
      ara::adsfi::MsgPoint3D ego, ara::adsfi::MsgPoint3DVec &polygon,
      float headCarLength, float tailCarLength, float halfWheelTrack) 
{
  polygon.clear();
  ara::adsfi::MsgPoint3D pt, ld, ru;
  std::set<float> cmpx, cmpy;
  float vyaw = ego.z + M_PI / 2.0;
  pt.x = ego.x + headCarLength * cos(ego.z) - halfWheelTrack * cos(vyaw);
  pt.y = ego.y + headCarLength * sin(ego.z) - halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x + headCarLength * cos(ego.z);
  pt.y = ego.y + headCarLength * sin(ego.z);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x - tailCarLength * cos(ego.z);
  pt.y = ego.y - tailCarLength * sin(ego.z);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x - tailCarLength * cos(ego.z) - halfWheelTrack * cos(vyaw);
  pt.y = ego.y - tailCarLength * sin(ego.z) - halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  ld.x = *(cmpx.begin());
  ld.y = *(cmpy.begin());
  ru.x = *(cmpx.rbegin());
  ru.y = *(cmpy.rbegin());
  auto rtn = std::forward_as_tuple(ld, ru);
  return rtn;
}

static std::tuple<ara::adsfi::MsgPoint3D, ara::adsfi::MsgPoint3D> carmodel(
  ara::adsfi::MsgPoint3D ego, ara::adsfi::MsgPoint3DVec &polygon,
  float headCarLength, float tailCarLength, float halfWheelTrack) {
  polygon.clear();
  ara::adsfi::MsgPoint3D pt, ld, ru;
  std::set<float> cmpx, cmpy;
  float vyaw = ego.z + M_PI / 2.0;
  pt.x = ego.x + headCarLength * cos(ego.z) - halfWheelTrack * cos(vyaw);
  pt.y = ego.y + headCarLength * sin(ego.z) - halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x + headCarLength * cos(ego.z) + halfWheelTrack * cos(vyaw);
  pt.y = ego.y + headCarLength * sin(ego.z) + halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x - tailCarLength * cos(ego.z) + halfWheelTrack * cos(vyaw);
  pt.y = ego.y - tailCarLength * sin(ego.z) + halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  pt.x = ego.x - tailCarLength * cos(ego.z) - halfWheelTrack * cos(vyaw);
  pt.y = ego.y - tailCarLength * sin(ego.z) - halfWheelTrack * sin(vyaw);
  // std::cout << "carmode::" << pt.x << "," << pt.y << std::endl;
  cmpx.insert(pt.x);
  cmpy.insert(pt.y);
  polygon.push_back(pt);
  ld.x = *(cmpx.begin());
  ld.y = *(cmpy.begin());
  ru.x = *(cmpx.rbegin());
  ru.y = *(cmpy.rbegin());
  auto rtn = std::forward_as_tuple(ld, ru);
  return rtn;
}


  template <class T>
  static bool CollisionDetection(const std::vector<T> &point_vec,
                                 SortedTree &obs,
                                 const avos::planning::CarModel &car_model,
                                 ara::adsfi::MsgPoint3D &collision_point,
                                 ara::adsfi::MsgPoint3D &path_collision_point,
                                 double valid_dis = numeric_limits<double>::max()) {

    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    double dis = 0;
    auto it = point_vec.begin();
    int counter = 0;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      counter++;

      if (dis > valid_dis) break;
      if (counter % COLLISION_INNER == 0) {
        if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, car_model, 0, collision_point)) {
          path_collision_point.x = it->x;
          path_collision_point.y = it->y;
          // std::cout << "collision::" << obs.size() << std::endl;
          return true;
        }
      }

    }
    path_collision_point.x = -888.0;
    path_collision_point.y = -888.0;
    return false;
  }

  template <class T>
  static double GetPointCollisionDistance(const T &point) {
    auto DP = avos::planning::DataPool::Instance()->GetMainDataPtr();
    VoronoiDiagramInfo voronoi_diagram_info;
    DP->GetVoronoiDiagramInfo(voronoi_diagram_info);
    double voronoi_front_length = voronoi_diagram_info.front_length;
    double voronoi_back_length = voronoi_diagram_info.back_length;
    double voronoi_resolution = voronoi_diagram_info.resolution;
    double voronoi_width = voronoi_diagram_info.width;
    std::vector<double> voronoi_diagram = voronoi_diagram_info.voronoi_diagram;
    double dis = 888;

    int sizeX = (2*voronoi_width)/voronoi_resolution+1, sizeY = (voronoi_front_length+voronoi_back_length)/voronoi_resolution+1
    , count_row = (2*voronoi_width)/voronoi_resolution+1;

    if(voronoi_diagram.size() < sizeX*sizeY) {
      dis = 999;
      return dis;
    }

    if(point.x > voronoi_front_length || point.x < ((-1)*voronoi_back_length) || std::fabs(point.y) > voronoi_width) {
      return dis;
    }

    int index_i = int(std::fabs(point.x + voronoi_back_length)/ voronoi_resolution);
    int index_j = int(std::fabs(voronoi_width - point.y)/ voronoi_resolution);
    dis = voronoi_diagram.at(index_i * count_row + index_j);

    return dis;
  }

    template <class T>
  static std::pair<double, double> GetPointCollision(const T &point) {
    auto DP = avos::planning::DataPool::Instance()->GetMainDataPtr();
    VoronoiDiagramInfo voronoi_diagram_info;
    DP->GetVoronoiDiagramInfo(voronoi_diagram_info);
    double voronoi_front_length = voronoi_diagram_info.front_length;
    double voronoi_back_length = voronoi_diagram_info.back_length;
    double voronoi_resolution = voronoi_diagram_info.resolution;
    double voronoi_width = voronoi_diagram_info.width;
    std::vector<std::pair<double, double>> voronoi_diagram_obj_coor = voronoi_diagram_info.voronoi_diagram_obj_coor;
    std::pair<double, double> obj_point(-1.0, -1.0);

    int sizeX = (2*voronoi_width)/voronoi_resolution+1, sizeY = (voronoi_front_length+voronoi_back_length)/voronoi_resolution+1
    , count_row = (2*voronoi_width)/voronoi_resolution+1;

    if(voronoi_diagram_obj_coor.size() < sizeX*sizeY) {
      return obj_point;
    }

    if(point.x > voronoi_front_length || point.x < ((-1)*voronoi_back_length) || std::fabs(point.y) > voronoi_width) {
      return obj_point;
    }

    int index_i = int(std::fabs(point.x + voronoi_back_length)/ voronoi_resolution);
    int index_j = int(std::fabs(voronoi_width - point.y)/ voronoi_resolution);
    obj_point = voronoi_diagram_obj_coor.at(index_i * count_row + index_j);

    return obj_point;
  }


//   static bool CollisionDetection(const geometry::Pose &point,
//                                  const double &expand_r) {
//     auto DP = avos::planning::DataPool::Instance()->GetMainDataPtr();
//     CoordTransform *coordtransform = CoordTransform::Instance();
//     std::vector<geometry::CircularPoint> raw_car_circles = DP->config_info.car_model.model_circles;
//     VoronoiDiagramInfo voronoi_diagram_info;
//     DP->GetVoronoiDiagramInfo(voronoi_diagram_info);
//     double voronoi_front_length = voronoi_diagram_info.front_length;
//     double voronoi_back_length = voronoi_diagram_info.back_length;
//     double voronoi_resolution = voronoi_diagram_info.resolution;
//     double voronoi_width = voronoi_diagram_info.width;
//     std::vector<double> voronoi_diagram = voronoi_diagram_info.voronoi_diagram;

//     int sizeX = (2*voronoi_width)/voronoi_resolution+1, sizeY = (voronoi_front_length+voronoi_back_length)/voronoi_resolution+1
//     , count_row = (2*voronoi_width)/voronoi_resolution+1;
//     if(voronoi_diagram.size() < sizeX*sizeY) {
//       return false;
//     }

//     if(point.position.x > voronoi_front_length || point.position.x < ((-1)*voronoi_back_length) || point.position.y > voronoi_width) {
//       return false;
//     }

//     for(int i=0; i<raw_car_circles.size(); i++){
//       geometry::Site ego, pos, gccs;
//       geometry::CircularPoint tem_circle_point;
//       ego.xg = point.position.x;
//       ego.yg = point.position.y;
//       ego.globalangle = point.angle;
//       pos.x = raw_car_circles.at(i).position.x;
//       pos.y = raw_car_circles.at(i).position.y;
//       coordtransform->VCS2GCCS(ego, pos, gccs);
//       tem_circle_point.position.x = gccs.xg;
//       tem_circle_point.position.y = gccs.yg;
//       tem_circle_point.radius = raw_car_circles.at(i).radius + expand_r;

//       if(tem_circle_point.position.x > voronoi_front_length || tem_circle_point.position.x < ((-1)*voronoi_back_length) 
//         || std::fabs(tem_circle_point.position.y) > voronoi_width) {
//         return false;
//       }

//       int index_i = int(std::fabs(tem_circle_point.position.x + voronoi_back_length)/ voronoi_resolution);
//       int index_j = int(std::fabs(voronoi_width - tem_circle_point.position.y)/ voronoi_resolution);
//       double dis = voronoi_diagram.at(index_i * count_row + index_j);
//       if(dis < tem_circle_point.radius) return true;

//     }

//   return false;
// }


  // static bool CollisionDetection(const std::vector<geometry::Pose> &point_vec,
  //                                const double &expand_r,
  //                                ara::adsfi::MsgPoint3D &path_collision_point,
  //                                double valid_dis = numeric_limits<double>::max()) {
  //   if (point_vec.size() == 0 || point_vec.size() == 1) return false;
  //   double dis = 0;
  //   auto it = point_vec.begin();
  //   int counter = 0;
  //   for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
  //     double temp = std::hypot(it->position.x - std::next(it, 1)->position.x , it->position.y - std::next(it, 1)->position.y);
  //     dis += temp;
  //     counter++;

  //     if (dis > valid_dis) break;
  //     if (counter % COLLISION_INNER == 0) {
  //       if (ObjectCollisionDetect::CollisionDetection(*it, expand_r)) {
  //         path_collision_point.x = it->position.x;
  //         path_collision_point.y = it->position.y;
  //         // std::cout << "collision::" << obs.size() << std::endl;
  //         return true;
  //       }
  //     }

  //   }
  //   path_collision_point.x = -888.0;
  //   path_collision_point.y = -888.0;
  //   return false;
  // }

  template <class T>
  static bool AdapterCollisionDetection(const std::vector<T> &point_vec,
                                        SortedTree &obs,
                                        const avos::planning::CarModel &car_model,
                                        ara::adsfi::MsgPoint3D &collision_point,
                                        double valid_dis = numeric_limits<double>::max()) {

    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    double dis = 0;
    avos::planning::CarModel cur_model;
    cur_model = car_model;
    auto it = point_vec.begin();
    int counter = 0;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      counter++;
      if (dis > valid_dis) break;
      float v = it->linear_velocity;
      float w = it->angular_velocity;
      double curvature = fabs(v) < 1e-2 ? 0.0 : fabs(w / v);
      double offset = 0.05;
      if (curvature < 0.0) {
        offset = 0.05;
      } else if (curvature < 0.005) {
        offset = 0.05;
      } else if (curvature < 0.02) {
        offset = 0.1;
      } else if (curvature < 0.1) {
        offset = 0.15;
      } else if (curvature < 0.2) {
        offset = 0.2;
      } else if (curvature < 0.5) {
        offset = 0.25;;
      } else if (curvature < 0.667) {
        offset = 0.35;
      } else {
        offset = 0.4;
      }
      double square_xy = it->x * it->x + it->y * it->y;
      if (square_xy < 9.0) {
        offset = 0.0;
      }
      cur_model.half_wheel = car_model.half_wheel + offset;
      if (counter % COLLISION_INNER == 0) {
        if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, cur_model, 0, collision_point)) {
          // std::cout << "collision::" << obs.size() << std::endl;
          return true;
        }
      }
    }
    return false;
  }

  template <class T, class ContainerT>
  static bool RoadCollisionDetection_dis(const ContainerT &point_vec,
                                         double& collid_dis,
                                         SortedTree &obs,
                                         const avos::planning::CarModel &car_model,
                                         ara::adsfi::MsgPoint3D &collision_point,
                                         double& collid_height,
                                         double valid_dis = numeric_limits<double>::max(),
                                         int half = 0,
                                         const float& delta_dis_threshold = 0.5) {

    // AWARN << "RoadCollisionDetection " << point_vec.size();
    collid_dis = numeric_limits<double>::max();
    collid_height = 0.0;
    int count = 0;
    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    int obs_cell_size = 0;
    for (auto it = obs.begin(); it != obs.end(); it++) {
      obs_cell_size += it->second.size();
    }
    if(obs_cell_size == 0) return false;
    double dis = 0, delta_dis = 0;
    auto it = point_vec.begin();
    int counter = 0;
    bool valid_dis_satisfied = false;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      //AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      delta_dis += temp;
      if(delta_dis < delta_dis_threshold){
        continue;
      }else{
        delta_dis = 0;
      }
      count++;
      counter++;
      if (dis > valid_dis){
        valid_dis_satisfied = true;
        break;
      } 
      // if (count < 25) continue;
      if (counter % COLLISION_INNER == 0) {
        // int pt_direction = it->reverse ? 1 : 0;
        int pt_direction = -1;
        if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, car_model, pt_direction, collision_point, half)) {
          // AWARN << "RoadCollisionDetection out index|x|y " << count << "|" << it->x << "|" << it->y;
          //AWARN << "object collision " << count << "|" << collision_point.x << "|" << collision_point.y;
          // if(std::hypot(it->x, it->y) < 0.5) continue;
          collid_dis = dis;
          collid_height = collision_point.z; 
          collision_point.z = count;  //for index  xy
          return true;
        }
      }
    }
    // AINFO<<"valid_dis_satisfied: "<<valid_dis_satisfied<<" it != point_vec.end() "<<(it != point_vec.end());
    if(!valid_dis_satisfied && it != point_vec.end()){
      //补充最后一个点的检测
      // AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      // int pt_direction = it->reverse ? 1 : 0;
      int pt_direction = -1;
      if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, car_model, pt_direction, collision_point, half)) {
        collid_dis = dis;//近似
        return true;
      }
    }

    // AWARN << "RoadCollisionDetection out" ;
    return false;
  }
  template <class T, class ContainerT>
  static bool RoadCollisionDetection_expan(const ContainerT &point_vec,
                                         double& collid_dis,
                                         SortedTree &obs,
                                         const avos::planning::CarModel &car_model,
                                         ara::adsfi::MsgPoint3D &collision_point,
                                         double& collid_height,
                                         double valid_dis = numeric_limits<double>::max(),
                                         bool no_expan = false,
                                         bool high_speed_avoid = false,
                                         double vehicle_expan_dis = 0.0) {

    //AWARN << "RoadCollisionDetection_expan " << point_vec.size();
    collid_dis = numeric_limits<double>::max();
    collid_height = 0.0;
    int count = 0;
    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    double dis = 0;
    auto it = point_vec.begin();
    int counter = 0;
    bool valid_dis_satisfied = false;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      //AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      count++;
      counter++;
      if (dis > valid_dis){
        valid_dis_satisfied = true;
        break;
      } 
      // if (count < 25) continue;
      if (counter % COLLISION_INNER == 0) {
        // int pt_direction = it->reverse ? 1 : 0;
        int pt_direction = -1;
        avos::planning::CarModel var_car_model = car_model;
        
        if(high_speed_avoid)
        {
          var_car_model.half_wheel += 0.02*std::min(6.0,std::hypot(it->x,it->y)) + vehicle_expan_dis;
          if(std::hypot(it->x,it->y) > 6.0) var_car_model.half_wheel -= 0.1;
        }
        else if(std::hypot(it->x,it->y) > 2.0)
        {
          var_car_model.half_wheel += 0.1+0.01*std::min(30.0,std::floor(std::hypot(it->x,it->y)/10.0)*10.0+10.0);
        }
        else
        {
          var_car_model.half_wheel -= 0.1;
        }
        if (no_expan && ObjectCollisionDetect::CollisionDetection<T>(*it, obs, car_model, pt_direction, collision_point, 0)) {
          // AWARN << "RoadCollisionDetection out index|x|y " << count << "|" << it->x << "|" << it->y;
          //AWARN << "object collision " << count << "|" << collision_point.x << "|" << collision_point.y;
          // if(std::hypot(it->x, it->y) < 0.5) continue;
          collid_dis = dis;
          collid_height = collision_point.z; 
          collision_point.z = count;  //for index  xy
          return true;
        }
        else if((!no_expan || high_speed_avoid) && ObjectCollisionDetect::CollisionDetection<T>(*it, obs, var_car_model, pt_direction, collision_point, 0)) 
        {
          // AWARN << "RoadCollisionDetection out index|x|y " << count << "|" << it->x << "|" << it->y;
          //AWARN << "object collision " << count << "|" << collision_point.x << "|" << collision_point.y;
          // if(std::hypot(it->x, it->y) < 0.5) continue;
          collid_dis = dis;
          collid_height = collision_point.z; 
          collision_point.z = count;  //for index  xy
          return true;
        }
        else if(false && high_speed_avoid && ObjectCollisionDetect::CollisionDetection<T>(*it, obs, var_car_model, pt_direction, collision_point, 0))
        {
          collid_dis = dis;
          collid_height = collision_point.z; 
          collision_point.z = count;  //for index  xy
          return true; 
        }         
      }
    }
    // AINFO<<"valid_dis_satisfied: "<<valid_dis_satisfied<<" it != point_vec.end() "<<(it != point_vec.end());
    if(!valid_dis_satisfied && it != point_vec.end()){
      //补充最后一个点的检测
      // AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      // int pt_direction = it->reverse ? 1 : 0;
      int pt_direction = -1;
      if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, car_model, pt_direction, collision_point, 0)) {
        collid_dis = dis;//近似
        return true;
      }
    }

    // AWARN << "RoadCollisionDetection out" ;
    return false;
  }  

  template <class T, class ContainerT>
  static bool RoadCollisionDetection_near(const ContainerT &point_vec,
                                         double& collid_dis,
                                         SortedTree &obs,
                                         const avos::planning::CarModel &car_model,
                                         ara::adsfi::MsgPoint3D &collision_point,
                                         double& collid_height,
                                         double valid_dis = numeric_limits<double>::max(),
                                         double check_length = 6.0,
                                         double half_wheel_expan = 0.3) {

    //AWARN << "RoadCollisionDetection_expan " << point_vec.size();
    collid_dis = numeric_limits<double>::max();
    collid_height = 0.0;
    int count = 0;
    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    double dis = 0;
    auto it = point_vec.begin();
    int counter = 0;
    bool valid_dis_satisfied = false;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      //AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      count++;
      counter++;
      if (dis > valid_dis){
        valid_dis_satisfied = true;
        break;
      } 
      // if (count < 25) continue;
      if (counter % COLLISION_INNER == 0) {
        // int pt_direction = it->reverse ? 1 : 0;
        int pt_direction = -1;
        avos::planning::CarModel var_car_model = car_model;
        
        if(std::hypot(it->x,it->y) < check_length)
        {
          var_car_model.half_wheel += half_wheel_expan;
        }
        else
        {
          return false;
        }
        if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, var_car_model, pt_direction, collision_point, 0)) {
          // AWARN << "RoadCollisionDetection out index|x|y " << count << "|" << it->x << "|" << it->y;
          //AWARN << "object collision " << count << "|" << collision_point.x << "|" << collision_point.y;
          // if(std::hypot(it->x, it->y) < 0.5) continue;
          collid_dis = dis;
          collid_height = collision_point.z; 
          collision_point.z = count;  //for index  xy
          return true;
        }        
      }
    }
    // AINFO<<"valid_dis_satisfied: "<<valid_dis_satisfied<<" it != point_vec.end() "<<(it != point_vec.end());
    if(!valid_dis_satisfied && it != point_vec.end()){
      //补充最后一个点的检测
      // AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      // int pt_direction = it->reverse ? 1 : 0;
      int pt_direction = -1;
      if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, car_model, pt_direction, collision_point, 0)) {
        collid_dis = dis;//近似
        return true;
      }
    }

    // AWARN << "RoadCollisionDetection out" ;
    return false;
  } 
  template <class T, class ContainerT>
  static bool RoadAdapterCollisionDetection_dis(const ContainerT &point_vec,
                                                double &collid_dis,
                                                SortedTree &obs,
                                                const avos::planning::CarModel &car_model,
                                                ara::adsfi::MsgPoint3D &collision_point,
                                                double& collid_height,
                                                double valid_dis = numeric_limits<double>::max()) {

    // AWARN << "RoadAdapterCollisionDetection " << point_vec.size();
    collid_dis = numeric_limits<double>::max();
    collid_height = 0.0;
    int count = 0;
    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    double dis = 0;
    avos::planning::CarModel cur_model;
    cur_model = car_model;
    auto it = point_vec.begin();
    int counter = 0;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      // AINFO << "ROAD POINT x|y  " << it->x << "|" << it->y << "|" << it->angle << "|" << it->globalangle;
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      count++;
      counter++;
      if (dis > valid_dis) break;
      // if (count < 25) continue;
      cur_model.half_wheel = car_model.half_wheel + it->velocity;
      if (counter % COLLISION_INNER == 0) {
        int pt_direction = it->reverse ? 1 : 0;
        if (ObjectCollisionDetect::CollisionDetection<T>(*it, obs, cur_model, pt_direction, collision_point)) {
          // AWARN << "RoadAdapterCollisionDetection out index|x|y " << count << "|" << it->x << "|" << it->y;
          // AWARN << "object collision " << count << "|" << collision_point.x << "|" << collision_point.y;
          collid_dis = dis;
          collid_height = collision_point.z;
          collision_point.z = count;  //for index  xy
          return true;
        }
      }
    }

    // AWARN << "RoadAdapterCollisionDetection out" ;
    return false;
  }

  static bool RoadCollisionCheck(const std::list<geometry::Site> &point_vec,
                                 SortedTree &obs,
                                 const avos::planning::CarModel &car_model,
                                 const double &valid_dis,
                                 CollisionInfo &collision_info,
                                 const int flag = 0,
                                 const double& delta_dis_threshold = 1.0) {

    // AWARN << "RoadAdapterCollisionDetection " << point_vec.size();
    collision_info.Reset();
    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
    int obs_cell_size = 0;
    for (auto it = obs.begin(); it != obs.end(); it++) {
      obs_cell_size += it->second.size();
    }
    if(obs_cell_size == 0) return false;
    int counter = 0;
    double dis = 0, delta_dis = 0;
    auto it = point_vec.begin();
    ara::adsfi::MsgPoint3D collision_point;
    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
      if (dis > valid_dis) break;
      dis += std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      delta_dis += std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      counter++;
      if(delta_dis < delta_dis_threshold){
        continue;
      }else{
        delta_dis = 0;
      }
      int pt_direction = it->reverse ? 1 : 0;
      
      avos::planning::CarModel var_car_model = car_model;
      if(false && flag == 1) {
        var_car_model.half_wheel += 0.0+0.017*std::max(0.0,std::floor(std::hypot(it->x,it->y)/6.0)*(-1.0*6.0)+6.0);
      }
      if (ObjectCollisionDetect::CollisionDetection<geometry::Site>(*it, obs, var_car_model, pt_direction, collision_point)) {
        collision_info.path_pt_info = *it;
        collision_info.path_pt_info.index = counter;
        collision_info.path_pt_info.length = dis;
        collision_info.cell_info.x = collision_point.x;
        collision_info.cell_info.y = collision_point.y;
        collision_info.cell_info.height = collision_point.z;
        return true;
      }
    }

    if(it != point_vec.end()){
      //补充最后一个点的检测
      avos::planning::CarModel var_car_model = car_model;
      int pt_direction = it->reverse ? 1 : 0;
      if (ObjectCollisionDetect::CollisionDetection<geometry::Site>(*it, obs, var_car_model, pt_direction, collision_point)) {
          collision_info.path_pt_info = *it;
          collision_info.path_pt_info.index = ++counter;
          collision_info.path_pt_info.length = dis;
          collision_info.cell_info.x = collision_point.x;
          collision_info.cell_info.y = collision_point.y;
          collision_info.cell_info.height = collision_point.z;
          return true;
        }
    }
    return false;
  }

 // static bool RoadCollisionCheck(const std::list<geometry::Site> &point_vec,
 //                                 SortedTree &obs,
 //                                 const avos::planning::CarModel &car_model,
 //                                 const double &valid_dis,
 //                                 CollisionInfo &collision_info,
 //                                 const int flag = 0) {

 //    // AWARN << "RoadAdapterCollisionDetection " << point_vec.size();
 //    collision_info.Reset();
 //    if (point_vec.size() == 0 || point_vec.size() == 1) return false;
 //    int counter = 0;
 //    double dis = 0;
 //    auto it = point_vec.begin();
 //    geometry_msgs::Point32 collision_point;
 //    int flag_end = 0;
 //    for (; it != point_vec.end() && std::next(it, 1) != point_vec.end(); ++it) {
 //        if (dis > valid_dis) {
 //            flag_end = 2;
 //            break;
 //        }
 //      dis += std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
 //      counter++;
 //      int pt_direction = it->reverse ? 1 : 0;
      
 //      avos::planning::CarModel var_car_model = car_model;
 //      if(false && flag == 1) {
 //        var_car_model.half_wheel += 0.0+0.017*std::max(0.0,std::floor(std::hypot(it->x,it->y)/6.0)*(-1.0*6.0)+6.0);
 //      }
 //      if (counter % 5 == 1) {
 //        if(counter == point_vec.size()) {
 //          flag_end = 1;
 //        }
 //          if (ObjectCollisionDetect::CollisionDetection<geometry::Site>(
 //                  *it, obs, var_car_model, pt_direction, collision_point)) {
 //              collision_info.path_pt_info = *it;
 //              collision_info.path_pt_info.index = counter;
 //              collision_info.path_pt_info.length = dis;
 //              collision_info.cell_info.x = collision_point.x;
 //              collision_info.cell_info.y = collision_point.y;
 //              collision_info.cell_info.height = collision_point.z;
 //              return true;
 //          }
 //      }
 //    }
 //    if (flag_end == 0) {
 //      int pt_direction = std::prev(it)->reverse ? 1 : 0;
      
 //      avos::planning::CarModel var_car_model = car_model;
 //      if(false && flag == 1) {
 //        var_car_model.half_wheel += 0.0+0.017*std::max(0.0,std::floor(std::hypot(std::prev(it)->x,std::prev(it)->y)/6.0)*(-1.0*6.0)+6.0);
 //      }
 //        if (ObjectCollisionDetect::CollisionDetection<geometry::Site>(
 //                *std::prev(it), obs, var_car_model, pt_direction, collision_point)) {
 //            collision_info.path_pt_info = *std::prev(it);
 //            collision_info.path_pt_info.index = counter;
 //            collision_info.path_pt_info.length = dis;
 //            collision_info.cell_info.x = collision_point.x;
 //            collision_info.cell_info.y = collision_point.y;
 //            collision_info.cell_info.height = collision_point.z;
 //            return true;
 //        }
 //    }
 //    return false;
 //  }

  // static CollisionInfoVec CollisionDetectionVec(const std::list<geometry::Site> &ego,
  //     SortedTree &obspts, VehicleElem &vehiclemodel) {
  //   CollisionInfoVec collision_info_vec;
  //   if (ego.size() == 0) {
  //     return collision_info_vec;
  //   }
  //   ara::adsfi::MsgPoint3D collision_cell;
  //   ara::adsfi::MsgPoint3D no_collision;
  //   ara::adsfi::MsgPoint3D p, q, tp, tp0;
  //   ara::adsfi::MsgPoint3DVec poly, poly0;
  //   collision_cell.x = __DBL_MAX__;
  //   collision_cell.y = __DBL_MAX__;
  //   collision_cell.z = __DBL_MAX__; // used for motion to record collision point index at path.
  //   tp0.x = ego.front().x;
  //   tp0.y = ego.front().y;
  //   tp0.z = ego.front().angle * M_PI / 180.0;
  //   no_collision = collision_cell;
  //   double test_dis = 0.0;
  //   geometry::carmodel(tp0, poly0, vehiclemodel.frontoverhang,
  //                      vehiclemodel.backoverhang,
  //                      vehiclemodel.halfwheeltrack);
  //   int i = 0;
  //   int counter = 0;
  //   for (auto it = ego.begin(); it != ego.end() &&
  //        std::next(it, 1) != ego.end(); ++it) {
  //     ++i;
  //     counter++;
  //     // size_t i = 0; i + 1 < ego.size(); i += 4) {
  //     tp.x = it->x;
  //     tp.y = it->y;
  //     tp.z = it->angle * M_PI / 180.0;
  //     double point_dis = std::hypot(tp.x - std::next(it, 1)->x,
  //                                   tp.y - std::next(it, 1)->y);
  //     test_dis += point_dis;
  //     if (test_dis > 20.0) break;

  //     if (counter % COLLISION_INNER == 0) {
  //       continue;
  //     }
  //     std::tie(p, q) = geometry::carmodel(tp, poly, vehiclemodel.frontoverhang,
  //                                         vehiclemodel.backoverhang,
  //                                         vehiclemodel.halfwheeltrack);
  //     auto ha = obspts.lower_bound(p.x);

  //     if (!obspts.count(ha->first) || ha->first > q.x) {
  //       // continue;
  //     } else {
  //       for (; ha->first <= q.x && ha != obspts.end(); ++ha) {
  //         auto hi = obspts[ha->first].lower_bound(p);
  //         if (!obspts[ha->first].count(*hi) || (*hi).y > q.y) {
  //           // continue;
  //         } else {
  //           for (; (*hi).y <= q.y && hi != obspts[ha->first].end(); ++hi) {
  //             if (geometry::CheckPointInPolygon<ara::adsfi::MsgPoint3D>(*hi, poly)) {
  //               if ( geometry::CheckPointInPolygon<ara::adsfi::MsgPoint3D>(*hi, poly0)) continue;
  //               collision_cell.x = hi->x;
  //               collision_cell.y = hi->y;
  //               collision_cell.z = static_cast<float>(i);// used for motion to record collision point index in path.
  //               // ROS_WARN_STREAM("[dwaplanner] collide id "<<collision_cell.z
  //               //   <<"\tcollide cell.x "<<collision_cell.x<<"\tcollide cell.y "<<collision_cell.y);
  //               collision_info_vec.emplace_back(make_tuple(1, collision_cell));
  //               return collision_info_vec;
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  //   return collision_info_vec;
  // }

  template <class T>
  static bool CollisionDetection(const T& point, const avos::planning::CarModel& car_model,
                                 avos::planning::UnstructEnv& perception_data) {
      int rtvalue = false;    
      ara::adsfi::MsgPoint3D collision_cell_obs;  

      bool result_obs = CollisionDetection<T>(point,
                          perception_data.dynamic_info.static_obj,
                          car_model,
                          -1,
                          collision_cell_obs);
      if(!result_obs)
      {
        result_obs = CollisionDetection<T>(point,
                          perception_data.dynamic_info.dynamic_obj,
                          car_model,
                          -1,
                          collision_cell_obs);   

        if(!result_obs)
        {
          result_obs = CollisionDetection<T>(point,
                          perception_data.dynamic_info.static_lidar_cell,
                          car_model,
                          -1,
                          collision_cell_obs);
          if(!result_obs)
          {
            result_obs = CollisionDetection<T>(point,
                          perception_data.dynamic_info.static_ultrasonic_cell,
                          car_model,
                          -1,
                          collision_cell_obs);
            // if(!result_obs)
            // {
            //   result_obs = ObjectCollisionDetect::CollisionDetection<T>(point,
            //                  perception_data.dynamic_info.predicted_obj,
            //                  car_model,
            //                  -1,
            //                  collision_cell_obs);
            // }
          }
        }
      } 
    rtvalue = result_obs;
    return rtvalue;
  }

  template <class T>
  static bool CollisionDetection(const std::vector<T>& point_vec, const avos::planning::CarModel& car_model,
                                 avos::planning::UnstructEnv& perception_data) {
      if (point_vec.size() < 2)
          return false;
      auto it = point_vec.begin();
      int counter = 0;
      for (; it != point_vec.end() && std::next(it) != point_vec.end(); ++it) {
          counter++;
          if (counter % COLLISION_INNER == 0) {
              if (ObjectCollisionDetect::CollisionDetection(*it, car_model, perception_data)) {
                  return true;
              }
          }
      }
      return false;
  }
};

#endif  // COMMON_TOOBOX_COLLISION_DETECT_INCLUDE_COLLISION_DETECT_H_
