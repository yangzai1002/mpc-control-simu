#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_
#include "Eigen/Dense"
#include <iostream>
#include <deque>
class KalmanFilter {
public:

  // state vector
  Eigen::VectorXd x_;

  // state covariance matrix
  Eigen::MatrixXd P_;

  // state transistion matrix
  Eigen::MatrixXd F_;

  // process covariance matrix
  Eigen::MatrixXd Q_;

  // measurement matrix
  Eigen::MatrixXd H_;

  // measurement covariance matrix
  Eigen::MatrixXd R_;

  // 历史残差序列（限制长度为6）
  std::deque<Eigen::VectorXd> residual_history_;

  // 初始过程噪声协方差
  Eigen::MatrixXd Q0_;

  // 自适应调整参数
  double alpha_ = 0.1;  // 学习率


  /**
   * Constructor
   */
  KalmanFilter();

  /**
   * Destructor
   */
  virtual ~KalmanFilter();

  /**
   * Init Initializes Kalman filter
   * @param x_in Initial state
   * @param P_in Initial state covariance
   * @param F_in Transition matrix
   * @param H_in Measurement matrix
   * @param R_in Measurement covariance matrix
   * @param Q_in Process covariance matrix
   */
  void Init(Eigen::VectorXd &x_in, Eigen::MatrixXd &P_in, Eigen::MatrixXd &F_in,
      Eigen::MatrixXd &H_in, Eigen::MatrixXd &R_in, Eigen::MatrixXd &Q_in);

  /**
   * Prediction Predicts the state and the state covariance
   * using the process model
   * @param delta_T Time between k and k+1 in s
   */
  void Predict();

  /**
   * Updates the state by using standard Kalman Filter equations
   * @param z The measurement at k+1
   */
  void Update(const Eigen::VectorXd&);

  Eigen::VectorXd PredictTemp(double dt); 
  Eigen::MatrixXd GetCovariance() { return P_; }

  float GetCovarianceVal(){
      float c_x,c_y,c_w,c_h;
      c_x = P_(0,0);
      c_y = P_(1,1);
      c_w = P_(2,2);
      c_h = P_(3,3);
      return (c_x+c_y+c_w+c_h)/4.0;
  }

  /**
   * 根据残差历史自适应调整过程噪声协方差Q
   */
  void AdaptQ();

};

#endif /* KALMAN_FILTER_H_ */
