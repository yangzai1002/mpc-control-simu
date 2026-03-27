#ifndef __CARTESIAN_FRENET__
#define __CARTESIAN_FRENET__

#include <cmath>
// #include "base_log/log.h"
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/geometry/dwa_header.h"
#include "xkit/planning/datapool/data_pool.h"

using namespace geometry;
namespace avos{
namespace planning{

class CartesianFrenetConverter {

public:
   void CartesianToFrenet(std::list<geometry::Site>& ref_cartesian_path,
                          const std::list<geometry::Site>& cartesian_path, std::list<geometry::Site>& frenet_path);

   void CartesianToFrenet(const double& rs, const double& rx, const double& ry,
                          const double& rtheta, const double& x, const double& y, double& s,
                          double& l);

   void CartesianToFrenet(std::list<geometry::Site>& ref_cartesian_path, SortedTree& static_obj,
                          SortedTree& dynamic_obj,
                          SortedTree& static_lidar_cell,
                          SortedTree& static_ultrasonic_cell,
                          std::map<double, std::multiset<double>>& frenet_cells);

   void FrenetToCartesian(const std::list<geometry::Site>& ref_cartesian_path,
                          const std::list<geometry::Site>& frenet_path, std::list<geometry::Site>& cartesian_path);

   void FrenetToCartesian(const double& rs, const double& rx, const double& ry,
                          const double& rtheta, const double& s_condition,
                          const double& d_condition, double& x, double& y);
   void GetMatchIterators(std::vector<std::list<geometry::Site>::iterator>& match_iterators) {
      match_iterators = match_iterators_;
   };
private:
   void FindNearestPoint(const std::list<geometry::Site>& path, const double& x, const double& y,
                         std::list<geometry::Site>::iterator& min_dis_iter);

   void RemoveSameS(std::list<geometry::Site>& frenet_path);

private:
   std::vector<std::list<geometry::Site>::iterator> match_iterators_;
};
}
}
#include "cartesian_frenet_conversion.hpp"
#endif
