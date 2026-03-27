
#pragma once
#include <cmath>
#include <chrono>

#include <iostream>
#include <iomanip>
struct PrivatePoint
{
	double x;
	double y;

	PrivatePoint() {
		x = 0.0;
		y = 0.0;
	}
	void Print(const std::string &prefix = "") const {
		std::cout << prefix << " x|y:" << std::setprecision(15) << x << "|" << y << std::endl;
	}

	PrivatePoint(double _x, double _y) {
		x = _x;
		y = _y;
	}


	double Dis(const PrivatePoint &p) {
		return std::hypot(p.x - x, p.y - y);
	}

	bool IsValid() {
		if (fabs(x) < 0.001 ) {
			return false;
		}

		if (fabs(y) < 0.001) {
			return false;
		}
		return true;
	}
};

class VehicleMotionCalculator {
private:
	double wheel_base;       // 轴距(米)
	double steering_ratio;   // 转向传动比
	double current_speed;    // 当前速度(m/s)
	double prev_speed;       // 前次速度(m/s)
	double current_steering; // 当前方向盘转角(度)
	double prev_steering;    // 前次方向盘转角(度)
	double current_timestamp;   // 上次更新时间戳(s)
	double prev_timestamp;

	PrivatePoint pos1, pos2, pos3;

	double yaw_rate;



public:


	VehicleMotionCalculator(double wb = 2, double sr = 16.0)
		: wheel_base(wb), steering_ratio(sr),
		  current_speed(0), prev_speed(0),
		  current_steering(0), prev_steering(0),
		  current_timestamp(getCurrentTime()), prev_timestamp(getCurrentTime()) {}




	// 更新车辆状态
	void updateState(double speed, const double _yaw_rate) {

		double steering = 0;
		// if (UpdatePos(p1) < 0) {
		// 	return ;
		// }


		// if (pos1.IsValid() && pos2.IsValid() && pos3.IsValid()) {

		// 	pos1.Print("pos1");
		// 	pos2.Print("pos2");
		// 	pos3.Print("pos3");
		// 	double radus = calculateTurningRadius(pos1, pos2, pos3);

		// 	std::cout << "radus:" << radus << std::endl;
		// 	steering = calculateSteeringAngle(radus);
		// }

		double radus = 0;
		if (fabs(speed) < 0.1 || fabs(_yaw_rate ) < 0.01 ) {
			radus = -1;
		} else {
			radus = speed / (fabs(_yaw_rate) / 180 * M_PI);
		}


		steering = calculateSteeringAngle(radus);
		std::cout << "speed|yaw_rate|steer:" << speed << "," << _yaw_rate << "," << steering << std::endl;

		double now = getCurrentTime();

		prev_timestamp = current_timestamp;
		current_timestamp = now;

		prev_speed = current_speed;
		current_speed = speed;

		prev_steering = current_steering;
		if (fabs(_yaw_rate ) < 0.01) {
			current_steering = 0.0;
		} else {
			current_steering = steering * (_yaw_rate) / fabs(_yaw_rate);
		}


		yaw_rate = _yaw_rate;


	}

	double GetSteerAngle() {
		return current_steering;
	}

	// 获取方向盘转角速率(deg/s)
	double GetSteeringRate() {
		return (current_steering - prev_steering) / (current_timestamp - prev_timestamp);
	}

	// 获取横摆角速度(rad/s)
	double GetYawRate() {
		return yaw_rate;
	}

	// 获取横向加速度(m/s²)
	double GetLateralAcc() {
		return GetYawRate() * current_speed;
	}

	// 获取纵向加速度(m/s²)
	double GetLongitudinalAcc() {
		return (current_speed - prev_speed) / (current_timestamp - prev_timestamp);
	}



private:
	int UpdatePos(const PrivatePoint& p1) {
		double dis = pos3.Dis(p1);
		// p1.Print();
		// pos1.Print();
		// pos2.Print();
		// pos3.Print();
		std::cout << "dis:" << dis << std::endl;
		if (dis < 0.2) {
			return -1;
		}

		pos1 = pos2;
		pos2 = pos3;
		pos3 = p1;
		return 0;
	}

	double getCurrentTime() {
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
	}


	double calculateTurningRadius(const PrivatePoint& A,
	                              const PrivatePoint& B,
	                              const PrivatePoint& C)
	{
		// 三边长
		double ab = std::hypot(B.x - A.x, B.y - A.y);
		double bc = std::hypot(C.x - B.x, C.y - B.y);
		double ca = std::hypot(A.x - C.x, A.y - C.y);

		// 半周长
		double s = (ab + bc + ca) * 0.5;

		// 面积 K，用海伦公式
		double area2 = s * (s - ab) * (s - bc) * (s - ca); // K^2
		if (area2 <= 0.0) {
			return -1;
			// throw std::invalid_argument("Points are collinear or duplicated.");
		}



		double K = std::sqrt(area2);

		// 外接圆半径
		return (ab * bc * ca) / (4.0 * K);
	}

	// // 计算三点转弯半径
	// double calculateTurningRadius(const PrivatePoint& p1, const PrivatePoint& p2, const PrivatePoint& p3) {
	// 	double area = p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
	// 	if (std::abs(area) < 1e-10) return -1;

	// 	PrivatePoint midAB((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
	// 	PrivatePoint midAC((p1.x + p3.x) / 2, (p1.y + p3.y) / 2);

	// 	double slopeAB = (p2.y - p1.y) / (p2.x - p1.x);
	// 	double slopeAC = (p3.y - p1.y) / (p3.x - p1.x);

	// 	double centerX, centerY;
	// 	if (std::isinf(slopeAB)) {
	// 		centerX = midAB.x;
	// 		centerY = midAC.y + (centerX - midAC.x) / slopeAC;
	// 	} else if (std::isinf(slopeAC)) {
	// 		centerX = midAC.x;
	// 		centerY = midAB.y + (centerX - midAB.x) / slopeAB;
	// 	} else {
	// 		centerX = (midAB.y - midAC.y + (midAC.x / slopeAC) - (midAB.x / slopeAB)) /
	// 		          (1.0 / slopeAC - 1.0 / slopeAB);
	// 		centerY = midAB.y + (centerX - midAB.x) / slopeAB;
	// 	}
	// 	return std::hypot(centerX - p1.x, centerY - p1.y);
	// }

	double calculateSteeringAngle(double radius) {
		if (radius < 0) return 0.0; // 直线行驶
		return steering_ratio * std::atan(wheel_base / radius) * 180.0 / M_PI;
	}


};
