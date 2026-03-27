#ifndef __AVOS_DRIVER_CVBRIDGE_H__
#define __AVOS_DRIVER_CVBRIDGE_H__

// #include "sensor_msgs/msg/image.hpp"
// #include "sensor_msgs/msg/compressed_image.hpp"
#include "common_proto/compress_image.pb.h"
#include "common_proto/sensor_image.pb.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string.h>
// #include "opencv2/highgui/highgui.hpp"
// #include "rclcpp/rclcpp.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>


typedef std::shared_ptr<cv::Mat> CVImagePtr;
typedef std::shared_ptr<cv::Mat const> CVImageConstPtr;
namespace avos {
typedef struct ImageBuffer
{
#define IMAGE_BUFFER_LEN 1024*1024*25	// 10 -> 17 for 3840 * 2160 * 2
	char image1[IMAGE_BUFFER_LEN];
	char image2[IMAGE_BUFFER_LEN];
	char image3[IMAGE_BUFFER_LEN];
	char image4[IMAGE_BUFFER_LEN];
	ImageBuffer() {
		memset(image1, 0 , IMAGE_BUFFER_LEN);
		memset(image2, 0 , IMAGE_BUFFER_LEN);
		memset(image3, 0 , IMAGE_BUFFER_LEN);
		memset(image4, 0 , IMAGE_BUFFER_LEN);
	}

	void SingleSerialzation( char *des, const cv::Mat &mat, double time = 0) {
		// std::cout << "size:" << mat.rows << "|" << mat.cols << "|" << mat.dataend - mat.datastart << std::endl;
		des[0] = mat.type();
		*(int*)(&des[4]) = mat.rows;
		*(int*)(&des[4 + sizeof(int)]) = mat.cols;
		*(double*)(&des[4 + 2 * sizeof(int)]) = time;

		int data_size;
		if (mat.type() == CV_8UC1 ) {
			data_size = 1;
		} else if (mat.type() == CV_8UC3) {
			data_size = 3;
		} else if (mat.type() == CV_16SC1) {
			data_size = 2;
		} else if (mat.type() == CV_8UC4) {
			data_size = 4;
		} else {
			std::cout << "[SingleDeSerialzation] error decode name " << (int)des[0] << std::endl;
			return;
		}

		// std::cout << "mat.rows: " << mat.rows
		// 			<< ", mat.cols: " << mat.cols
		// 			<< ", data_size: " << data_size << std::endl;

		memcpy(&des[4 + 2 * sizeof(int) + sizeof(double)], mat.data, mat.rows * mat.cols * data_size);
	}

	void SingleDeSerialzation(cv::Mat &mat, double &time, char *des ) {
		int height = *(int*)(&des[4]);
		int width = *(int*)(&des[4 + sizeof(int)]);
		time = *(double*)(&des[4 + 2 * sizeof(int)]);
		int data_size;
		if (des[0] == CV_8UC1 ) {
			mat.create( height, width, CV_8UC1);
			data_size = 1;
		} else if (des[0] == CV_8UC3) {
			mat.create( height, width, CV_8UC3);
			data_size = 3;
		} else if (des[0] == CV_16SC1) {
			mat.create( height, width,  CV_16SC1);
			data_size = 2;
		} else if (des[0] == CV_8UC4) {
			mat.create( height, width,  CV_8UC4);
			data_size = 4;
		} else {
			std::cout << "[SingleDeSerialzation] error decode name " << (int)des[0] << std::endl;
			return;
		}
		// std::cout << "height: " << height
		// 			<< ", width: " << width
		// 			<< ", data_size: " << data_size << std::endl;
		memcpy((char *)mat.data, (char *)&des[4 + 2 * sizeof(int) + sizeof(double)], height * width * data_size);
	}

	void Serialization(const cv::Mat &mat, double time) {
		SingleSerialzation(image1, mat, time);
	}

	void Serialization(const cv::Mat &mat1, const cv::Mat &mat2, double time) {
		SingleSerialzation(image1, mat1, time);
		SingleSerialzation(image2, mat2, time);
	}

	void Serialization(const cv::Mat &mat1, const cv::Mat &mat2,
	                   const cv::Mat &mat3, double time) {
		SingleSerialzation(image1, mat1, time);
		SingleSerialzation(image2, mat2, time);
		SingleSerialzation(image3, mat3, time);
	}

	void Serialization(const cv::Mat &mat1, const cv::Mat &mat2,
	                   const cv::Mat &mat3, const cv::Mat &mat4, double time) {
		SingleSerialzation(image1, mat1, time);
		// std::cout << "SingleSerialzation 2" << std::endl;
		SingleSerialzation(image2, mat2, time);
		// std::cout << "SingleSerialzation 3" << std::endl;
		SingleSerialzation(image3, mat3, time);
		// std::cout << "SingleSerialzation 4" << std::endl;
		SingleSerialzation(image4, mat4, time);
	}

	void Deserialization(cv::Mat &mat1, double &time) {
		SingleDeSerialzation(mat1, time, image1);
	}

	void Deserialization(cv::Mat &mat1, cv::Mat &mat2, double &time) {
		SingleDeSerialzation(mat1,  time, image1);
		SingleDeSerialzation(mat2, time, image2);
	}

	void Deserialization(cv::Mat &mat1, cv::Mat &mat2,
	                     cv::Mat &mat3, double &time) {
		SingleDeSerialzation(mat1, time, image1);
		SingleDeSerialzation(mat2, time, image2);
		SingleDeSerialzation(mat3,  time, image3);
	}


	void Deserialization(cv::Mat &mat1, cv::Mat &mat2,
	                     cv::Mat &mat3, cv::Mat &mat4, double &time) {
		SingleDeSerialzation(mat1, time, image1);
		SingleDeSerialzation(mat2, time, image2);
		SingleDeSerialzation(mat3,  time, image3);
		SingleDeSerialzation(mat4,  time, image4);
	}

} ImageBuffer;

static unsigned char static_buf[25 * 1024 * 1024];
class CVBridge
{
private:
public:
	CVBridge()
	{
		// tmp_buf = (unsigned char *)malloc(1024 * 1024 * 25);
	}
	~CVBridge()
	{
	}
	// static unsigned char tmp_buf[1024 * 1024 * 25];
	static void CVMatToImage(const cv::Mat &source, avos::sensor_msgs::Image &image)
	{
		int buf_size = sizeof(static_buf);
		// std::cout << "buf_size = " << buf_size << std::endl;
		// image.mutable_header()->CopyFrom(header);
		image.set_height(source.rows);
		image.set_width(source.cols);
		std::string encoding;
		encoding = mat_type2encoding(source.type());
		image.set_encoding(encoding);
		// image.set_is_bigendian();
		int data_size;
		if (source.type() == CV_8UC1)
		{
			data_size = 1;
		}
		else if (source.type() == CV_8UC3)
		{
			data_size = 3;
		}
		else if (source.type() == CV_16SC1)
		{
			data_size = 2;
		}
		else if (source.type() == CV_8UC4)
		{
			data_size = 4;
		}
		else
		{
			std::cout << "[CVMatToImage] error source.type() " << source.type() << std::endl;
			return;
		}
		image.set_step(data_size * source.cols); // Full row length in bytes
		// std::vector<uchar> tmp_buf(1024*1024*25);

		memcpy(static_buf, source.data, source.rows * source.cols * data_size);
		// std::string tmp_str(25 * 1024 * 1024);
		// tmp_str.copy(source.data, source.rows * source.cols * data_size);
		// std::string tmp = std::string(reinterpret_cast<const char *>(static_buf), source.rows * source.cols * data_size);
		// std::cout << "tmp.size() " << tmp.size() << std::endl;
		image.set_data(std::string(reinterpret_cast<const char *>(static_buf), source.rows * source.cols * data_size));
	}

	static void ImageToCVMat(const avos::sensor_msgs::Image &image, cv::Mat &mat)
	{
		int height = image.height();
		int width = image.width();

		int data_size;
		if (image.encoding() == "mono8")
		{
			mat.create(height, width, CV_8UC1);
			data_size = 1;
		}
		else if (image.encoding() == "bgr8")
		{
			mat.create(height, width, CV_8UC3);
			data_size = 3;
		}
		else if (image.encoding() == "mono16")
		{
			mat.create(height, width, CV_16SC1);
			data_size = 2;
		}
		else if (image.encoding() == "rgba8")
		{
			mat.create(height, width, CV_8UC4);
			data_size = 4;
		}
		else
		{
			std::cout << "[ImageToCVMat] error image.encoding:  " << image.encoding() << std::endl;
			return;
		}

		memcpy(mat.data, image.data().data(), height * width * data_size);
	}

	static void CVToJPEGCompressedImage(const cv::Mat &source, avos::sensor_msgs::CompressedImage &image)
	{
		std::vector<uchar> data_encode;
		imencode(".jpeg", source, data_encode);
		image.clear_data();
		image.set_format("jpeg");
		// data_encode.swap(image.data());
		// image.mutable_data()->CopyFrom({data_encode.begin(), data_encode.end()});
		// std::string *pData = image.mutable_data();
		// memcpy(pData->data(), data_encode[0], data_encode.size());
		// data_encode.swap((unsigned char)image.data().data());
		// image.data()
		std::string tmp_str;
		tmp_str.insert(tmp_str.begin(), data_encode.begin(), data_encode.end());
		image.set_data(tmp_str);
	}

	static void CVToPNGCompressedImage(const cv::Mat &source, avos::sensor_msgs::CompressedImage &image)
	{
		std::vector<uchar> data_encode;
		imencode(".png", source, data_encode);
		image.clear_data();
		image.set_format("png");
		// data_encode.swap(image.data);
		// image.mutable_data()->CopyFrom({data_encode.begin(), data_encode.end()});
		// memcpy(image.data().c_str(), data_encode[0], data_encode.size());
		std::string tmp_str;
		tmp_str.insert(tmp_str.begin(), data_encode.begin(), data_encode.end());
		image.set_data(tmp_str);
	}

	static void CompressedImageToCVmat(const avos::sensor_msgs::CompressedImage &source, cv::Mat &image)
	{
		// image = cv::imdecode(source.data(), cv::IMREAD_UNCHANGED);
#ifdef _L4SSL_PROJECT_
        cv::Mat one_image(1,source.data().length(), CV_8UC2);
#else
        cv::Mat one_image(1,source.data().length(), CV_8U);
#endif
		memcpy(one_image.data, source.data().data(), source.data().length());
		image = cv::imdecode(one_image, cv::IMREAD_UNCHANGED);
	}

	static std::string mat_type2encoding(int mat_type)
	{
		switch (mat_type)
		{
		case CV_8UC1:
			return "mono8";
		case CV_8UC3:
			return "bgr8";
		case CV_16SC1:
			return "mono16";
		case CV_8UC4:
			return "rgba8";
		default:
			return "";
		}
	}
#if 0
	static CVImagePtr toCvCopy(const avos::common::Image::SharedPtr source) {
		CVImagePtr data_ptr = std::make_shared<cv::Mat>();
		int data_size;
		if (source->encoding == "mono8") {
			data_ptr->create(source->height, source->width, CV_8UC1);
			data_size = 1;
		} else if (source->encoding == "bgr8") {
			data_ptr->create(source->height, source->width, CV_8UC3);
			data_size = 3;
		} else if (source->encoding == "mono16") {
			data_ptr->create(source->height, source->width, CV_16SC1);
			data_size = 2;
		} else if (source->encoding == "rgba8") {
			data_ptr->create(source->height, source->width, CV_8UC4);
			data_size = 4;
		} else {
			std::cout << "[cv_bridge] error decode name " << source->encoding << std::endl;
			return data_ptr;
		}
		memcpy((char *)data_ptr->data, (char *)&source->data[0], source->height * source->width * data_size);
		return data_ptr;
	}


	static CVImagePtr toCvShare(const avos::common::Image::SharedPtr source) {
		CVImagePtr data_ptr = std::make_shared<cv::Mat>();
		if (source->encoding == "mono8") {
			data_ptr->create(source->height, source->width, CV_8UC1);
		} else if (source->encoding == "bgr8") {
			data_ptr->create(source->height, source->width, CV_8UC3);
		} else if (source->encoding == "mono16") {
			data_ptr->create(source->height, source->width, CV_16SC1);
		} else if (source->encoding == "rgba8") {
			data_ptr->create(source->height, source->width, CV_8UC4);
		} else {
			std::cout << "[cv_bridge] error decode name " << source->encoding << std::endl;
			return data_ptr;
		}
		data_ptr->data = &(source->data[0]);
		return data_ptr;
	}


	static sensor_msgs::msg::Image::SharedPtr toImageMsg(CVImageConstPtr mat) {
		sensor_msgs::msg::Image::SharedPtr image_msg(new sensor_msgs::msg::Image());
		// image_msg->header.stamp = now();
		// image_msg->header.frame_id = "base_link";
		image_msg->height = mat->rows;
		image_msg->width = mat->cols;
		image_msg->encoding = CVBridge::mat_type2encoding(mat->type());
		image_msg->is_bigendian = false;
		int data_size = 1;
		if (mat->type() == CV_8UC1 ) {
			data_size = 1;
		} else if (mat->type() == CV_8UC3) {
			data_size = 3;
		} else if (mat->type() == CV_16SC1) {
			data_size = 2;
		} else if (mat->type() == CV_8UC4) {
			data_size = 4;
		} else {
			std::cout << "[SingleDeSerialzation] error decode name " << (int)mat->type() << std::endl;
			// return;
		}
		image_msg->step = data_size * image_msg->width;
		image_msg->data.resize(image_msg->height * image_msg->width * data_size);
		memcpy((char *)&image_msg->data[0], (char *)&mat->data[0], image_msg->height * image_msg->width * data_size );
		return image_msg;
	}

	static sensor_msgs::msg::Image::SharedPtr toImageMsg(const cv::Mat &mat) {
		sensor_msgs::msg::Image::SharedPtr image_msg(new sensor_msgs::msg::Image());
		// image_msg->header.stamp = now();
		// image_msg->header.frame_id = "base_link";
		image_msg->height = mat.rows;
		image_msg->width = mat.cols;
		image_msg->encoding = CVBridge::mat_type2encoding(mat.type());
		image_msg->is_bigendian = false;
		// image_msg->step = static_cast<sensor_msgs::msg::Image::_step_type>(mat.step);

		// std::cout << "cv_bridge: " << mat.type()  << ", encoding: " <<image_msg->encoding << std::endl;

		int data_size = 1;
		if (mat.type() == CV_8UC1 ) {
			data_size = 1;
		} else if (mat.type() == CV_8UC3) {
			data_size = 3;
		} else if (mat.type() == CV_16SC1) {
			data_size = 2;
		} else if (mat.type() == CV_8UC4) {
			data_size = 4;
		} else {
			std::cout << "[SingleDeSerialzation] error decode name " << (int)mat.type() << std::endl;
			// return;
		}

		// std::cout << "step: " << data_size << std::endl;
		image_msg->step = data_size * image_msg->width;
		image_msg->data.resize(image_msg->height * image_msg->width * data_size);
		memcpy((char *)&image_msg->data[0], (char *)&mat.data[0], image_msg->height * image_msg->width * data_size );
		// std::cout << "mat.step:" << mat.step << "|" << mat.step * mat.rows << "|" << mat.dataend - mat.datastart << std::endl;
		// image_msg->data.assign(mat.datastart, mat.dataend);
		return image_msg;
	}
#endif
};
}

#endif
