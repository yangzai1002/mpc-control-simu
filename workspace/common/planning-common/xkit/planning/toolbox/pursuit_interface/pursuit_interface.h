#ifndef SRC_KERNEL_IVPATHPLAN_COMMON_TOOBOX_PURSUIT_INTERFACE_INCLUDE_PURSUIT_INTERFACE_H
#define SRC_KERNEL_IVPATHPLAN_COMMON_TOOBOX_PURSUIT_INTERFACE_INCLUDE_PURSUIT_INTERFACE_H
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/optimizer/pure_pursuit_angle.h"
#include "xkit/planning/toolbox/math/math_common.h"
#include "xkit/planning/toolbox/coordtransform/coordtransform.h"
#include "xkit/planning/datapool/common_config.h"
#include "xkit/planning/datapool/data_pool.h"
// #include "base_log/log.h"
#include <locale>
#include "xkit/planning/toolbox/path_process/path_processing.h"
#include "xkit/planning/datapool/data_pool.h"

class PursuitInterface {
 public:
  PursuitInterface();
  ~PursuitInterface();
  
  static double CalculateOffset(const double &curvature) {
    double offset;
    if (curvature < 0.0) {
      offset = 0.0;
    } else if (curvature < 0.005) {
      offset = 0.05;
    } else if (curvature < 0.05) {
      offset = 0.1;
    } else if (curvature < 0.1) {
      offset = 0.15;
    } else if (curvature < 0.2) {
      offset = 0.2;
    } else if (curvature < 0.5) {
      offset = 0.25;
    } else if (curvature < 0.667) {
      offset = 0.35;
    } else {
      offset = 0.4;
    }
    return offset;
  }

  static int GeneratePurePursuitPath(const std::list<geometry::Site> &orig_path,
                                     const geometry::Site &ego_pos,
                                     const avos::planning::CarModel &car_model,
                                     std::list<geometry::Site> &result_path,
                                     bool pivot_steering = false) {
    auto DP = avos::planning::DataPool::Instance();
    result_path.clear();
    std::vector<geometry::Site> pure_pursuit_path;
    if (orig_path.size() < 3) return -1;
    if (orig_path.front().reverse) return -1;
    int orig_size = orig_path.size();
    pure_pursuit_path.reserve(orig_size);
    double dis = 0.0;
    auto it = orig_path.begin();
    auto bk_it = orig_path.begin();
    for (; it != orig_path.end() && std::next(it, 1) != orig_path.end(); it = std::next(it), bk_it = std::next(bk_it)) {
      double temp = std::hypot(it->x - std::next(it, 1)->x , it->y - std::next(it, 1)->y);
      dis += temp;
      if (dis > 15.0) break;
      if (it->reverse != std::next(it, 1)->reverse) break;
      pure_pursuit_path.push_back(*it);
    }
    if (pure_pursuit_path.size() < 3) return -1;
    double angle = pivot_steering ? 
                   orig_path.front().angle : 0.0;
    geometry::Site cur_loc_vcs(0.0, 0.0, angle);
    auto pp_smoother = std::make_shared<toolbox::optimizer::ppangle::PurePursuitAngle>();
    pp_smoother->Init(cur_loc_vcs,
                      car_model.length_wheelbase,
                      car_model.min_turning_radius+0.3,
                      car_model.length);
    int inter_ind = pp_smoother->DisSmooth(pure_pursuit_path);
    
    // need to calculate the xg, yg, global angle
    avos::planning::CoordTransform *coordtransform = avos::planning::CoordTransform::Instance();
    geometry::Site glob;
    for (auto &p : pure_pursuit_path) {
      coordtransform->VCS2GCCS (ego_pos, p, glob);
      p.xg = glob.xg;
      p.yg = glob.yg;
      p.globalangle = glob.globalangle;
    }
    for (; bk_it != orig_path.end(); bk_it = std::next(bk_it)) {
      pure_pursuit_path.push_back(*bk_it);
    }
    if (!pure_pursuit_path.empty()) {
      result_path.insert(result_path.end(), pure_pursuit_path.begin(), pure_pursuit_path.end());
    }
    auto path_process_ptr = std::make_shared<trackalgorithm::PathProcessing>();
    path_process_ptr->SupplementPath(result_path);
    return inter_ind;
  }

  // set the velocity param as the offset
  static int GetPursuitPath(const std::list<geometry::Site> &orig_path,
                            const geometry::Site &ego_pos,
                            const avos::planning::CarModel &car_model,
                            std::list<geometry::Site> &result_path) {
    size_t ind = GeneratePurePursuitPath(orig_path, ego_pos, car_model, result_path);
    if (ind < 0 || ind >= result_path.size()) {
      std::cout << "Generateoffset error";
      return -1;
    }
    for (auto &p : result_path) {
      p.velocity = CalculateOffset(p.curvature);
    }
    return ind;
  }
};

#endif
