#ifndef DATAPOOL_INCLUDE_PERCEPTION_STRUCT_H_
#define DATAPOOL_INCLUDE_PERCEPTION_STRUCT_H_
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "xkit/planning/toolbox/geometry/geoheader.h"
#include "xkit/planning/datapool/public_typedef.h"
namespace avos {
namespace planning {

enum class PerceptionStatus {
    ALL_ON = 0,
    PERCEPTION_OFF,
    SEMANTICMAP_OFF,
    ALL_OFF
};

enum class eObjectType {
    CAR = 0,
    TRUCK = 1,
    PEDESTRIAN = 2,
    CYCLIST = 3,
    VIRTUAL = 4,
    UNKNOWN = 5
};

typedef struct ObjectCell {
    int idc;
    int type;
    double x;
    double y;
    double xg;
    double yg;
    ObjectCell() {
        idc = 0;
        type = 0;
        x = 0.0;
        y = 0.0;
        xg = 0.0;
        yg = 0.0;
    }
} ObjectCell;

typedef struct SemanticCell {
    int type;        // unsigned cell type, 0-unknow(TODO)
    int box_type;
    int height_mean; // height, unit:cm, -327.68~327.67m.
    int height_min;
    int height_max;
    int conf; // unsigned pass confidence.
    double x;    // relitive pos x, unit: cm, -327.68~327.67m.
    double y;
    double length;
    double width;
    double speed;
    SemanticCell() {
        type = -1;
	box_type = -1;
        height_mean = -1;
        height_min = -1;
        height_max = -1;
        conf = -1;
        x = -1;
        y = -1;
        length = 0;
	width = 0;
	speed = 0.0;
    }

    template <class T>
    void Assaign(const T& cell) {
        type = (int)cell.type;
	box_type = (int)cell.box_type
        height_mean = (int)cell.height_mean;
        height_min = (int)cell.height_min;
        height_max = (int)cell.height_max;
        conf = (int)cell.conf;
        x = (int)cell.x;
        y = (int)cell.y;
	length = (double)cell.length;
	width = (double)cell.width;
        speed = cell.speed;
    }
} SemanticCell;

struct LaneSL {
    double s;
    double l;
    LaneSL() {
        Reset();
    }
    void Reset() {
        s = 0;
        l = 0;
    }
};

typedef struct UltraStaticFeature {
  double angle;
  double half_major_axis_size;
  double half_minor_axis_size;
  double mean_vx;

  UltraStaticFeature() { Reset();}

  void Reset() {
    angle = 0.0;
    half_major_axis_size = 1.0;
    half_minor_axis_size = 0.5;
    mean_vx = 0.0;
  }
} UltraStaticFeature;



typedef struct Object {
    // from perception msgs
    int id;
    double x;
    double y;
    double z;
    double vxrel;
    double vyrel;
    double xabs;
    double yabs;
    double vxabs;
    double vyabs;
    double vsabs; // frenet坐标系下
    double vlabs; // frenet坐标系下
    double width;
    double length;
    double height;
    double v_global_angle;
    double global_angle;
    double speed;
    int type;
    int source;
    double confidence;
    double age; // 未定义，需要与振业协商
    UltraStaticFeature ultra_static_feature;
    std::vector<ObjectCell> cells;
    geometry::SiteVec box_corners;
    // from prediction
    int prediction_type;
    int prediction_index;
    // PredictionObject prediction;
    // from caculation
    double obj_lane_s;
    std::string obj_lane_id;
    std::string enter_lane_id;
    int conflict_type;
    int right_of_way;
    int position_property;
    int acc_type;
    bool is_static;
    double dis_to_junction;
    // StructSLBoundary sl_boundary;
    // StructSTBoundary st_boundary;//根据预测线数量
    std::queue<std::pair<double, double>> history_position;
    Object() {
        cells.reserve(MAX_OBJ_CELL_SIZE);
        Reset();
    }

    friend bool operator<(const Object& obj_l, const Object& obj_r) {
        return (std::hypot(obj_l.x, obj_l.y) <= std::hypot(obj_r.x, obj_r.y));
    }

    bool IsSameIdObject(const Object& obj_) {
        return (obj_.id == id);
    }

    double DistanceToObj(const Object& obj_) {
        if (IsSameIdObject(obj_))
            return 0.0;
        std::cout << "obj: " << obj_.xabs << ", " << obj_.yabs << " last: " << xabs << ", " << yabs
                  << std::endl;
        return std::hypot(obj_.xabs - xabs, obj_.yabs - yabs);
    }

    void Reset() {
        id = 0;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        vxrel = 0.0;
        vyrel = 0.0;
        xabs = 0.0;
        yabs = 0.0;
        vxabs = 0.0;
        vyabs = 0.0;
        width = 0.0;
        length = 0.0;
        height = 0.0;
        v_global_angle = 0.0;
        global_angle = 0.0;
        speed = 0.0;
        type = (int)eObjectType::UNKNOWN;
        source = 0;
        confidence = 0.0;
        age = 0.1;
        cells.reserve(MAX_OBJ_CELL_SIZE);
        cells.clear();
        box_corners.clear();
        prediction_type = 0;
        prediction_index = -1;
        // prediction.Reset();
        conflict_type = 0;
        right_of_way = 1;
        obj_lane_id = "";
        enter_lane_id = "";
        acc_type = 0;
        is_static = true;
        dis_to_junction = 100.0;
        // sl_boundary.Reset();
        // st_boundary.Reset();
    }
    ~Object() {
        cells.clear();
    }
} Object;

struct ObjectHistory {
  std::deque<Object> features;
  // times for calculate motion status(dynamic or static)
  // StatisticsDetailTimes static_times;
  // times for sturn stay time
  // HystereticTimes<std::string> sturn_stay_times;
  
  ObjectHistory() { Reset(); }

  void Reset() {
    features.clear();
    // sturn_stay_times.SetTimes(10, 5, 3);
    // static_times.SetStatisticsTimes(4, 0.6, 0.6);
    // junction_times.SetStatisticsTimes(8, 0.8, 0.8);
  }

  void Update(double& time_stamp) {
    // while (features.size() > 0) {
    //   if (time_stamp - features.front().timestamp > 7.0 ||
    //       features.size() > 70) {
    //     AINFO << "pop id " << features.front().id;
    //     features.pop_front();
    //   } else {
    //     break;
    //   }
    // }
  }

  int Push(const Object& feature) {
    if (features.size() == 0) {
      features.push_back(feature);
      return 0;
    }
    if (features.back().id != feature.id) {
      std::cout << "id is not same , please check " << features.back().id << ", " << feature.id << std::endl;;
      return -1;
    }
    std::cout << "Push id " << feature.id << std::endl;;
    features.push_back(feature);
  }
};

struct PerceptionFrame {
  double timestamp; // 
  double start_timestamp; //
  double end_timestamp; //
  std::unordered_map<int, ObjectHistory> objects;
  
  PerceptionFrame() { Reset(); }
  
  void Reset() {
    timestamp = 0.0;
    start_timestamp = 0.0;
    end_timestamp = 0.0;
    objects.clear();
  }

  int Push(const Object& object) {
    auto iter = objects.find(object.id);
    if (iter == objects.end()) {
      ObjectHistory history;
      history.Push(object);
      objects[object.id] = history;
      return 0;
    }
    return iter->second.Push(object);
  }

  void Update(double& timestamp) {
    for (auto it = objects.begin(); it != objects.end(); ) {
      it->second.Update(timestamp);
      if (it->second.features.size() == 0) {
        it = objects.erase(it);
      } else {
        ++it;
      }
    }
  }
};

} // namespace planning
} // namespace avos

#endif
