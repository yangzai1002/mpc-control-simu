#include <math.h>
#include "kalman_filter.hh"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

const float PI2 = 2 * M_PI;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;

  Q0_ = Q_in;  // 保存初始Q
  alpha_ = 0.1;
  residual_history_.clear();  // 清空历史残差
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
  //std::cout << "H_: " << H_.size() << "  x_: " << x_.size() << std::endl;
  VectorXd z_pred = H_ * x_;

  VectorXd y = z - z_pred;

  // 保存残差到历史记录，限制长度为6
  residual_history_.push_back(y);
  if (residual_history_.size() > 6) {
    residual_history_.pop_front();
  }

  MatrixXd Ht = H_.transpose();
  MatrixXd PHt = P_ * Ht;
  MatrixXd S = H_ * PHt + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K = PHt * Si;


  // if (residual_history_.size() < 2) {
    // 残差数据不足，使用初始Q
  Q_ = Q0_;
  // }else{
  //   // 计算残差的协方差
  //   VectorXd mean = VectorXd::Zero(residual_history_[0].size());
  //   for (const auto& r : residual_history_) {
  //     mean += r;
  //   }
  //   mean /= residual_history_.size();

  //   MatrixXd cov = MatrixXd::Zero(mean.size(), mean.size());
  //   for (const auto& r : residual_history_) {
  //     VectorXd diff = r - mean;
  //     cov += diff * diff.transpose();
  //   }
  //   cov /= (residual_history_.size() - 1);

  //   // 自适应调整Q：结合初始Q和残差协方差
  //   Q_ = (1 - alpha_) * Q0_ + alpha_ * K * cov * K.transpose();
  // }

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;

}

// template<typename T>
// void KalmanFilter::Update(const T& meas){
//     Eigen::VectorXd z_pred;
// }

Eigen::VectorXd KalmanFilter::PredictTemp(double dt) {
  Eigen::VectorXd x_temp = x_;
    // Modify the F matrix so that the time is integrated
  Eigen::MatrixXd F = F_; 
  F(0, 2) = dt;
  F(1, 3) = dt;
  // std::cout<<"Eigen::VectorXd x_temp "<<x_<<std::endl;
  // std::cout<<"Eigen::VectorXd F_ "<<F<<std::endl;
  x_temp = F * x_temp;
  return x_temp;
}