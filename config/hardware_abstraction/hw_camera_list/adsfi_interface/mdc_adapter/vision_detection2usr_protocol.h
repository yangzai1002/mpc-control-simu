#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>



namespace avos
{
namespace perception
{
enum EIdpVisionDectectionIOType
{
	// init area
	IOCTL_INIT_INTRINSICS_PATH,

	// input area
	IOCTL_SET_IMAGE_LIST_DATA,

	// output area
	IOCTL_GET_VISION_DETECT_DATA,
};

struct SVisionImageData
{
	int height;
	int width;
	std::string camera_name;
	std::string encoding; //"bgr8  rgb8  YUYV422  UYUV422"
	char *data_ptr;
};

}
}