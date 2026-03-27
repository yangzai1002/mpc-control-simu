#ifndef __MVIZ_INTERFACE_H__
#define __MVIZ_INTERFACE_H__

#include <functional>
#include <string>
#include <chrono>
#include <string>
#include <vector>
#include <type_traits>
#include <random>
#include <publisher.h>
#include "display_types/viz_times.h"
#include "display_types/viz_path.h"

typedef std::function < float (void *)> pColorFunction;

typedef std::function < std::string (void *)> pToStringFunction;

inline mdc::visual::Times GetCurrentTime()
{
	const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
	uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;
	return mdc::visual::Times { sec, nsec };
}



template<typename T, typename = void>
struct has_field : std::false_type {};

template<typename T>
struct has_field<T, std::void_t<decltype(T::field)>> : std::true_type {};



template <class T>
struct RGBPointCloud
{
	mdc::visual::PointCloud<mdc::visual::PointXYZRGB> publish_points;
	mdc::visual::Publisher pcPointXYZRGBPub;
	bool isInitialized;

	std::vector<T> inner_points;
	pColorFunction pget_g_func;
	pColorFunction pget_r_func;
	pColorFunction pget_b_func;
	float DefaultGetGFunc(void*) {
		return 1.0;
	}

	float DefaultGetRFunc(void*) {
		return 1.0;
	}

	float DefaultGetBFunc(void*) {
		return 0.0;
	}
	RGBPointCloud(const std::string &topic_name) {


		bool result = mdc::visual::Connect();
		if (result) {
			isInitialized = true;
			std::cout << "The MViz server address is parsed and the visualization client is created successfully." << std::endl;
		} else {
			isInitialized = false;
			std::cout << "error Failed to parse the MViz server address, or a visualization client has been created." << std::endl;
			return;
		}

		inner_points.clear();
		publish_points.header.frameId = "base_link";
		publish_points.points.clear();
		publish_points.isDense = false;

		pcPointXYZRGBPub =
		    mdc::visual::Publisher::Advertise<mdc::visual::PointCloud2>(ara::core::String(topic_name));


		pget_g_func = std::bind(&RGBPointCloud::DefaultGetGFunc, this, std::placeholders::_1);
		pget_r_func = std::bind(&RGBPointCloud::DefaultGetRFunc, this, std::placeholders::_1);
		pget_b_func = std::bind(&RGBPointCloud::DefaultGetBFunc, this, std::placeholders::_1);
	}
	void Reset() {
		publish_points.points.clear();
		inner_points.clear();
	}
	void SetGFunc(pColorFunction usr_get_g_func) {
		pget_g_func = std::move(usr_get_g_func);
	}
	void SetRFunc(pColorFunction usr_get_r_func) {
		pget_r_func = std::move(usr_get_r_func);
	}
	void SetBFunc(pColorFunction usr_get_b_func) {
		pget_b_func = std::move(usr_get_b_func);
	}
	void publish(const char &color = 'y') {
		if (isInitialized == false) {
			return;
		}
		publish_points.points.clear();
		for (auto &ip : inner_points) {
			mdc::visual::PointXYZRGB temp;
			temp.x = ip.x;
			temp.y = ip.y;
			// if (has_field<T>::z) {
			// 	temp.z = ip.z;
			// } else {
				temp.z = 0;
			// }

			switch (color) {
			case 'y':
				temp.color.channel.b = 0;
				temp.color.channel.g = 255;
				temp.color.channel.r = 255;
				break;
			case 'r':
				temp.color.channel.b = 0;
				temp.color.channel.g = 0;
				temp.color.channel.r = 255;
				break;
			case 'b':
				temp.color.channel.b = 255;
				temp.color.channel.g = 0;
				temp.color.channel.r = 0;
				break;
			case 'g':
				temp.color.channel.b = 0;
				temp.color.channel.g = 255;
				temp.color.channel.r = 0;
				break;
			case 'p':
				temp.color.channel.b = 128;
				temp.color.channel.g = 0;
				temp.color.channel.r = 128;
				break;
			case 'w':
				temp.color.channel.b = 255;
				temp.color.channel.g = 255;
				temp.color.channel.r = 255;
				break;
			default:
				temp.color.channel.b = pget_b_func((void *)&ip) * 255;
				temp.color.channel.g = pget_g_func((void *)&ip) * 255;
				temp.color.channel.r = pget_r_func((void *)&ip) * 255;
			}
			temp.color.channel.a = 128;
			publish_points.points.push_back(temp);
		}

		publish_points.header.stamp = GetCurrentTime();
		pcPointXYZRGBPub.Publish(publish_points);
	}

};

template <class T>
struct VizPath
{
	mdc::visual::Path publish_path;
	mdc::visual::Publisher pathPub;
	bool isInitialized;

	std::vector<T> inner_points;
	pColorFunction pget_g_func;
	pColorFunction pget_r_func;
	pColorFunction pget_b_func;
	
	// 速度阈值，默认 40 km/h = 40.0/3.6 m/s
	double up_speed;
	
	float DefaultGetGFunc(void*) {
		return 1.0;
	}

	float DefaultGetRFunc(void*) {
		return 1.0;
	}

	float DefaultGetBFunc(void*) {
		return 0.0;
	}
	
	// // 根据速度设置颜色
	// void SetColorByVelocity(const T& point, mdc::visual::PoseStamped& temp_pose) {
	// 	double velocity = point.velocity;
		
	// 	if (fabs(velocity) <= 0.001) {
	// 		// 速度为0或接近0：白色
	// 		temp_pose.color.channel.r = 255;
	// 		temp_pose.color.channel.g = 255;
	// 		temp_pose.color.channel.b = 255;
	// 		temp_pose.color.channel.a = 128;
	// 	} else if (velocity < 0) {
	// 		// 速度为负：黑色
	// 		temp_pose.color.channel.r = 0;
	// 		temp_pose.color.channel.g = 0;
	// 		temp_pose.color.channel.b = 0;
	// 		temp_pose.color.channel.a = 128;
	// 	} else if (velocity >= up_speed) {
	// 		// 速度 >= UP_SPEED：红色
	// 		temp_pose.color.channel.r = 255;
	// 		temp_pose.color.channel.g = 0;
	// 		temp_pose.color.channel.b = 0;
	// 		temp_pose.color.channel.a = 128;
	// 	} else if (velocity < up_speed / 2.0) {
	// 		// 速度 < UP_SPEED/2：从绿色渐变到黄色
	// 		double ratio = velocity / (up_speed / 2.0);
	// 		temp_pose.color.channel.r = static_cast<uint8_t>(ratio * 255);
	// 		temp_pose.color.channel.g = 255;
	// 		temp_pose.color.channel.b = 0;
	// 		temp_pose.color.channel.a = 128;
	// 	} else if (velocity < up_speed) {
	// 		// UP_SPEED/2 <= 速度 < UP_SPEED：从黄色渐变到红色
	// 		double ratio = 2.0 - velocity / (up_speed / 2.0);
	// 		temp_pose.color.channel.r = 255;
	// 		temp_pose.color.channel.g = static_cast<uint8_t>(ratio * 255);
	// 		temp_pose.color.channel.b = 0;
	// 		temp_pose.color.channel.a = 128;
	// 	} else {
	// 		// 其他情况：黑色
	// 		temp_pose.color.channel.r = 0;
	// 		temp_pose.color.channel.g = 0;
	// 		temp_pose.color.channel.b = 0;
	// 		temp_pose.color.channel.a = 128;
	// 	}
	// }
	
	VizPath(const std::string &topic_name, double speed_threshold = 40.0 / 3.6) {
		bool result = mdc::visual::Connect();
		if (result) {
			isInitialized = true;
			std::cout << "The MViz server address is parsed and the visualization client is created successfully." << std::endl;
		} else {
			isInitialized = false;
			std::cout << "error Failed to parse the MViz server address, or a visualization client has been created." << std::endl;
			return;
		}

		inner_points.clear();
		publish_path.header.frameId = "base_link";
		publish_path.header.stamp = GetCurrentTime();
		publish_path.poses.clear();
		up_speed = speed_threshold;  // 默认 40 km/h = 40.0/3.6 m/s

		pathPub = mdc::visual::Publisher::Advertise<mdc::visual::Path>(
		    ara::core::String(topic_name));

		pget_g_func = std::bind(&VizPath::DefaultGetGFunc, this, std::placeholders::_1);
		pget_r_func = std::bind(&VizPath::DefaultGetRFunc, this, std::placeholders::_1);
		pget_b_func = std::bind(&VizPath::DefaultGetBFunc, this, std::placeholders::_1);
	}
	
	void Reset() {
		publish_path.poses.clear();
		inner_points.clear();
	}
	
	void SetGFunc(pColorFunction usr_get_g_func) {
		pget_g_func = std::move(usr_get_g_func);
	}
	
	void SetRFunc(pColorFunction usr_get_r_func) {
		pget_r_func = std::move(usr_get_r_func);
	}
	
	void SetBFunc(pColorFunction usr_get_b_func) {
		pget_b_func = std::move(usr_get_b_func);
	}
	
	void SetFrameId(const std::string &frame_id) {
		publish_path.header.frameId = frame_id;
	}
	
	void SetSpeedThreshold(double speed_threshold) {
		up_speed = speed_threshold;
	}
	
	void publish(const char &color = 'v') {
		if (isInitialized == false) {
			return;
		}
		publish_path.poses.clear();
		
		for (auto &ip : inner_points) {
			mdc::visual::PoseStamped temp_pose;
			temp_pose.pose.position.x = ip.x;
			temp_pose.pose.position.y = ip.y;
			temp_pose.pose.position.z = 0;
			// temp_pose.pose.orientation.x = ip.qx;
			// temp_pose.pose.orientation.y = ip.qy;
			// temp_pose.pose.orientation.z = ip.qz;
			// temp_pose.pose.orientation.w = ip.qw;
			
			// 设置颜色
			// 根据速度设置颜色（默认模式）
			// SetColorByVelocity(ip, temp_pose);
			
			temp_pose.header.stamp = GetCurrentTime();
			temp_pose.header.frameId = publish_path.header.frameId;
			
			publish_path.poses.push_back(temp_pose);
		}

		publish_path.header.stamp = GetCurrentTime();
		pathPub.Publish(publish_path);
	}
};


// template <class T>
// struct IPointCloud
// {
// 	mdc::visual::PointCloud<mdc::visual::PointXYZI> publish_points;
// 	mdc::visual::Publisher pcPointXYZIPub;
// 	bool isInitialized;

// 	std::vector<T> inner_points;
// 	pColorFunction pget_i_func;
// 	float DefaultGetIFunc(void*) {
// 		return 1.0;
// 	}

// 	IPointCloud(const std::string &topic_name) {


// 		bool result = mdc::visual::Connect();
// 		if (result) {
// 			isInitialized = true;
// 			std::cout << "The MViz server address is parsed and the visualization client is created successfully." << std::endl;
// 		} else {
// 			isInitialized = false;
// 			std::cout << "error Failed to parse the MViz server address, or a visualization client has been created." << std::endl;
// 			return;
// 		}

// 		inner_points.clear();
// 		publish_points.header.frameId = "base_link";
// 		publish_points.points.clear();
// 		publish_points.isDense = false;

// 		pcPointXYZIPub =
// 		    mdc::visual::Publisher::Advertise<mdc::visual::PointCloud2>(ara::core::String(topic_name));


// 		pget_i_func = std::bind(&IPointCloud::DefaultGetIFunc, this, std::placeholders::_1);
// 	}
// 	void Reset() {
// 		publish_points.points.clear();
// 		inner_points.clear();
// 	}
// 	void SetIFunc(pColorFunction usr_get_i_func) {
// 		pget_i_func = std::move(usr_get_i_func);
// 	}

// 	void publish() {
// 		if (isInitialized == false) {
// 			return;
// 		}
// 		publish_points.points.clear();
// 		for (auto &ip : inner_points) {
// 			mdc::visual::PointXYZI temp;
// 			temp.x = ip.x;
// 			temp.y = ip.y;
// 			// if (HasMemberVar<T>::z) {
// 			temp.z = ip.z;

// 			temp.intensity = pget_i_func();
// 			// // } else {
// 			// // temp.z = 0;
// 			// // }

// 			// switch (color) {
// 			// case 'y':
// 			// 	temp.color.channel.b = 0;
// 			// 	temp.color.channel.g = 255;
// 			// 	temp.color.channel.r = 255;
// 			// 	break;
// 			// case 'r':
// 			// 	temp.color.channel.b = 0;
// 			// 	temp.color.channel.g = 0;
// 			// 	temp.color.channel.r = 255;
// 			// 	break;
// 			// case 'b':
// 			// 	temp.color.channel.b = 255;
// 			// 	temp.color.channel.g = 0;
// 			// 	temp.color.channel.r = 0;
// 			// 	break;
// 			// case 'g':
// 			// 	temp.color.channel.b = 0;
// 			// 	temp.color.channel.g = 255;
// 			// 	temp.color.channel.r = 0;
// 			// 	break;
// 			// case 'p':
// 			// 	temp.color.channel.b = 128;
// 			// 	temp.color.channel.g = 0;
// 			// 	temp.color.channel.r = 128;
// 			// 	break;
// 			// case 'w':
// 			// 	temp.color.channel.b = 255;
// 			// 	temp.color.channel.g = 255;
// 			// 	temp.color.channel.r = 255;
// 			// 	break;
// 			// default:
// 			// 	temp.color.channel.b = pget_b_func((void *)&ip) * 255;
// 			// 	temp.color.channel.g = pget_g_func((void *)&ip) * 255;
// 			// 	temp.color.channel.r = pget_r_func((void *)&ip) * 255;
// 			// }
// 			// temp.color.channel.a = 128;
// 			publish_points.points.push_back(temp);
// 		}

// 		publish_points.header.stamp = GetCurrentTime();
// 		pcPointXYZIPub.Publish(publish_points);
// 	}



// };



// struct TestPointCloud {
// 	double x;
// 	double y;
// 	double z;

// 	TestPointCloud() {
// 		x = 0;
// 		y = 0;
// 		z = 0;
// 	}



// };




// inline double generateDouble(double min, double max) {
// 	std::random_device rd;
// 	std::mt19937 gen(rd());
// 	std::uniform_real_distribution<> dis(min, max);
// 	return dis(gen);
// }


// inline void TestIPointCloud()
// {
// 	static int size = 0;
// 	size++;
// 	if (size > 10) {
// 		size = 0;
// 	}


// 	static RGBPointCloud<TestPointCloud> rgb_cloud("test_cloud");
// 	rgb_cloud.inner_points.clear();
// 	rgb_cloud.inner_points.resize(size * 10);
// 	std::cout << "pub size:"<<size * 10<<std::endl;
// 	for (int i = 0; i < size * 10; i++) {
// 		rgb_cloud.inner_points[i].y = generateDouble(0, 200);
// 	}

// 	rgb_cloud.publish('r');
// }



#endif



