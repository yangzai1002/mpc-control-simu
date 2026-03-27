#pragma once
#include <cmath>
#include <string>
#include <iostream>
#include <limits>


class LatMath {
public:
	LatMath() {

	}
	~LatMath() {

	}
	double GetLength(double x_point1, double y_point1, double x_point2, double y_point2) {
		return std::hypot(x_point1 - x_point2, y_point1 - y_point2);
	}
	double GetDist2Seg(double x, double y, double x1, double y1, double x2, double y2) {
		double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
		if (cross <= 0)
		{
			return (GetLength(x, y, x1, y1));
		}

		double d2 = GetLength(x1, y1, x2, y2);
		if (cross >= d2 * d2)
		{
			return (GetLength(x, y, x2, y2));
		}
		double r = cross / (d2 * d2);
		double px = x1 + (x2 - x1) * r;
		double py = y1 + (y2 - y1) * r;
		double dist2seg = GetLength(x, y, px, py);
		return dist2seg;
	}
	double GetLineDirection(double x_from, double y_from, double x_to, double y_to) {
		double vector_x = x_to - x_from;
		double vector_y = y_to - y_from;
		double vector_norm = sqrt(pow(vector_x, 2) + pow(vector_y, 2));
		double cosvalue = vector_x / vector_norm;
		double line_direction = acos(cosvalue) * 180 / M_PI;
		if (NearEqual(x_from, x_to) && NearEqual(y_from, y_to)) {
			return 0;
		}
		else if (vector_y < 0) {
			return (-line_direction);
		}
		else {
			return (line_direction);
		}
	}
	bool NearEqual(double value1, double value2) {
		if (fabs(value1 - value2) < DMIN_VALUE) {
			return true;
		}
		else {
			return false;
		}
	}
	int Signn(double value) {
		if (value > 0) {
			return 1;
		}
		else {
			return -1;
		}
	}
	double normalize_angle(double angle){
	    if (angle > 180.0){
	        angle -= 2.0 * 180.0;
	    }

	    if (angle < -180.0){
	        angle += 2.0 * 180.0;
	    }
	    return angle;
	}
	const double DMIN_VALUE = std::numeric_limits<double>::min();
	const double DMAX_VALUE = std::numeric_limits<double>::max();
};

