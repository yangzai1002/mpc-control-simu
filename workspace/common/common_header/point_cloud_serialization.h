#ifndef __POINT_CLOUD_SERIALZATION_H__
#define __POINT_CLOUD_SERIALZATION_H__
#pragma once

#include <pcl/point_types.h>

#include <array>
#include <string>
#include <iostream>
#include <vector>
#include "point_cloud_types.h"

#define ShmSerializationBuffer  10000000

namespace avos {
namespace driver {
typedef struct ShmSerializationPointCloud
{
	template <class T>
	static int Serialization(std::vector<uint8_t> &buffer, const T &data)
	{
		int serial_size = sizeof(T) / sizeof(uint8_t); // ros::serialization::serializationLength(data);
		buffer.resize(serial_size);
		memcpy((void *)&buffer[0], (void *)&data, sizeof(T));
		return serial_size;
	}

	template <class T>
	static int Serialization(uint8_t *buffer, int buffer_len, const T &data)
	{
		int serial_size = sizeof(T) / sizeof(uint8_t);
		if (serial_size >= buffer_len)
		{
			std::cout << "error buffer is not meet command please check " << serial_size << "|" << buffer_len << std::endl;
			exit(0);
		}
		memcpy((void *)buffer, (void *)&data, sizeof(T));
		return serial_size;
	}

	template <class T>
	static void DeSerialization(T &data, std::vector<uint8_t> &buffer)
	{
		memcpy((void *)&data, (void *)buffer, sizeof(T));
	}

	template <class T>
	static void DeSerialization(T &data, uint8_t *buffer, int buffer_len)
	{
		memcpy((void *)&data, (void *)buffer, sizeof(T));
	}

	uint8_t buffer[ShmSerializationBuffer];

	ShmSerializationPointCloud() {
		memset(buffer, 0, ShmSerializationBuffer);
	}

	int Serialization(const VPointCloud &point_cloud) {
		return Serialization(buffer, ShmSerializationBuffer, point_cloud);
	}

	int Deserialization(VPointCloud &point_cloud) {
		DeSerialization(point_cloud, buffer, ShmSerializationBuffer);
		return 0;
	}
} ShmSerializationPointCloud;

}
}


#endif
