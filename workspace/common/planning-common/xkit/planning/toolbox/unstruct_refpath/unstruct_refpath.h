#ifndef SRC_EXECUTION_REFPATH_UNSTRUCT_REFPATH_INCLUDE_UNSTRUCT_REFPATH_H_
#define SRC_EXECUTION_REFPATH_UNSTRUCT_REFPATH_INCLUDE_UNSTRUCT_REFPATH_H_

#include "xkit/planning/datapool/common_typedef.h"
#include "xkit/planning/toolbox/geometry/geoheader.h"
#include "macros.h"
#include "xkit/planning/toolbox/math/cgl.h"
#include "xkit/planning/datapool/data_pool.h"


namespace avos{
namespace planning {

class UnstructRefpath {
private:
	UnstructRefpath();

public:
	static UnstructRefpath *Instance() {
		static UnstructRefpath *in = nullptr;
		if (in == nullptr) {
			in = new UnstructRefpath();
		}
		return in;
	}
  ~UnstructRefpath();
  void Reset();
  int InitPath(std::vector<InnerPointInfo> &raw_path, std::string &err_code);
  int InitPathDR(std::vector<InnerPointInfo> &raw_path, std::string &err_code,
               const LocalizationData &loc_data);
  int PathExtraction(const double &speed_limit,
                      const LocalizationData &loc_data,
                     const std::vector<InnerPointInfo> &raw_path,
                     const CarModel &car_model,
                     const bool &is_front,
                     Paths &paths,
                     std::string &err_code,
                     int follow_index_diff = 0,
                     bool follow_avoid_flag = false);//默认参非follw模式下不要传入！ xyk
  int PathExtractionDr(const double &speed_limit,
   		       const LocalizationData &loc_data,
                       const std::vector<InnerPointInfo> &raw_path, const CarModel &car_model,
                       const bool &is_front, Paths &paths, std::string &err_code);
  int WriteCalculatePath(const LocalizationData &loc_data,
                         const std::vector<InnerPointInfo> &raw_path,
                         const PathData &edit_path,
                         std::string &err_code);
 public:
  bool IsAvoid() { return is_avoid_; }
  int GetNearestIndex() { return cur_nearest_index_; }
  int GetNearestOriginIndex() { return cur_nearest_origin_index_; }
  int GetLocalPathIndex() { return local_path_index_; }
  int GetCutPathIndex() { return cut_path_index_; }
  bool GetCurrentReverse() { return cur_reverse_; }
  void SetStructBehaviorStatus(const bool& status){
    enable_struct_behavior_ = status;
  }
 private:
  int CheckRawValid(const std::vector<InnerPointInfo> &raw_path, std::string &err_code);
  int AddRawToLocal(const std::vector<InnerPointInfo> &raw_path, std::string &err_code, int follow_index_diff = 0, bool follow_avoid_flag = false);//默认参非follw模式下不要传入！ xyk
  bool CheckFrontPtExistTurn(const bool &is_front, bool &cur_reverse);
  int GetForwaradAndBackwardPaths(const double &speed_limit,
                                  const bool &is_front,
                                  const bool &cur_reverse,
                                  std::list<geometry::Site> &forward_path,
                                  std::list<geometry::Site> &backward_path,
                                  std::string &err_code);
  int GetForwaradAndBackwardPathsDr(const double &speed_limit,
                                    const bool &is_front, const bool &cur_reverse,
                                    std::list<geometry::Site> &forward_path,
                                    std::list<geometry::Site> &backward_path, std::string &err_code);
  void PathCutting();
  void GetFrontPursuitPath(const PathData &origin_path,
                           const CarModel &car_model,
                           PathData &new_path);
  void GetFrontPursuitPathDr(const PathData &origin_path,
  			     const CarModel &car_model,
  			     PathData &new_path);
  int CalculatePathCheck(const LocalizationData &loc_data,
                         const std::vector<InnerPointInfo> &raw_path,
                         const PathData &edit_path,
                         std::string &err_code);
  void GetRawPath(const std::vector<InnerPointInfo> &raw_path,
                  std::list<geometry::Site> &path);
 private:
  LocalizationData loc_data_;
  std::list<geometry::Site> raw_path_;
  std::list<geometry::Site> local_path_;
  int local_path_index_;
  int cut_path_index_;
  bool is_avoid_;
  bool cur_reverse_;
  int cur_nearest_index_;
  int cur_nearest_origin_index_;
  std::vector<InnerPointInfo> sehs_raw_path_;
  bool enable_struct_behavior_;
};

} // namespace planning
} // namespace avos

#include "unstruct_refpath.hpp"

#endif
