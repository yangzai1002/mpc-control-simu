 /**
 * 文件  MainProc.cpp
 * 简介  车辆运动学控制模块，用于控制矿车的循迹行驶
 * 作者  张金洋
 * 日期  2023-8-11 10：30：55
 * 版本  0.1
 * 版权  中兵智能AT150
 * 邮件  zjybuaa14@163.com
 * 修改说明 :初版
 */
#include <cmath>
#include "fstream"
#include <algorithm>
#include "comm/yaml_reader.hpp"
#include "comm/comm_struct.hpp"
#include "mpc_lateral_controller/mpc_follower_core.hpp"

using namespace std;
namespace MpcFollower
{

MPCFollowerController::MPCFollowerController(std::string params_path)
{
  std::string config_path = params_path;
  std::string log_path;
  YAML::Node config;
  try
  {
	  config = YAML::LoadFile(config_path);
  }
  catch (...)
  {
    RS_ERROR << "The format of config file " << config_path
      << " is wrong. Please check (e.g. indentation)." << RS_REND;
  }
	YAML::Node param_config = yamlSubNodeAbort(config, "mpc_params");
	//YAML::Node param_config = yamlSubNodeAbort(param_mpc, "common_params");
	yamlRead<bool>(param_config, "show_debug_info", show_debug_info_, bool(false));
	yamlRead<double>(param_config, "ctrl_period", ctrl_period_, double(0.02));
	yamlRead<bool>(param_config, "enable_path_smoothing", enable_path_smoothing_, bool(false));
	yamlRead<bool>(param_config, "enable_yaw_recalculation", enable_yaw_recalculation_, bool(false));
	yamlRead<int>(param_config, "path_filter_moving_ave_num", path_filter_moving_ave_num_, int(35));
	yamlRead<int>(param_config, "path_smoothing_times", path_smoothing_times_, int(1));
	yamlRead<int>(param_config, "curvature_smoothing_num", curvature_smoothing_num_, int(35));
	yamlRead<double>(param_config, "traj_resample_dist", traj_resample_dist_, double(0.1));
	yamlRead<double>(param_config, "admisible_position_error", admisible_position_error_, double(5));
	yamlRead<double>(param_config, "admisible_yaw_error_deg", admisible_yaw_error_deg_, double(5));
	//yamlRead<std::string>(param_config, "ctrl_cmd_interface", output_interface_, std::string("twist"));
	yamlRead<int>(param_config, "mpc_prediction_horizon", mpc_param_.prediction_horizon, int(50));
	yamlRead<double>(param_config, "mpc_prediction_sampling_time", mpc_param_.prediction_sampling_time, double(0.1));
	yamlRead<double>(param_config, "mpc_weight_lat_error", mpc_param_.weight_lat_error, double(1.0));
	yamlRead<double>(param_config, "mpc_weight_heading_error", mpc_param_.weight_heading_error, double(0.0));
	yamlRead<double>(param_config, "mpc_weight_heading_error_squared_vel_coeff", mpc_param_.weight_heading_error_squared_vel_coeff, double(0.3));
	yamlRead<double>(param_config, "mpc_weight_steering_input", mpc_param_.weight_steering_input, double(1.0));
	yamlRead<double>(param_config, "mpc_weight_steering_input_squared_vel_coeff", mpc_param_.weight_steering_input_squared_vel_coeff, double(0.25));
	yamlRead<double>(param_config, "mpc_weight_lat_jerk", mpc_param_.weight_lat_jerk, double(0.0));
	yamlRead<double>(param_config, "mpc_weight_terminal_lat_error", mpc_param_.weight_terminal_lat_error, double(1.0));
	yamlRead<double>(param_config, "mpc_weight_terminal_heading_error", mpc_param_.weight_terminal_heading_error, double(0.1));
	yamlRead<double>(param_config, "mpc_zero_ff_steer_deg", mpc_param_.zero_ff_steer_deg, double(2.0));
	yamlRead<double>(param_config, "delay_compensation_time", mpc_param_.delay_compensation_time, double(0.0));
	yamlRead<double>(param_config, "steer_lim_deg", steer_lim_deg_, double(50.0));
	yamlRead<double>(param_config, "vehicle_model_wheelbase", wheelbase_, double(0.582));
  yamlRead<std::string>(param_config, "log_path", log_path, "opt/usr/");
  yamlRead<std::string>(param_config, "vehicle_model_type", vehicle_model_type_, "kinematics_no_delay");
  
    // 打印 MPC 读取到的所有参数
  std::cout << "\n==================== MPC 参数配置读取结果 ====================" << std::endl;
  std::cout << "show_debug_info                : " << std::boolalpha << show_debug_info_ << std::endl;
  std::cout << "ctrl_period                    : " << ctrl_period_ << " s" << std::endl;
  std::cout << "enable_path_smoothing          : " << std::boolalpha << enable_path_smoothing_ << std::endl;
  std::cout << "enable_yaw_recalculation       : " << std::boolalpha << enable_yaw_recalculation_ << std::endl;
  std::cout << "path_filter_moving_ave_num     : " << path_filter_moving_ave_num_ << std::endl;
  std::cout << "path_smoothing_times           : " << path_smoothing_times_ << std::endl;
  std::cout << "curvature_smoothing_num        : " << curvature_smoothing_num_ << std::endl;
  std::cout << "traj_resample_dist             : " << traj_resample_dist_ << " m" << std::endl;
  std::cout << "admisible_position_error       : " << admisible_position_error_ << " m" << std::endl;
  std::cout << "admisible_yaw_error_deg        : " << admisible_yaw_error_deg_ << " deg" << std::endl;
  std::cout << "mpc_prediction_horizon         : " << mpc_param_.prediction_horizon << std::endl;
  std::cout << "mpc_prediction_sampling_time   : " << mpc_param_.prediction_sampling_time << " s" << std::endl;
  std::cout << "mpc_weight_lat_error           : " << mpc_param_.weight_lat_error << std::endl;
  std::cout << "mpc_weight_heading_error       : " << mpc_param_.weight_heading_error << std::endl;
  std::cout << "weight_heading_error_vel_coeff : " << mpc_param_.weight_heading_error_squared_vel_coeff << std::endl;
  std::cout << "mpc_weight_steering_input      : " << mpc_param_.weight_steering_input << std::endl;
  std::cout << "weight_steer_input_vel_coeff   : " << mpc_param_.weight_steering_input_squared_vel_coeff << std::endl;
  std::cout << "mpc_weight_lat_jerk            : " << mpc_param_.weight_lat_jerk << std::endl;
  std::cout << "weight_terminal_lat_error      : " << mpc_param_.weight_terminal_lat_error << std::endl;
  std::cout << "weight_terminal_heading_error  : " << mpc_param_.weight_terminal_heading_error << std::endl;
  std::cout << "mpc_zero_ff_steer_deg          : " << mpc_param_.zero_ff_steer_deg << " deg" << std::endl;
  std::cout << "delay_compensation_time        : " << mpc_param_.delay_compensation_time << " s" << std::endl;
  std::cout << "steer_lim_deg                  : " << steer_lim_deg_ << " deg" << std::endl;
  std::cout << "vehicle_model_wheelbase        : " << wheelbase_ << " m" << std::endl;
  std::cout << "log_path                       : " << log_path << std::endl;
  std::cout << "vehicle_model_type             : " << vehicle_model_type_ << std::endl;
  std::cout << "==============================================================\n" << std::endl;

  if (vehicle_model_type_ == "kinematics")
  {
    double steer_tau;
    yamlRead<double>(param_config, "vehicle_model_steer_tau", steer_tau, 0.1);
    vehicle_model_ptr_ = std::make_shared<KinematicsBicycleModel>(wheelbase_, amathutils::deg2rad(steer_lim_deg_), steer_tau);
    RS_INFO << "[MPC] set vehicle_model = kinematics";
  }
  else if (vehicle_model_type_ == "kinematics_no_delay")
  {
    vehicle_model_ptr_ = std::make_shared<KinematicsBicycleModelNoDelay>(wheelbase_, amathutils::deg2rad(steer_lim_deg_));
    RS_INFO <<"[MPC] set vehicle_model = kinematics_no_delay";
  }
  else if (vehicle_model_type_ == "dynamics")
  {
    double mass_fl, mass_fr, mass_rl, mass_rr, cf, cr;
    yamlRead<double>(param_config, "mass_fl", mass_fl, 600);
    yamlRead<double>(param_config, "mass_fr", mass_fr, 600);
    yamlRead<double>(param_config, "mass_rl", mass_rl, 600);
    yamlRead<double>(param_config, "mass_rr", mass_rr, 600);
    yamlRead<double>(param_config, "cf", cf, 155494.663);
    yamlRead<double>(param_config, "cr", cr, 155494.663);
    vehicle_model_ptr_ = std::make_shared<DynamicsBicycleModel>(wheelbase_, mass_fl, mass_fr, mass_rl, mass_rr, cf, cr);
    RS_INFO << "[MPC] set vehicle_model = dynamics";
  }
  else
  {
    RS_ERROR << "[MPC] vehicle_model_type is undefined";
  }

  /* QP solver setup */
  std::string qp_solver_type_;
  yamlRead<std::string>(param_config, "qp_solver_type", qp_solver_type_, "unconstraint_fast");
  if (qp_solver_type_ == "unconstraint")
  {
    qpsolver_ptr_ = std::make_shared<QPSolverEigenLeastSquare>();
    RS_INFO << "[MPC] set qp solver = unconstraint";
  }
  else if (qp_solver_type_ == "unconstraint_fast")
  {
    qpsolver_ptr_ = std::make_shared<QPSolverEigenLeastSquareLLT>();
    RS_INFO << "[MPC] set qp solver = unconstraint_fast";
  }
  else if (qp_solver_type_ == "qpoases_hotstart")
  {
    int max_iter;
    yamlRead<int>(param_config, "qpoases_max_iter", max_iter,500);
    qpsolver_ptr_ = std::make_shared<QPSolverQpoasesHotstart>(max_iter);
    RS_INFO << "[MPC] set qp solver = qpoases_hotstart";
  }
  else
  {
    RS_ERROR << "[MPC] qp_solver_type is undefined";
  }

  steer_cmd_prev_ = 0.0;
  lateral_error_prev_ = 0.0;
  yaw_error_prev_ = 0.0;

  /* delay compensation */
  const int delay_step = std::round(mpc_param_.delay_compensation_time / ctrl_period_);
  std::deque<double> tmp_deque(delay_step, 0.0);
  input_buffer_ = tmp_deque;

  /* initialize lowpass filter */
  double steering_lpf_cutoff_hz, error_deriv_lpf_curoff_hz;
  yamlRead<double>(param_config, "steering_lpf_cutoff_hz", steering_lpf_cutoff_hz, double(3.0));
  yamlRead<double>(param_config, "", error_deriv_lpf_curoff_hz, double(5.0));
  lpf_steering_cmd_.initialize(ctrl_period_, steering_lpf_cutoff_hz);
  lpf_lateral_error_.initialize(ctrl_period_, error_deriv_lpf_curoff_hz);
  lpf_yaw_error_.initialize(ctrl_period_, error_deriv_lpf_curoff_hz);
  lpf_vel_cmd_.initialize(ctrl_period_,steering_lpf_cutoff_hz);

  time_t now = time(nullptr);
  char buf[128] = {0};
    // 格式：年月日_时分秒
  strftime(buf, sizeof(buf), "MPC_%Y%m%d_%H%M%S", localtime(&now));
  string full_path = log_path + string(buf) + ".log";

  ofs_log.open(full_path, ios::out);

  if (!ofs_log.is_open()) {
        cerr << full_path <<"打开日志文件失败！" << endl;
        //return;
  }

}

bool MPCFollowerController::MpcUpdate(double& vel_cmd,double& acc_cmd,double& steer_cmd,double& steer_vel_cmd)
{

  if (vehicle_model_ptr_ == nullptr || qpsolver_ptr_ == nullptr)
  {
    RS_DEBUG << "[MPC] vehicle_model =nullptr  qpsolver_ptr_ == nullptr ";
    return false;
  }

  /* solve MPC */
  auto start = std::chrono::system_clock::now();
  const bool mpc_solved = calculateMPC(vel_cmd, acc_cmd, steer_cmd, steer_vel_cmd);
  //角速度按照ackerman转向根据转角重新计算

  double elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count() * 1.0e-6;
  printf("[MPC] timerCallback: MPC calculating time = %f [ms]\n",elapsed_ms);
  if (!mpc_solved)/*此情况mpc不收敛,需要紧急制动*/
  {
    RS_WARNING << "[MPC] MPC is not solved. publish 0 velocity.";
    steer_cmd = 0.0;
    steer_vel_cmd = 0.0;
  }
 return mpc_solved;

}

bool MPCFollowerController::calculateMPC(double &vel_cmd, double &acc_cmd, double &steer_cmd, double &steer_vel_cmd)
{
  const int N = mpc_param_.prediction_horizon;
  const double DT = mpc_param_.prediction_sampling_time;
  const int DIM_X = vehicle_model_ptr_->getDimX();
  const int DIM_U = vehicle_model_ptr_->getDimU();
  const int DIM_Y = vehicle_model_ptr_->getDimY();
  /* calculate nearest point on reference trajectory (used as initial state) */
  unsigned int nearest_index = 0;
  double yaw_err, dist_err, nearest_traj_time;
  
  double max_yaw_error = amathutils::deg2rad(admisible_yaw_error_deg_ );

  Adsfi::HafPose vehicle_pose,nearest_pose;

  vehicle_pose = m_current_pose;/*将经纬度转换为xy坐标*/
  
  //double current_yaw = MPCUtils::getYaw(vehicle_pose.orientation);
 // double transferYaw = amathutils::normalizeRadian(M_PI/2 - current_yaw);
  double current_yaw = vehicle_pose.orientation.z;
  //current_yaw = amathutils::normalizeRadian(M_PI/2 - current_yaw);
  /*debug 将车辆位置发布显示*/
  convertPosToMarkerAndPublish(vehicle_pose,"currentPos",0,1,0,0.2);

  if (!MPCUtils::calcNearestPoseInterp(ref_traj_, vehicle_pose, nearest_pose, nearest_index, dist_err, 
    yaw_err, nearest_traj_time,max_yaw_error))
  {
	  RS_WARNING << "[MPC] calculateMPC: error in calculating nearest pose. stop mpc.";
    return false;
  };
  /* check if lateral error is not too large */
  if (dist_err > admisible_position_error_ || std::fabs(yaw_err) > amathutils::deg2rad(admisible_yaw_error_deg_ ))
  {
	printf("[MPC] error is over limit, stop mpc. (pos: error = %f[m], limit: %f[m], yaw: error = %f[deg], limit %f[deg])",
             dist_err, admisible_position_error_, amathutils::rad2deg(yaw_err), admisible_yaw_error_deg_);
	/*pid控制器输入*/
    return false;
  }

  /* set mpc initial time */
  const double mpc_start_time = nearest_traj_time;
  /* check trajectory length */
  const double mpc_end_time = mpc_start_time + (N - 1) * DT + mpc_param_.delay_compensation_time + ctrl_period_;
  if (mpc_end_time > ref_traj_.relative_time.back())
  {
    printf("[MPC] path is too short for prediction. path end: %f[s], mpc end time: %f[s]", ref_traj_.relative_time.back(), mpc_end_time);
    return false;
  }
  /* convert tracking x,y error to lat error */
  const double err_x = vehicle_pose.position.x - nearest_pose.position.x;
  const double err_y = vehicle_pose.position.y - nearest_pose.position.y;
  const double sp_yaw = MPCUtils::getYaw(nearest_pose.orientation);
  const double err_lat = -sin(sp_yaw) * err_x + cos(sp_yaw) * err_y;
  /* get steering angle */
  const double steer = m_chassisReport.steerAngle;
  /* define initial state for error dynamics */
  Eigen::VectorXd x0 = Eigen::VectorXd::Zero(DIM_X);
  if (vehicle_model_type_ == "kinematics")
  {
    x0 << err_lat, yaw_err, steer;
  }
  else if (vehicle_model_type_ == "kinematics_no_delay")
  {
    x0 << err_lat, yaw_err;
  }
  else if (vehicle_model_type_ == "dynamics")
  {
    double dot_err_lat = (err_lat - lateral_error_prev_) / ctrl_period_;
    double dot_err_yaw = (yaw_err - yaw_error_prev_) / ctrl_period_;
    printf("[MPC] (before lpf) dot_err_lat = %f, dot_err_yaw = %f", dot_err_lat, dot_err_yaw);
    lateral_error_prev_ = err_lat;
    yaw_error_prev_ = yaw_err;
    dot_err_lat = lpf_lateral_error_.filter(dot_err_lat);
    dot_err_yaw = lpf_yaw_error_.filter(dot_err_yaw);
    printf("[MPC] (after lpf) dot_err_lat = %f, dot_err_yaw = %f", dot_err_lat, dot_err_yaw);
    x0 << err_lat, dot_err_lat, yaw_err, dot_err_yaw;
  }
  else
  {
    RS_ERROR << "vehicle_model_type is undefined";
    return false;
  }
  printf("[MPC] selfpose.x = %f, y = %f, yaw = %f", vehicle_pose.position.x, vehicle_pose.position.y, current_yaw);
  printf("[MPC] nearpose.x = %f, y = %f, yaw = %f", nearest_pose.position.x, nearest_pose.position.y, MPCUtils::getYaw(nearest_pose.orientation));
  printf("[MPC] nearest_index = %d, nearest_traj_time = %f", nearest_index, nearest_traj_time);
  printf("[MPC] lat error = %f, yaw error = %f, steer = %f, sp_yaw = %f, my_yaw = %f", err_lat, yaw_err, steer, sp_yaw, current_yaw);

  
        
  /////////////// delay compensation  ///////////////
  Eigen::MatrixXd Ad(DIM_X, DIM_X);
  Eigen::MatrixXd Bd(DIM_X, DIM_U);
  Eigen::MatrixXd Wd(DIM_X, 1);
  Eigen::MatrixXd Cd(DIM_Y, DIM_X);
  Eigen::MatrixXd Uref(DIM_U, 1);
  Eigen::MatrixXd x_curr = x0;
  double mpc_curr_time = mpc_start_time;
  for (unsigned int i = 0; i < input_buffer_.size(); ++i)
  {
    double k = 0.0;
    double v = 0.0;
    if (!MPCUtils::interp1d(ref_traj_.relative_time, ref_traj_.k, mpc_curr_time, k) ||
        !MPCUtils::interp1d(ref_traj_.relative_time, ref_traj_.vx, mpc_curr_time, v))
    {
      RS_WARNING << "[MPC] calculateMPC: mpc resample error at delay compensation, stop mpc calculation. check code!";
      return false;
    }
    /* get discrete state matrix A, B, C, W */
    vehicle_model_ptr_->setVelocity(v);
    vehicle_model_ptr_->setCurvature(k);
    vehicle_model_ptr_->calculateDiscreteMatrix(Ad, Bd, Cd, Wd, ctrl_period_);
    Eigen::MatrixXd ud = Eigen::MatrixXd::Zero(DIM_U, 1);
    ud(0, 0) = input_buffer_.at(i); // for steering input delay
    x_curr = Ad * x_curr + Bd * ud + Wd;
    mpc_curr_time += ctrl_period_;
  }
  x0 = x_curr; // set delay compensated initial state
  /////////////// generate mpc matrix  /////////////yaw//
  /*
   * predict equation: Xec = Aex * x0 + Bex * Uex + Wex
   * cost function: J = Xex' * Qex * Xex + (Uex - Uref)' * Rex * (Uex - Urefex)
   * Qex = diag([Q,Q,...]), Rex = diag([R,R,...])
   */
  Eigen::MatrixXd Aex = Eigen::MatrixXd::Zero(DIM_X * N, DIM_X);
  Eigen::MatrixXd Bex = Eigen::MatrixXd::Zero(DIM_X * N, DIM_U * N);
  Eigen::MatrixXd Wex = Eigen::MatrixXd::Zero(DIM_X * N, 1);
  Eigen::MatrixXd Cex = Eigen::MatrixXd::Zero(DIM_Y * N, DIM_X * N);
  Eigen::MatrixXd Qex = Eigen::MatrixXd::Zero(DIM_Y * N, DIM_Y * N);
  Eigen::MatrixXd Rex = Eigen::MatrixXd::Zero(DIM_U * N, DIM_U * N);
  Eigen::MatrixXd Urefex = Eigen::MatrixXd::Zero(DIM_U * N, 1);
  /* weight matrix depends on the vehicle model */
  Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(DIM_Y, DIM_Y);
  Eigen::MatrixXd R = Eigen::MatrixXd::Zero(DIM_U, DIM_U);
  Eigen::MatrixXd Q_adaptive = Eigen::MatrixXd::Zero(DIM_Y, DIM_Y);
  Eigen::MatrixXd R_adaptive = Eigen::MatrixXd::Zero(DIM_U, DIM_U);
  Q(0, 0) = mpc_param_.weight_lat_error;
  Q(1, 1) = mpc_param_.weight_heading_error;
  R(0, 0) = mpc_param_.weight_steering_input;
  /* resample ref_traj with mpc sampling time */
  std::vector<double> mpc_time_v;

  for (int i = 0; i < N; ++i)
  {//steer_vel_cmd = m_chassisReport.velocity*std::tan(steer_cmd)/wheelbase_;
    mpc_time_v.push_back(mpc_curr_time + i * DT);
  }
  MPCTrajectory mpc_resampled_ref_traj;
  if (!MPCUtils::interp1dMPCTraj(ref_traj_.relative_time, ref_traj_, mpc_time_v, mpc_resampled_ref_traj))
  {
    RS_WARNING << "[MPC] calculateMPC: mpc resample error, stop mpc calculation. check code!";
    return false;
  }

  /* predict dynamics for N times */
  for (int i = 0; i < N; ++i)
  {
    const double ref_k = mpc_resampled_ref_traj.k[i];
    const double ref_vx = mpc_resampled_ref_traj.vx[i];
    const double ref_vx_squared = ref_vx * ref_vx;

    /* get discrete state matrix A, B, C, W */
    vehicle_model_ptr_->setVelocity(ref_vx);
    vehicle_model_ptr_->setCurvature(ref_k);
    vehicle_model_ptr_->calculateDiscreteMatrix(Ad, Bd, Cd, Wd, DT);

    Q_adaptive = Q;
    R_adaptive = R;
    if (i == N - 1)
    {
      Q_adaptive(0, 0) = mpc_param_.weight_terminal_lat_error;
      Q_adaptive(1, 1) = mpc_param_.weight_terminal_heading_error;
    }
    Q_adaptive(1, 1) += ref_vx_squared * mpc_param_.weight_heading_error_squared_vel_coeff;
    R_adaptive(0, 0) += ref_vx_squared * mpc_param_.weight_steering_input_squared_vel_coeff;

    /* update mpc matrix */
    int idx_x_i = i * DIM_X;
    int idx_x_i_prev = (i - 1) * DIM_X;
    int idx_u_i = i * DIM_U;
    int idx_y_i = i * DIM_Y;
    if (i == 0)
    {
      Aex.block(0, 0, DIM_X, DIM_X) = Ad;
      Bex.block(0, 0, DIM_X, DIM_U) = Bd;
      Wex.block(0, 0, DIM_X, 1) = Wd;
    }
    else
    {
      Aex.block(idx_x_i, 0, DIM_X, DIM_X) = Ad * Aex.block(idx_x_i_prev, 0, DIM_X, DIM_X);
      for (int j = 0; j < i; ++j)
      {
        int idx_u_j = j * DIM_U;
        Bex.block(idx_x_i, idx_u_j, DIM_X, DIM_U) = Ad * Bex.block(idx_x_i_prev, idx_u_j, DIM_X, DIM_U);
      }
      Wex.block(idx_x_i, 0, DIM_X, 1) = Ad * Wex.block(idx_x_i_prev, 0, DIM_X, 1) + Wd;
    }
    Bex.block(idx_x_i, idx_u_i, DIM_X, DIM_U) = Bd;
    Cex.block(idx_y_i, idx_x_i, DIM_Y, DIM_X) = Cd;
    Qex.block(idx_y_i, idx_y_i, DIM_Y, DIM_Y) = Q_adaptive;
    Rex.block(idx_u_i, idx_u_i, DIM_U, DIM_U) = R_adaptive;
    /* get reference input (feed-forward) */
    vehicle_model_ptr_->calculateReferenceInput(Uref);
    if (std::fabs(Uref(0, 0)) < amathutils::deg2rad(mpc_param_.zero_ff_steer_deg))
    {
      Uref(0, 0) = 0.0; // ignore curvature noise
    }
    Urefex.block(i * DIM_U, 0, DIM_U, 1) = Uref;//steer_vel_cmd = m_chassisReport.velocity*std::tan(steer_cmd)/wheelbase_;
    mpc_curr_time += DT;
  }
  /* add lateral jerk : weight for (v * {u(i) - u(i-1)} )^2 */
  for (int i = 0; i < N - 1; ++i)
  {
    const double v = mpc_resampled_ref_traj.vx[i];
    const double lateral_jerk_weight = v * v * mpc_param_.weight_lat_jerk;
    Rex(i, i) += lateral_jerk_weight;
    Rex(i + 1, i) -= lateral_jerk_weight;
    Rex(i, i + 1) -= lateral_jerk_weight;
    Rex(i + 1, i + 1) += lateral_jerk_weight;
  }
  if (Aex.array().isNaN().any() ||Bex.array().isNaN().any() ||
      Cex.array().isNaN().any() || Wex.array().isNaN().any())
  {
    RS_WARNING << "[MPC] calculateMPC: model matrix includes NaN, stop MPC.";
    return false;
  }
  /////////////// optimization ///////////////
  /*
   * solve quadratic optimization.
   * cost function: 1/2 * Uex' * H * Uex + f' * Uex
   */
  const Eigen::MatrixXd CB = Cex * Bex;
  const Eigen::MatrixXd QCB = Qex * CB;
  Eigen::MatrixXd H = Eigen::MatrixXd::Zero(DIM_U * N, DIM_U * N);
  H.triangularView<Eigen::Upper>() = CB.transpose() * QCB; // NOTE: This calculation is very heavy. searching for a good way...
  H.triangularView<Eigen::Upper>() += Rex;
  H.triangularView<Eigen::Lower>() = H.transpose();
  Eigen::MatrixXd f = (Cex * (Aex * x0 + Wex)).transpose() * QCB - Urefex.transpose() * Rex;
  /* constraint matrix : lb < U < ub, lbA < A*U < ubA */
  const double u_lim = amathutils::deg2rad(steer_lim_deg_);
  Eigen::MatrixXd A = Eigen::MatrixXd::Zero(DIM_U * N, DIM_U * N);
  Eigen::MatrixXd lbA = Eigen::MatrixXd::Zero(DIM_U * N, 1);
  Eigen::MatrixXd ubA = Eigen::MatrixXd::Zero(DIM_U * N, 1);
  Eigen::VectorXd lb = Eigen::VectorXd::Constant(DIM_U * N, -u_lim); // min steering angle
  Eigen::VectorXd ub = Eigen::VectorXd::Constant(DIM_U * N, u_lim);  // max steering angle
  auto start = std::chrono::system_clock::now();
  Eigen::VectorXd Uex;
  if (!qpsolver_ptr_->solve(H, f.transpose(), A, lb, ub, lbA, ubA, Uex))
  {
    RS_WARNING << "[MPC] qp solver error";
    return false;
  }
  double elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count() * 1.0e-6;
  printf("[MPC] calculateMPC: qp solver calculation time = %f [ms]", elapsed);
  if (Uex.array().isNaN().any())
  {
    RS_WARNING << "[MPC] calculateMPC: model Uex includes NaN, stop MPC. ";
    return false;
  }
  /* saturation */
  const double u_sat = std::max(std::min(Uex(0), u_lim), -u_lim);
  /* filtering */
  const double u_filtered = lpf_steering_cmd_.filter(u_sat);
  /* set steering command */
  steer_cmd = u_filtered;
  steer_vel_cmd = (Uex(1) - Uex(0)) / DT;
  
  steer_cmd_prev_ = steer_cmd;
  /* save to buffer */
  input_buffer_.push_back(steer_cmd);
  input_buffer_.pop_front();

  printf("[MPC] calculateMPC: mpc steer command raw = %f, filtered = %f, steer_vel_cmd = %f", Uex(0, 0), u_filtered, steer_vel_cmd);
  printf("[MPC] calculateMPC: mpc vel command = %f, acc_cmd = %f", vel_cmd, acc_cmd);


  if (ofs_log.is_open())
  {
    time_t now = time(nullptr);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));

    ofs_log << "[" << timeStr << "] " << "lat error = " <<err_lat <<",yaw error = " << yaw_err;


  }


//  ////////////////// DEBUG ///////////////////
//
//  /* calculate predicted trajectory */
 Eigen::VectorXd Xex = Aex * x0 + Bex * Uex + Wex;
 MPCTrajectory debug_mpc_predicted_traj;
  for (int i = 0; i < N; ++i)
  {
    const double lat_error = Xex(i * DIM_X);
    const double yaw_error = Xex(i * DIM_X + 1);
    const double x = mpc_resampled_ref_traj.x[i] - std::sin(mpc_resampled_ref_traj.yaw[i]) * lat_error;
    const double y = mpc_resampled_ref_traj.y[i] + std::cos(mpc_resampled_ref_traj.yaw[i]) * lat_error;
    const double z = mpc_resampled_ref_traj.z[i];
    debug_mpc_predicted_traj.push_back(x, y, z, mpc_resampled_ref_traj.yaw[i] + yaw_error, 0, 0, 0,0);
  }

  /* publish for visualization */
  //visualization_msgs::Marker marker;
  // convertTrajToMarkersPub(debug_mpc_predicted_traj, pub_debug_predicted_traj_,"predicted_traj", 0, 1, 0, 0.7);
  mdc::visual::Marker marker;
  convertTrajToMarker(debug_mpc_predicted_traj, marker,0.99,0.99,0.99);
  mpc_predicted_trajPub.Publish(marker);
  return true;
}

LateralOutput MPCFollowerController::run(trajectory_follower::InputData const  &input_data) {

    trajectory_follower::LateralOutput lateral_output;
    double vel_cmd = 0.0;
    double acc_cmd = 0.0;
    double steer_cmd = 0.0;
    double steer_vel_cmd = 0.0;
    bool mpc_solved = false;

    m_chassisReport = input_data.current_chassis_report;
    m_current_twist = input_data.current_twist;
    m_current_pose = input_data.current_pose;
    
    MpcUpdateTraj(input_data.current_trajectory);
    mpc_solved = MpcUpdate(vel_cmd,acc_cmd,steer_cmd,steer_vel_cmd);
    lateral_output.sync_data.is_steer_converged = mpc_solved;

    steer_vel_cmd = m_chassisReport.velocity * std::tan(steer_cmd)/wheelbase_;

    steer_vel_cmd = std::clamp(steer_vel_cmd, -1.5, 1.5);

    ofs_log << ",steer_cmd = " <<steer_cmd <<",steer_vel_cmd = " << steer_vel_cmd << ",m_chassisReport.velocity = " << m_chassisReport.velocity << endl;
    // 立即刷新缓冲区，防止掉日志
    ofs_log.flush(); 
    lateral_output.control_cmd.steering_tire_angle = steer_cmd;
    lateral_output.control_cmd.steering_tire_rotation_rate = steer_vel_cmd;
   

    return lateral_output;
}


void MPCFollowerController::MpcUpdateTraj(HafEgoTrajectory curTrajectory)
{
  printf("[MPC] path callback: received path size = %lu", curTrajectory.trajectoryPoints.size());
  MPCTrajectory traj;
  /* calculate relative time */
  std::vector<double> relative_time;
  MPCUtils::calcPathRelativeTime(curTrajectory, relative_time);
  printf("[MPC] path callback: relative_time.size() = %lu, front() = %f, back() = %f",
             relative_time.size(), relative_time.front(), relative_time.back());
  /* resampling */
  MPCUtils::convertWaypointsToMPCTrajWithDistanceResample(curTrajectory, relative_time, traj_resample_dist_, traj);
  MPCUtils::convertEulerAngleToMonotonic(traj.yaw);
  printf("[MPC] path callback: resampled traj size() = %lu", traj.relative_time.size());
  /* path smoothing */
  if (enable_path_smoothing_)
  {
    for (int i = 0; i < path_smoothing_times_; ++i)
    {
      if (!MoveAverageFilter::filt_vector(path_filter_moving_ave_num_, traj.x) ||
          !MoveAverageFilter::filt_vector(path_filter_moving_ave_num_, traj.y) ||
          !MoveAverageFilter::filt_vector(path_filter_moving_ave_num_, traj.yaw) ||
          !MoveAverageFilter::filt_vector(path_filter_moving_ave_num_, traj.vx))
      {
        RS_WARNING << "[MPC] path callback: filtering error. stop filtering";
        return;
      }
    }
  }
  /* calculate yaw angle */
  if (enable_yaw_recalculation_)
  {
    MPCUtils::calcTrajectoryYawFromXY(traj);
    MPCUtils::convertEulerAngleToMonotonic(traj.yaw);
  }
  /* calculate curvature */
  if(traj.size()<curvature_smoothing_num_)
  {
	  RS_WARNING << "traj size num less than curvature_smoothing_num_";
    return;
  }
  MPCUtils::calcTrajectoryCurvature(traj, curvature_smoothing_num_);
  const double max_k = *max_element(traj.k.begin(), traj.k.end());
  const double min_k = *min_element(traj.k.begin(), traj.k.end());
  const double max_yaw = *max_element(traj.yaw.begin(), traj.yaw.end());
  const double min_yaw = *min_element(traj.yaw.begin(), traj.yaw.end());
  printf("[MPC] path callback: trajectory curvature : max_k = %f, min_k = %f,max_yaw:%f,min_yaw:%f\n", max_k, min_k,max_yaw,min_yaw);
  /* add end point with vel=0 on traj for mpc prediction */
  const double mpc_predict_time_length = (mpc_param_.prediction_horizon + 1) * mpc_param_.prediction_sampling_time + mpc_param_.delay_compensation_time + ctrl_period_;
  //const double end_velocity = 0.0;
  //const double end_velocity = current_waypoints_.waypoints.back().twist.twist.linear.x;
  //traj.vx.back() = end_velocity; // also for end point
  traj.push_back(traj.x.back(), traj.y.back(), traj.z.back(), traj.yaw.back(),
                 traj.vx.back(), traj.k.back(),traj.k.back(), traj.relative_time.back() + mpc_predict_time_length);

  if (!traj.size())
  {
	RS_ERROR << "[MPC] path callback: trajectory size is undesired";
    printf("size: x=%lu, y=%lu, z=%lu, yaw=%lu, v=%lu,k=%lu,t=%lu", traj.x.size(), traj.y.size(),
               traj.z.size(), traj.yaw.size(), traj.vx.size(), traj.k.size(), traj.relative_time.size());
    return;
  }
  ref_traj_ = traj;

  mdc::visual::Marker marker;
  convertTrajToMarker(ref_traj_, marker,1.0,0,0);
  mpc_origin_trajPub.Publish(marker);
}



void MPCFollowerController::convertTrajToMarker(MPCTrajectory debug_mpc_predicted_traj,mdc::visual::Marker& marker,
float r,float g,float b)
  {
    mdc::visual::Marker predicted_trajectory_marker;
    predicted_trajectory_marker.header.frameId = "base_link";
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;;
    predicted_trajectory_marker.header.stamp.sec = sec;
    predicted_trajectory_marker.header.stamp.nsec = nsec;
    predicted_trajectory_marker.ns = "local_lane_array_marker_colored";
    predicted_trajectory_marker.type = mdc::visual::MarkerType::LINE_STRIP;
    predicted_trajectory_marker.action = mdc::visual::MarkerAction::ADD;
    predicted_trajectory_marker.scale.x = 0.2;
    predicted_trajectory_marker.scale.y = 0.2;
    //lane_waypoint_marker.scale.z = 0.1;
    predicted_trajectory_marker.frameLocked = false;
    predicted_trajectory_marker.color.r = r;
    predicted_trajectory_marker.color.g = g;
    predicted_trajectory_marker.color.b = b;
    predicted_trajectory_marker.color.a = 1.0;
    for(unsigned int i = 0; i < debug_mpc_predicted_traj.x.size(); i++)
    {
      mdc::visual::Point p;
      p.x = debug_mpc_predicted_traj.x.at(i);
      p.y = debug_mpc_predicted_traj.y.at(i);
      //p.z = safety_rect.at(i).z;
      p.z = 0;

      predicted_trajectory_marker.points.push_back(p);
    }
    marker = predicted_trajectory_marker;
  }

void MPCFollowerController::convertPosToMarkerAndPublish(const Adsfi::HafPose vehicle_pose,
                                      std::string ns, double r, double g, double b, double z)
{
  mdc::visual::Marker marker;
  marker.points.clear();
  //marker.header.frame_id = current_waypoints_.header.frame_id;
  marker.header.frameId = "base_link";
  const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
  uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
  uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;;
  marker.header.stamp.sec = sec;
  marker.header.stamp.nsec = nsec;
  marker.ns = ns;
  marker.id = 0;
  marker.type = mdc::visual::MarkerType::ARROW;
  marker.action = mdc::visual::MarkerAction::ADD;
  marker.scale.x = 1.0;
  marker.scale.y = 1.0;
  marker.scale.z = 1.0;
  marker.color.a = 1.0;
  marker.color.r = r;
  marker.color.g = g;
  marker.color.b = b;

  //geometry_msgs::Point p;
  marker.pose.position.x = vehicle_pose.position.x;
  marker.pose.position.y = vehicle_pose.position.y;
  marker.pose.position.z = 0;
  // p.z = traj.z.at(i) + z;
  //printf("p.x:%f,p.y:%f\n",p.x, p.y);
  //p.z = 0;
  Adsfi::HafQuaternion q = MPCUtils::eulerToQuaternion(vehicle_pose.orientation.x, vehicle_pose.orientation.y, vehicle_pose.orientation.z);
  //marker.points.push_back(p);
  marker.pose.orientation.x = q.x;
  marker.pose.orientation.y = q.y;
  marker.pose.orientation.z = q.z;
  marker.pose.orientation.w = q.w;
  mpc_current_pos_Pub.Publish(marker);
}

bool MPCFollowerController::isReady(
        [[maybe_unused]] const InputData &input_data)
{

    if (input_data.current_trajectory.trajectoryPoints.size() < 3)
    {
        return false;
    }

    return true;
}
}