#include "perception_fusion_common/base_tracker/object_filter.h"
#include "perception_fusion_common/common/covariance.h"
#include "perception_fusion_common/common/math_util.h"
#include <Eigen/Dense>
#include <Eigen/Eigen>




namespace avos {
namespace perception {

using namespace cv;
using namespace std;





void OObject::_setQ() {
	kal.Q = Matx<double,5,5>::zeros();
	assert(tracktype == 1 || tracktype == 2 || tracktype == 3);
	kal.Q(0,0) = 0.1;
	kal.Q(1,1) = 0.1;
	kal.Q(2,2) = 0.1;
	kal.Q(3,3) = 0.1;
	kal.Q(4,4) = 1;
}

void OObject::_setObserve(TrackObject &det, const LocationType &locdr, float Tperiod) {
  while(det.heading_dr - kal.theta*180/CV_PI > 90) det.heading_dr -= 180;
  while(kal.theta*180/CV_PI - det.heading_dr > 90) det.heading_dr += 180;
  Vec3d observe_3ddet(det.x_dr, det.y_dr, det.heading_dr*CV_PI/180);

  Matx33d R_3ddet = Matx33d::eye();
  if(tracktype == 1) { // vision
    bool clipped = false;
    if((box2d.width > 0) && (box2d.x < 50 || box2d.br().x > 640-50))
      clipped = true;
    float dist = norm(Vec2f(x_vcs,y_vcs));

    Matx22f cov;
    if(type == FusionObjType::FUS_TRUCK || type == FusionObjType::FUS_BUS) {
      // if(clipped) {
      //   Matx22f cov1 = getCovMat1(5+dist*0.12, 2+dist*0.01, atan2(y_vcs,x_vcs)*180/CV_PI + locdr.yaw);
      //   Matx22f cov2 = getCovMat1(100, 0.1, kal.theta*180/CV_PI); // 车身长度误差
      //   // Matx22f cov3 = getCovMat1(fmax(10,30-dist), 0.1, heading_dr); // 近距离大车检测误差
      //   cov = cov1 + cov2;
      // }
      // else {
        Matx22f cov1 = getCovMat1(2+dist*0.06, 2+dist*0.007, atan2(y_vcs,x_vcs)*180/CV_PI + locdr.yaw); //计算协方差矩阵
        Matx22f cov2 = getCovMat1(5, 0.01, kal.theta*180/CV_PI); // 车身长度误差 
        cov = cov1 + cov2;
      // }
    }
    else {
      cov = getCovMat1(2+dist*0.05, 1.5+dist*0.002, atan2(y_vcs,x_vcs)*180/CV_PI + locdr.yaw);
    }
    R_3ddet(0,0) = cov(0,0);
    R_3ddet(0,1) = cov(0,1);
    R_3ddet(1,0) = cov(1,0);
    R_3ddet(1,1) = cov(1,1);

    if(type == FusionObjType::FUS_TRUCK || type == FusionObjType::FUS_BUS)
      R_3ddet(2,2) = fmax(10,150-8*dist);
    else
      R_3ddet(2,2) = fmax(10,90-8*dist);
    if(clipped || box2d.width > 200)
      R_3ddet(2,2) = 300;

    R_3ddet *= 25; // 视觉统一把协方差按比例放大来达到平滑的效果，tracker里的匹配门限要记得按比例缩回去才能与实际尺寸相符
  }
  else if(tracktype == 2) { // lidar
  	if(length < 8) {
	  	Matx22f cov = getCovMat1(length*0.2, width*0.2, kal.theta*180/CV_PI);
	    R_3ddet(0,0) = cov(0,0);
	    R_3ddet(0,1) = cov(0,1);
	    R_3ddet(1,0) = cov(1,0);
	    R_3ddet(1,1) = cov(1,1);
	    R_3ddet(2,2) = 3.0*CV_PI/180.;
	  }
	  else {
	  	Matx22f cov = getCovMat1(length*1.5/2, width/2, kal.theta*180/CV_PI);
	    R_3ddet(0,0) = cov(0,0);
	    R_3ddet(0,1) = cov(0,1);
	    R_3ddet(1,0) = cov(1,0);
	    R_3ddet(1,1) = cov(1,1);
	    R_3ddet(2,2) = 5.0*CV_PI/180.;
	  }

	  // R_3ddet *= 25;
  }
  else if(tracktype == 3) { // bev vision
    float dist = norm(Vec2f(x_vcs,y_vcs));

    Matx22f cov;
    if(type == FusionObjType::FUS_TRUCK || type == FusionObjType::FUS_BUS) {
      Matx22f cov1 = getCovMat1(2+dist*0.06, 2+dist*0.007, atan2(y_vcs,x_vcs)*180/CV_PI + locdr.yaw);
      Matx22f cov2 = getCovMat1(5, 0.01, kal.theta*180/CV_PI); // 车身长度误差
      cov = cov1 + cov2;
    }
    else {
      cov = getCovMat1(2+dist*0.05, 1.5+dist*0.002, atan2(y_vcs,x_vcs)*180/CV_PI + locdr.yaw);
    }
    R_3ddet(0,0) = cov(0,0);
    R_3ddet(0,1) = cov(0,1);
    R_3ddet(1,0) = cov(1,0);
    R_3ddet(1,1) = cov(1,1);

    if(type == FusionObjType::FUS_TRUCK || type == FusionObjType::FUS_BUS)
      R_3ddet(2,2) = fmax(10,150-8*dist);
    else
      R_3ddet(2,2) = fmax(10,90-8*dist);

    R_3ddet *= 25; // 视觉统一把协方差按比例放大来达到平滑的效果，tracker里的匹配门限要记得按比例缩回去才能与实际尺寸相符
  }

  kal.observe = observe_3ddet;
  kal.R = R_3ddet;
}


void OObject::_fuseObserveWithRadar(const LocationType &locdr, float Tperiod, const RadarTrackMsg &radartracks) {
  // // radar speed correction
  // if(fabs(locdr.yawrate) < 6 && tracktype == 2 && trk_age > 1) {
  //   double sumx = 0, sumy = 0;
  //   int N = 0;
  //   for(auto &ro: radartracks) {
  //     RotatedRect rrect(cv::Point2f(x_vcs,y_vcs),Size2f(length,width),heading_vcs);
  //     cv::Point2f vertices[4];
  //     rrect.points(vertices);
  //     vector<cv::Point2f> points;
  //     for(int i=0;i<4;i++)
  //       points.push_back(vertices[i]);
  //     if(pointPolygonTest(points, cv::Point2f(ro.x_vcs, ro.y_vcs), false) >= 0) { // 在矩形内部
  //       double xx = kal.x_prev + ro.vx_dr*Tperiod;
  //       double yy = kal.y_prev + ro.vy_dr*Tperiod;
  //       sumx += xx;
  //       sumy += yy;
  //       N++;
  //     }
  //   }
  //   if(N > 0) {
  //     double xdr = sumx/N;
  //     double ydr = sumy/N;
  //     kal.observe[0] = xdr*0.7+kal.observe[0]*0.3;
  //     kal.observe[1] = ydr*0.7+kal.observe[1]*0.3;
  //   }
  // }
  // // TODO 对应修正观测协方差R
}




void OObject::_coorTransform(const LocationType &locdr) {
  // transform to vcs
  Eigen::Vector3d obj_dr_pos(kal.x, kal.y, 0);
  Eigen::Vector3d obj_dr_vel(kal.v*cos(kal.theta), kal.v*sin(kal.theta), 0);
  auto obj_vcs_pos = locdr.ego_R.inverse() * (obj_dr_pos - locdr.ego_t);
  Eigen::Vector3d own_abs_vel(locdr.speed - locdr.yawrate * CV_PI/180 * obj_vcs_pos[1],
                             locdr.yawrate * CV_PI/180 * obj_vcs_pos[0], 0);
  auto obj_vcs_vel = locdr.ego_R.inverse() * obj_dr_vel - own_abs_vel;
  auto obj_dr_vel_self = locdr.ego_R.inverse() * obj_dr_vel;
  x_vcs = obj_vcs_pos[0];
  y_vcs = obj_vcs_pos[1];
  vx_vcs = obj_vcs_vel[0];
  vy_vcs = obj_vcs_vel[1];
  vx_ground = obj_dr_vel_self[0];
  vy_ground = obj_dr_vel_self[1];
  heading_vcs = algorithm::NormalizeHeading(kal.theta*180/CV_PI - locdr.yaw);

  x_dr = kal.x;
  y_dr = kal.y;
  z_dr = 0;
  vx_dr = obj_dr_vel[0];
  vy_dr = obj_dr_vel[1];
  vz_dr = obj_dr_vel[2];
  speed_dr = sqrt(vx_dr*vx_dr + vy_dr*vy_dr + vz_dr*vz_dr);
  heading_dr = kal.theta*180/CV_PI;

  Vec2f spdvec = algorithm::angle2vec(atan2(vy_dr, vx_dr) * 180 / CV_PI);
  Vec2f nowvec = algorithm::angle2vec(kal.theta * 180 / CV_PI);
  //机动车速度大于3m/s，可以用运动方向判断朝向是否错误
  if (std::abs(speed_dr)>3&&(type==FusionObjType::FUS_CAR ||type==FusionObjType::FUS_BUS||type==FusionObjType::FUS_TRUCK)&& algorithm::vecdelta(nowvec, spdvec) > 90)
  {
    kal.theta -= CV_PI;
    kal.v = -kal.v;
    kal.a = -kal.a;
    kal.w = -kal.w;
    heading_vcs = algorithm::NormalizeHeading(kal.theta*180/CV_PI - locdr.yaw);
    heading_dr = kal.theta*180/CV_PI;

  }

}


void OObject::of_predect_2d(float shake_y) {
	if(camid == 2) // 2d框纵向用消抖预测
	  box2d.y -= shake_y;
  box2d_predict = box2d;
}
void OObject::of_update_2d_type(TrackObject& det)
{
  if(trk_type.type == FusionObjType::FUS_UNDEFINED)
  {
    trk_type.type = det.type;
    trk_type.type_cnt++;
    return;
  }
  if(trk_type.type == det.type)
  {
    trk_type.type_cnt = std::min(trk_type.type_cnt+1,20);
  }
  else 
  {
    trk_type.type_cnt = std::max(trk_type.type_cnt-1,0);
    if(trk_type.type_cnt<5)
    {
      trk_type.type = det.type;
    }
  }

}
void OObject::of_update_2d(TrackObject& det) {
  det.matched = true;
  matched = true;
  is3d = false;
  trk_age++;

  trigger_stamp = det.trigger_stamp;
  trk_guess_age = 0;
  seg_img_pts = det.seg_img_pts;
  seg_ground_pts = det.seg_ground_pts;
  box2d = det.box2d;
  camid = det.camid;
  of_update_2d_type(det);
  type = trk_type.type;
  //type = det.type;
  conf2d = det.conf2d;
  crop_h30 = det.crop_h30;
}

void OObject::of_fake_update_2d() {
  is3d = false;
  trk_age++;
  // 递推的目标跳过更新，并且清空单帧观测量如分割接地点
  trigger_stamp = 0;
  trk_guess_age++;
  seg_img_pts.clear();
  seg_ground_pts.clear();
}


int OObject::of_predict(const LocationType &locdr, float Tperiod) {
	// 记录上一次的状态用于判断本次滤波结果是否异常
	kal.x_prev = kal.x;
	kal.y_prev = kal.y;
	kal.theta_prev = kal.theta;
	kal.v_prev = kal.v;
	kal.a_prev = kal.a;

	// kalman
	double tmp = (kal.v*Tperiod + 0.5*kal.a*Tperiod*Tperiod); // = vt+1/2vt^2
	kal.x = kal.x + tmp * cos(kal.theta);
	kal.y = kal.y + tmp * sin(kal.theta);
	kal.v = kal.v + kal.a*Tperiod;

  CHECK_RANGE(kal.a, -20, 20);
  CHECK_RANGE(kal.v, -60, 60);

	// J
	Matx<double,5,5> J = Matx<double,5,5>::eye();
	J(0,2) = -tmp*sin(kal.theta);
	J(0,3) = Tperiod*cos(kal.theta);
	J(0,4) = 0.5*Tperiod*Tperiod*cos(kal.theta);
	J(1,2) = tmp*cos(kal.theta);
	J(1,3) = Tperiod*sin(kal.theta);
	J(1,4) = 0.5*Tperiod*Tperiod*sin(kal.theta);
	J(3,4) = Tperiod;
	_setQ();

	kal.P = J * kal.P * J.t() + kal.Q;

  _coorTransform(locdr);
  return 0;
}


int OObject::of_update(TrackObject& det, bool isinit, const LocationType &locdr, float Tperiod, const RadarTrackMsg &radartracks) {
  det.matched = true;
  matched = true;
  trk_age++;

  trigger_stamp = det.trigger_stamp;
  trk_guess_age = 0;
  seg_img_pts = det.seg_img_pts;
  seg_ground_pts = det.seg_ground_pts;
  x_vcs_det = det.x_vcs;
  y_vcs_det = det.y_vcs;
  z_vcs_det = det.z_vcs;
  heading_vcs_det = det.heading_vcs;
  x_dr_det = det.x_dr;
  y_dr_det = det.y_dr;
  z_dr_det = det.z_dr;
  heading_dr_det = det.heading_dr;
  //车头车尾
  htflag = det.htflag; // -1:none 0:head 1:tail 
	htbox2d = det.htbox2d; // head or tail box2d 
  if(tracktype == 2) { // lidar obj尺寸不做滤波，因为当检测框尺寸变化时会根据内侧边缘修正位置，滤波后边缘就是错的了

    //std::cout<<"lidartrack "<<trk_id<<","<<trk_age<<","<<(int)type<<","<<det.y_vcs<<","<<det.heading_vcs<<std::endl;
    if ((type == FusionObjType::FUS_TRUCK || type == FusionObjType::FUS_CAR) && fabs(det.y_vcs) < 10&&det.x_vcs>-30 && IsHeadingVertical(det.heading_vcs))//对正前方的大车滤波尺寸
    {
      if (length == 0)
      {
        length = det.length;
        width = det.width;
        height = det.height;
      }
      else
      {
        length = length * 0.99 + det.length * 0.01;
        width = width * 0.9 + det.width * 0.1;
        height = height * 0.9 + det.height * 0.1;
      }
      float angle = det.heading_vcs;
      while (angle > 90)
        angle -= 180;
      while (angle < -90)
        angle += 180;
      angle *= CV_PI / 180;
      float tailx = det.x_vcs - det.length / 2 * cos(angle); //检测尾部中心点
      float taily = det.y_vcs - det.length / 2 * sin(angle);
      x_vcs_det = tailx + length / 2 * cos(angle); 
      y_vcs_det = taily + length / 2 * sin(angle);
      if(tailx < 0)//用头部点准确
      {
        tailx = det.x_vcs + det.length / 2 * cos(angle);
        x_vcs_det = tailx - length / 2 * cos(angle);
        taily = det.y_vcs + det.length / 2 * sin(angle);
        y_vcs_det = taily - length / 2 * sin(angle);
      }
      else if (tailx > 0 && tailx < 8 && (type == FusionObjType::FUS_CAR && det.length < 6) && fabs(det.y_vcs) < 4)//对侧面的近距离目标，激光会脑补车头车尾，用中心点就好
      {
        x_vcs_det = det.x_vcs;
        y_vcs_det = det.y_vcs;
      }
      // transform vcs observation to dr
      Eigen::Vector3d obj_vcs_pos(x_vcs_det, y_vcs_det, 0);
      auto obj_dr_pos = locdr.ego_R * obj_vcs_pos + locdr.ego_t;
      x_dr_det = obj_dr_pos[0];
      y_dr_det = obj_dr_pos[1];
      det.x_dr = x_dr_det;
      det.y_dr = y_dr_det;
    }
    else 
    {
      length = length*0.9 + det.length*0.1;
      width = width*0.9 + det.width*0.1;
      height = height*0.9 + det.height*0.1;
    }
  }
  else {
    if(length == 0) {
      length = det.length;
      width = det.width;
      height = det.height; 
    }
    else {
      length = length*0.99 + det.length*0.01;
      width = width*0.9 + det.width*0.1;
      height = height*0.9 + det.height*0.1;
    }
  }
  box2d = det.box2d;
  type = det.type;
  conf2d = det.conf2d;
  conf3d = det.conf3d;
  crop_h30 = det.crop_h30;


  // 注意_setObserve里用到的一些属性初始化时还没有，在这里把第一帧观测值赋值进去
  if(isinit) {
    x_vcs = det.x_vcs;
    y_vcs = det.y_vcs;
    z_vcs = det.z_vcs;
    heading_vcs = det.heading_vcs;
    kal.theta = det.heading_dr*CV_PI/180;
  }

  is3d = det.is3d;
  if(det.is3d) {

    _setObserve(det, locdr, Tperiod);
    kal.S = kal.H * kal.P * kal.H.t() + kal.R;
    kal.K = kal.P * kal.H.t() * kal.S.inv();

    if(isinit) {
      kal.x = kal.observe[0];
      kal.y = kal.observe[1];
      kal.theta = kal.observe[2];
      kal.v = 0;
      kal.a = 0;
      kal.P = Matx<double,5,5>::eye();
      // kal.P(0,0) = 100;
      // kal.P(1,1) = 100;
      Matx22f cov2 = getCovMat1(3, 3, 0); // 初始运动不确定
      kal.P(0,0) = kal.R(0,0) + cov2(0,0);
      kal.P(0,1) = kal.R(0,1) + cov2(0,1);
      kal.P(1,0) = kal.R(1,0) + cov2(1,0);
      kal.P(1,1) = kal.R(1,1) + cov2(1,1);
      kal.P(2,2) = 10;
      kal.P(3,3) = 20;
      kal.P(4,4) = 10;
    }
    else {
      // if(norm(Vec2f(kal.x-kal.observe[0], kal.y-kal.observe[1])) > 5) {
      //   kal.x = kal.observe[0];
      //   kal.y = kal.observe[1];
      // }
      // else {
        Vec<double,5> pred(kal.x, kal.y, kal.theta, kal.v, kal.a);
        Vec<double,5> state = pred + kal.K * (kal.observe - kal.H * pred);
        kal.P = (Matx<double,5,5>::eye() - kal.K * kal.H) * kal.P;
        kal.x = state[0];
        kal.y = state[1];
        kal.theta = state[2];
        kal.v = state[3];
        kal.a = state[4];

        // // if(fabs(state[2]-kal.theta) > 10/57.3) kal.bad_count++;
        // // if(fabs(state[3]-kal.v) > 5) kal.bad_count++;
        // // if(fabs(state[4]-kal.a) > 0.5) kal.bad_count++;
        // if(fabs(state[2]-kal.theta_prev) > 10/57.3) kal.bad_count++;
        // if(fabs(state[3]-kal.v_prev) > 5) kal.bad_count++;
        // if(fabs(state[4]-kal.a_prev) > 0.5) kal.bad_count++;
      // }
    }

    _coorTransform(locdr);
    z_vcs = det.z_vcs;

    CHECK_RANGE(x_vcs, -300, 300);
    CHECK_RANGE(y_vcs, -300, 300);
    CHECK_RANGE(vx_vcs, -60, 60);
    CHECK_RANGE(vy_vcs, -60, 60);
    CHECK_RANGE(vx_ground, -60, 60);
    CHECK_RANGE(vy_ground, -60, 60);
    CHECK_RANGE(heading_vcs, -180, 180);
    CHECK_RANGE(kal.a, -20, 20);
    CHECK_RANGE(kal.v, -60, 60);
  }

  return 0;
}



void OObject::of_fake_update() {
  trk_age++;
  // 递推的目标跳过更新，并且清空单帧观测量如分割接地点
  trigger_stamp = 0;
  trk_guess_age++;
  seg_img_pts.clear();
  seg_ground_pts.clear();
  x_vcs_det = 0;
  y_vcs_det = 0;
  z_vcs_det = 0;
  heading_vcs_det = 0;
  x_dr_det = 0;
  y_dr_det = 0;
  z_dr_det = 0;
  heading_dr_det = 0;
}



void OObject::of_initFromGlobalObj(const vector<RefObject> &globalobjs) {
  // 外部使用者负责确保线程安全
  // 不局限于是否是同一个目标，而是假定车辆前后一定距离范围内出现的目标速度都与车辆相同，目标车速越快，距离范围越大
  if(type == FusionObjType::FUS_PEDESTRIAN) return;
  if(trk_age > 5) return;
  for(auto &glo: globalobjs) {
    if(glo.trk_age < 10) continue;
    if(glo.trk_age < 30 && glo.trk_guess_age > 0) continue;
    if(fabs(glo.kal_a) > 1.0) continue; // 只用平稳的目标做，加速度大可能是错的
    if(norm(Vec2f(x_vcs-glo.x_vcs, y_vcs-glo.y_vcs)) > 50) continue;
    Matx22f cov = getCovMat1(glo.length+glo.speed_dr, glo.width, glo.heading_vcs);
    RotatedRect rrect = getRRectFromCovMat1(cov, cv::Point2f(glo.x_vcs,glo.y_vcs), 1);
    if(ellipseDist(rrect, cv::Point2f(x_vcs,y_vcs)) < 0.5) {
      kal.theta = glo.kal_theta;
      kal.v = glo.kal_v;
      kal.a = glo.kal_a;
    }
  }
}




void OObject::of_initFromRadar(const RadarTrackMsg &radartracks) {
  if(type == FusionObjType::FUS_PEDESTRIAN) return;
  if(trk_age > 3) return;
  if(tracktype == 1) { // vision
    for(auto &ro: radartracks.objs) {
      if(ro.speed_dr < 5) continue;
      if(norm(Vec2f(x_vcs-ro.x_vcs, y_vcs-ro.y_vcs)) > 50) continue;
      Matx22f cov = getCovMat1(5+ro.speed_dr, 4, atan2(ro.vy_ground, ro.vx_ground));
      RotatedRect rrect = getRRectFromCovMat1(cov, cv::Point2f(ro.x_vcs,ro.y_vcs), 1);
      if(ellipseDist(rrect, cv::Point2f(x_vcs,y_vcs)) < 0.5) {
        kal.theta = atan2(ro.vy_dr, ro.vx_dr);
        kal.v = ro.speed_dr; // 速度方向正负号与theta自洽
        kal.a = 0;
      }
    }
  }
  else if(tracktype == 2) { // lidar
    double sumv = 0, sumvx = 0, sumvy = 0;
    int N = 0;
    for(auto &ro: radartracks.objs) {
      if(ro.speed_dr < 5) continue;
      RotatedRect rrect(cv::Point2f(x_vcs,y_vcs),Size2f(length,width),heading_vcs);
      cv::Point2f vertices[4];
      rrect.points(vertices);
      vector<cv::Point2f> points;
      for(int i=0;i<4;i++)
        points.push_back(vertices[i]);
      if(pointPolygonTest(points, cv::Point2f(ro.x_vcs, ro.y_vcs), false) >= 0) { // 在矩形内部
        sumv += ro.speed_dr;
        sumvx += ro.vx_dr;
        sumvy += ro.vy_dr;
        N++;
      }
    }
    if(N > 0) {
      kal.theta = atan2(sumvy/N, sumvx/N);
      kal.v = sumv/N; // 速度方向正负号与theta自洽
      kal.a = 0;
    }
  }
}





}
}
