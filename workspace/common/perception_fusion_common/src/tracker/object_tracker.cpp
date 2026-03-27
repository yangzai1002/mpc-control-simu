#include "perception_fusion_common/base_tracker/object_tracker.h"
#include "perception_fusion_common/base_tracker/object_filter.h"
#include "perception_fusion_common/common/covariance.h"
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include "perception_fusion_common/common/overlaps.h"

namespace avos {
namespace perception {

using namespace cv;
using namespace std;


typedef std::pair<size_t, size_t> MatchPair;

int OTracker::tracking_bev_GNN(const vector<TrackObject> &dets, vector<MatchPair> &pairs) {
	pairs.clear();
  if(dets.empty() || trackobj.empty()) return 0;

  int num_det=dets.size();
  int num_trk=trackobj.size();
  vector<vector<float>> distance_matrix;
  distance_matrix.resize(num_det, vector<float>(num_trk));
  Overlaps m;
  for(int i=0;i<num_det;i++) {
    auto &det = dets[i];
    for(int j=0;j<num_trk;j++) {
      auto &trk = trackobj[j];
      distance_matrix[i][j] = 1000;
      if(det.matched || trk.matched) continue;
      if(!det.is3d || !trk.is3d) continue;
      float dist = 10000;
      // if(fmin(det.length,trk.length)/fmax(det.length,trk.length) < 0.3) // 尺寸不会突变 //1016激光模型效果较差，先注释掉
      //   dist = 1000;
      // else 
      {
        double heading_dr = det.heading_dr;
        while(heading_dr - trk.kal.theta*180/CV_PI > 90) heading_dr -= 180;
        while(trk.kal.theta*180/CV_PI - heading_dr > 90) heading_dr += 180;
        Vec3d observe(det.x_dr, det.y_dr, heading_dr*CV_PI/180);
        // Vec3d observe(det.x_dr, det.y_dr, trk.kal.theta);
        Vec<double,5> pred(trk.kal.x, trk.kal.y, trk.kal.theta, trk.kal.v, trk.kal.a);
        Vec3d residual = observe - trk.kal.H * pred; //计算测量残差
        // auto residual_dif = trk.kal.K * residual;

         //dist = sqrt(residual_dif[0]*residual_dif[0] +residual_dif[1]*residual_dif[1]);
          dist = sqrt(residual[0]*residual[0] +residual[1]*residual[1]);
       // Matx33d Sinv = trk.kal.S.inv();
        // if(trk.type == FusionObjType::FUS_PEDESTRIAN || trk.type == FusionObjType::FUS_CYCLIST) { // 行人和骑行者的航向检测不准，不要用来计算匹配代价
        //   Vec3d residual0(residual[0], residual[1], 0);
        //   //dist = sqrt((residual0.t() * Sinv * residual0)[0]); // mahalanobis dist
        //   dist = fabs(residual[0]);
        // }
        // else {
        //   //dist = sqrt((residual.t() * Sinv * residual)[0]); // mahalanobis dist

        // }
        float org_boxes[5] = {
            static_cast<float>(trk.x_dr),
            static_cast<float>(trk.y_dr),
            static_cast<float>(trk.width),
            static_cast<float>(trk.length),
            static_cast<float>(trk.heading_dr)
        };
        float det_boxes[5] = {
            static_cast<float>(det.x_dr),
            static_cast<float>(det.y_dr),
            static_cast<float>(det.width),
            static_cast<float>(det.length),
            static_cast<float>(det.heading_dr)
        };
        //  float org_boxes[5] = {trk.x_dr, trk.y_dr, trk.width, trk.length, trk.heading_dr};
          // float det_boxes[5] = {det.x_dr, det.y_dr, det.width, det.length, det.heading_dr};
          double iou_cost = m.Compute(org_boxes,det_boxes,2);
          if(iou_cost>=1)iou_cost=0.9;
          dist = dist*(1-iou_cost);
        if(trk.tracktype == 1) { // vision
        	if(dist > 1) dist = 1000;
	        else if(trk.trk_age > 3) { // 排除不合理的错误
	          Vec<double,5> state = pred + trk.kal.K * residual;
	          if(fabs(state[2]-trk.kal.theta_prev) > 10/57.3) dist = 1000;
	          if(fabs(state[3]-trk.kal.v_prev) > 5) dist = 1000;
	          if(fabs(state[4]-trk.kal.a_prev) > 0.5) dist = 1000;
	          if(det.type == FusionObjType::FUS_PEDESTRIAN && state[3] > 5) dist = 1000;
	          if(state[3] > 40) dist = 1000;
	        }
        }
        else if(trk.tracktype == 2) { // lidar
        //std::cout<<"trk "<<trk.trk_id<<","<<trk.trk_age<<","<<dist<<","<<trk.x_vcs<<","<<trk.y_vcs<<","<<det.x_vcs_det<<","<<det.y_vcs_det<<std::endl;
          if(dist > 5) dist = 1000;
          else if(trk.trk_age > 1 && iou_cost<=0.5) { // 排除不合理的错误
            float extra_alpha = fabs(fmax(det.length,trk.length)/fmin(det.length,trk.length));//尺寸跳变，阈值也相应改变
            if(trk.length<6&&trk.trk_age > 3&&trk.speed_dr<10&&dist>2)dist = 1000; //小车或行人机动车，检测稳定，dist不应该很大。todo速度很快时有可能跟踪不上
            if((det.type == FusionObjType::FUS_CAR || det.type == FusionObjType::FUS_TRUCK || det.type == FusionObjType::FUS_BUS)&&extra_alpha>3)dist = 1000;
            if(det.type != FusionObjType::FUS_PEDESTRIAN) extra_alpha = 2*extra_alpha;
            Vec<double,5> state = pred + trk.kal.K * residual;
            if((det.type == FusionObjType::FUS_CAR || det.type == FusionObjType::FUS_TRUCK || det.type == FusionObjType::FUS_BUS) 
                && fabs(state[2]-trk.kal.theta_prev) > 10/57.3) dist = 1000; // 非车类别的航向容易检测不准，不用来判断异常
            if((trk.type == FusionObjType::FUS_CAR||trk.type ==  FusionObjType::FUS_TRUCK||trk.type ==  FusionObjType::FUS_BUS)&&
            (det.type == FusionObjType::FUS_PEDESTRIAN)) dist = 1000;
            if((det.type == FusionObjType::FUS_CAR||det.type ==  FusionObjType::FUS_TRUCK||det.type ==  FusionObjType::FUS_BUS)&&
            (trk.type == FusionObjType::FUS_PEDESTRIAN)) dist = 1000;
            //if(fmin(det.length,trk.length) > 3 && fmin(det.length,trk.length) < 7 && fmax(det.length,trk.length)/fmin(det.length,trk.length) > 1.5) dist = 1000;// 尺寸不许突变

            //if(trk.trk_age > 5&&sqrt((trk.kal.x - det.x_dr_det)*(trk.kal.x - det.x_dr_det) + (trk.kal.y - det.y_dr_det)*(trk.kal.y - det.y_dr_det))> 2 )dist = 1000;

            if(fabs(state[3]-trk.kal.v_prev) > 5*extra_alpha) dist = 1000;
            if(fabs(state[4]-trk.kal.a_prev) > 3*extra_alpha) dist = 1000;
            if(trk.type == FusionObjType::FUS_PEDESTRIAN && (state[3] > 5||dist>1)) dist = 1000;
            if(state[3] > 40) dist = 1000;
          // std::cout<<"trk match"<<trk.trk_id<<","<<trk.trk_age<<","<<dist<<","<<state[3]<<","<<trk.kal.v_prev<<","<<state[2]<<","<<
          // trk.kal.theta_prev<<","<<state[4]<<","<< trk.kal.a_prev<<","<<extra_alpha<<std::endl;
          // std::cout<<trk.x_dr<<","<<trk.y_dr<<","<<det.x_dr<<","<<det.y_dr<<","<<residual[0]<<","<<residual[1]<<std::endl;
          }
        }
        else if(trk.tracktype == 3) { // bev vision
          if(dist > 2) dist = 1000;
          else if(trk.trk_age > 3) { // 排除不合理的错误
            Vec<double,5> state = pred + trk.kal.K * residual;
            // if((det.type == FusionObjType::FUS_CAR || det.type == FusionObjType::FUS_TRUCK || det.type == FusionObjType::FUS_BUS) 
            //     && fabs(state[2]-trk.kal.theta_prev) > 10/57.3) dist = 1000; // 非车类别的航向容易检测不准，不用来判断异常
            if(fabs(state[3]-trk.kal.v_prev) > 5) dist = 1000;
            if(fabs(state[4]-trk.kal.a_prev) > 0.5) dist = 1000;
            // if(det.type == FusionObjType::FUS_PEDESTRIAN && state[3] > 5) dist = 1000;
            if(state[3] > 40) dist = 1000;
          }
        }

      }
      distance_matrix[i][j] = dist;
    }
  }

  // for(int i=0;i<num_det;i++) {
  //   for(int j=0;j<num_trk;j++)
  //     std::cout << distance_matrix[i][j] << ", " << std::flush;
  //   std::cout << "" << std::endl;
  // }

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
      // std::cout << "push: " << mini << " " << minj << std::endl;
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




int OTracker::tracking_img2d_GNN(const vector<TrackObject> &dets, vector<MatchPair> &pairs) {
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

      if(!det.is3d || !trk.is3d) continue;
      float dist = 10000;
      if(fmin(det.length,trk.length)/fmax(det.length,trk.length) < 0.3) // 尺寸不会突变
        dist = 1000;
      else {
        float iou = (float)(det.box2d & trk.box2d).area() / (det.box2d | trk.box2d).area();
        if(iou > 0.7) {
          if(trk.trk_age < 5 || norm(Vec2f(det.x_vcs-trk.x_vcs, det.y_vcs-trk.y_vcs)) > 10) dist = 1000; // 距离太远的就不关联了，关联上对滤波也是干扰
          else {
            dist = 1000-iou;
            double heading_dr = det.heading_dr;
            while(heading_dr - trk.kal.theta*180/CV_PI > 90) heading_dr -= 180;
            while(trk.kal.theta*180/CV_PI - heading_dr > 90) heading_dr += 180;
            Vec3d observe(det.x_dr, det.y_dr, heading_dr*CV_PI/180);
            Vec<double,5> pred(trk.kal.x, trk.kal.y, trk.kal.theta, trk.kal.v, trk.kal.a);
            Vec3d residual = observe - trk.kal.H * pred;
            if(trk.trk_age > 2) { // 排除不合理的错误
              Vec<double,5> state = pred + trk.kal.K * residual;
              if(fabs(state[2]-trk.kal.theta_prev) > 10/57.3) dist = 1000;
              if(fabs(state[3]-trk.kal.v_prev) > 5) dist = 1000;
              if(fabs(state[4]-trk.kal.a_prev) > 0.5) dist = 1000;
              if(det.type == FusionObjType::FUS_PEDESTRIAN && state[3] > 5) dist = 1000;
              if(state[3] > 40) dist = 1000;
            }
          }
        }
        else
          dist = 1000;
      }
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



void OTracker::getMatchGate(OObject &trk, const LocationType &locdr) {
	trk.kal.S = trk.kal.H * trk.kal.P * trk.kal.H.t() + trk.kal.R; // R用的是上一帧的，假设相邻帧R变化不大
	trk.kal.K = trk.kal.P * trk.kal.H.t() * trk.kal.S.inv(); // 这个K只是在匹配时做pseudo update时用的

	// 可视化匹配门限 只是近似，因为马氏距离除了位置还有航向维度
	if(trk.tracktype == 1) { // vision
		trk.kal.S *= 1.0/25;
	}
	else if(trk.tracktype == 2) { // lidar
	}
  else if(trk.tracktype == 3) { // bev vision
    trk.kal.S *= 1.0/25;
  }
	Matx22f cov(trk.kal.S(0,0), trk.kal.S(0,1), trk.kal.S(1,0), trk.kal.S(1,1));
	trk.match_gate = getRRectFromCovMat1(cov, cv::Point2f(trk.x_vcs,trk.y_vcs), 1);
	trk.match_gate.angle -= locdr.yaw; // dr2vcs
}


int OTracker::obj_tracking(double stamp, const vector<DetObject> &input_dets, \
                            const LocationType &locdr, float shake_y, \
                            const RadarTrackMsg &radartracks, std::vector<OObject> &out_trackobj) {
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
	for(auto &trk: trackobj) {
  	trk.of_predict(locdr, Tperiod);
	  if(camid == 2)
	  	trk.of_predect_2d(shake_y);
	  getMatchGate(trk, locdr);
    CHECK_RANGE(trk.x_vcs, -300, 300);
    CHECK_RANGE(trk.y_vcs, -300, 300);
	}

	// bev跟踪
	vector<MatchPair> pairs;
	if(tracking_bev_GNN(dets, pairs) > 0)
	  for(auto &p: pairs)
	    trackobj[p.second].of_update(dets[p.first], false, locdr, Tperiod, radartracks);

  // 图像2d跟踪
	if((camid == 2 || camid == 5 || camid == 3 || camid == 4) && fabs(locdr.yawrate) < 6) {
	  if(tracking_img2d_GNN(dets, pairs) > 0)
		  for(auto &p: pairs)
		    trackobj[p.second].of_update(dets[p.first], false, locdr, Tperiod, radartracks);
	}

  // 删掉跟踪上但是不合理的
  if(!trackobj.empty()) {
    for(int i=trackobj.size()-1;i>=0;i--) {
      auto &trk = trackobj[i];
      if(trk.kal.bad_count > 0)
        trackobj.erase(trackobj.begin()+i);
    }
  }

  // 删掉没有跟踪上的
  if(!trackobj.empty()) {
    for(int i=trackobj.size()-1;i>=0;i--) {
      auto &trk = trackobj[i];
      if(trk.matched)
        continue;
      trk.of_fake_update();
      // 逻辑反过来写：哪些不要删，剩下的都删
      bool tobedelete = true;
      if(trk.tracktype == 1) { // vision
	      if(trk.trk_age >= 2 && trk.trk_guess_age <= 3) tobedelete = false;
	      if((trk.type == FusionObjType::FUS_TRUCK || trk.type == FusionObjType::FUS_BUS) && fabs(trk.y_vcs) < 5 && fabs(trk.x_vcs) < 10 && trk.trk_age >= 5 && trk.trk_guess_age < 10) tobedelete = false;
	      // if(trk.trk_age >= 5 && norm(Vec2f(trk.vx_dr,trk.vy_dr)) < 3 && trk.trk_guess_age < 20) tobedelete = false;
	    }
	    else if(trk.tracktype == 2) { // lidar
	      if(trk.trk_age >= 3 && trk.trk_guess_age <= 4) tobedelete = false;
	    }
      else if(trk.tracktype == 3) { // bev vision
        if(trk.trk_age >= 2 && trk.trk_guess_age <= 4) tobedelete = false;
      }
      if(tobedelete)
        trackobj.erase(trackobj.begin()+i);
    }
  }

  // create new trk
  for(auto &det: dets) {
    // if(det.matched == false && ((det.type == FUS_CYCLIST && det.conf >= 0.3) || det.conf > 0.4)) { // 没有3d信息也能创建纯2d目标  && det.is3d
    if(det.matched == false && det.is3d && (det.conf2d > 0.5 || det.conf3d > 0.25)) { // 没有3d信息也能创建纯2d目标  && det.is3d
      // 只在关心的区域创建目标
      // if((camid == 0 || camid == 4 || camid == 1 || camid == 3 || camid == 5) && !det.is3d && det.box2d.height < 40) continue; // 
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
      if(camid == 10) {
      	obj.tracktype = 2;
        obj.pos_source = PosSource::LIDAR;
      }
      else if(camid == 7) { // bev vision
        obj.tracktype = 3;
        obj.pos_source = PosSource::VISION;
      }
      else {
      	obj.tracktype = 1;
        obj.pos_source = PosSource::VISION;
      }
      trackobj.push_back(obj);
      trackobj[trackobj.size()-1].of_update(obj, true, locdr, Tperiod, radartracks);
    }
  }


  if(fabs(locdr.yawrate) < 6) // 自车转弯时毫米波速度不准
    init_from_radar(radartracks);


  for(auto &ob: trackobj) {
    CHECK_RANGE(ob.x_vcs, -300, 300);
    CHECK_RANGE(ob.y_vcs, -300, 300);
  }

  out_trackobj = trackobj;
  return 0;
}






}
}
