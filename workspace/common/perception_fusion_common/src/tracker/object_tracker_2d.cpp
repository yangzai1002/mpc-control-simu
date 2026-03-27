#include "perception_fusion_common/base_tracker/object_tracker_2d.h"
#include "perception_fusion_common/base_tracker/object_filter.h"
#include "perception_fusion_common/common/covariance.h"
#include <Eigen/Dense>
#include <Eigen/Eigen>


namespace avos {
namespace perception {

using namespace cv;
using namespace std;


typedef std::pair<size_t, size_t> MatchPair;


int OTracker2d::tracking_pure_2d_GNN(const vector<TrackObject> &dets, vector<MatchPair> &pairs) {
  pairs.clear();
  if(dets.empty() || trackobj.empty()) return 0;

  int num_det=dets.size();
  int num_trk=trackobj.size();
  vector<vector<float>> distance_matrix;
  distance_matrix.resize(num_det, vector<float>(num_trk));

  for(int i =0;i<num_det;i++) {
    auto &det = dets[i];
    for(int j=0;j<num_trk;j++) {
      auto &trk = trackobj[j];
      distance_matrix[i][j] = 1000;
      if(det.matched || trk.matched) continue;
      float dist = 10000;
      float iou = (float)(det.box2d & trk.box2d).area() / (det.box2d | trk.box2d).area();
      if(iou > 0.3)
        dist = 1000-iou;
      else
        dist = 1000;
      distance_matrix[i][j] = dist;
    }
  }

  while(1) {
    int mini, minj;
    float mind = 1000;
    for(int i=0;i<num_det;i++) {
      for(int j=0;j<num_trk;j++) {
        if(distance_matrix[i][j] < mind) {
          mind = distance_matrix[i][j];
          mini = i;
          minj = j;
        }
      }
    }
    if(mind < 1000) {
      pairs.push_back(MatchPair(mini,minj));
      for(int i=0;i<num_det;i++)
        distance_matrix[i][minj] = 1000;
      for(int j=0;j<num_trk;j++)
        distance_matrix[mini][j] = 1000;
    }
    else
      break;
  }

  return pairs.size();
}




int OTracker2d::obj_tracking_2d(double stamp, const vector<DetObject> &input_dets, const LocationType &locdr, float shake_y, std::vector<OObject> &out_trackobj) {
  Tperiod = stamp - trigger_stamp;
  trigger_stamp = stamp;

  vector<TrackObject> dets;
  for(auto &d: input_dets)
    dets.push_back(d);

  // clear tracked label
  for(auto &trk: trackobj)
    trk.matched = false;
  for(auto &det: dets)
    det.matched = false;

  // 预测
  for(auto &trk: trackobj)
      trk.of_predect_2d(shake_y);

  // 图像2d跟踪
  vector<MatchPair> pairs;
  // if((camid == 2 || camid == 5 || camid == 3 || camid == 4) && fabs(locdr.yawrate) < 6) {
    if(tracking_pure_2d_GNN(dets, pairs) > 0)
      for(auto &p: pairs)
        trackobj[p.second].of_update_2d(dets[p.first]);
  // }

  // 删掉没有跟踪上的
  if(!trackobj.empty()) {
    for(int i=trackobj.size()-1;i>=0;i--) {
      auto &trk = trackobj[i];
      if(trk.matched)
        continue;
      trk.of_fake_update_2d();
      if(trk.trk_guess_age > 1)
        trackobj.erase(trackobj.begin()+i);
    }
  }


  // create new trk
  for(auto &det: dets) {
    if(det.matched == false && (det.conf2d >= 0.3 || det.conf3d > 0.4)) {
      idpool++;
      if(idpool >= 1000*(camid+1)) idpool = camid*1000;
      // idpool 检查与其他Visiontracker的是否重复
      while(1) {
        bool OK = true;
        for(auto &trk: trackobj) {
          if(trk.trk_id == idpool) {
            OK = false;
            idpool++;
            if(idpool >= 1000*(camid+1)) idpool = camid*1000;
            break;
          }
        }
        if(OK) break;
      }
      OObject obj = det;
      obj.trk_id = idpool;
      obj.pos_source = PosSource::VISION;
      trackobj.push_back(obj);
      trackobj[trackobj.size()-1].of_update_2d(obj);
    }
  }

  out_trackobj = trackobj;
  return 0;
}




}
}
