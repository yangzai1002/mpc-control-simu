#pragma once
#include <string>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <functional>
#include <dirent.h>

class Tools
{
public:
	Tools() {

	}
	~Tools() {

	}

	static std::vector<std::string> TraverseDirectory(const std::string& path) {
		std::vector<std::string> res;
		DIR* dir = opendir(path.c_str());
		if (!dir) {
			std::cerr << "无法打开目录: " << path << std::endl;
			return res;
		}

		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			std::string name = entry->d_name;
			if (name.size() >= 9 && name.substr(name.size() - 9) == ".proj_cfg") {
				// std::cout << "file list: " << path + "/" + name << std::endl;
				res.push_back(path + "/" + name);
			}
		}
		closedir(dir);
		return res;
	}

	static int RunTaskPeriodically(int interval_ms, std::function<void()> callback) {
		while (true) {
			auto startTime = std::chrono::high_resolution_clock::now();

			// 调用回调函数
			callback();
			auto endTime = std::chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
			// 等待指定时间
			auto waitTime = std::chrono::milliseconds(interval_ms) - elapsedTime;
			// auto waitTime = std::chrono::seconds(interval_ms) - elapsedTime;
			if (waitTime > std::chrono::milliseconds::zero()) {
				std::this_thread::sleep_for(waitTime);
			}
		}
		return 0;
	}

	static std::string GetFileExtension(const std::string& filename) {

		size_t dot_pos = filename.rfind('.');
		if (dot_pos == std::string::npos) {
			// 没有找到'.'，所以没有扩展名
			return "";

		}
		return filename.substr(dot_pos + 1);
	}

	static double Second() {
		auto now = std::chrono::high_resolution_clock::now();

		// 转换为毫秒数
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		return (double)milliseconds / 1000.0;
	}

	static std::vector<std::string> SplitString(std::string str, char delimiter) {
		std::vector<std::string> result;

		// 找到第一个分隔符的位置
		size_t pos = str.find(delimiter);

		// 从头开始遍历字符串，找到所有分隔符的位置
		while (pos != std::string::npos) {
			// 提取从头到分隔符位置的子字符串
			std::string token = str.substr(0, pos);

			// 将子字符串加入结果数组
			result.push_back(token);

			// 从分隔符位置之后的位置开始继续查找
			str.erase(0, pos + 1);
			pos = str.find(delimiter);
		}

		// 将最后一个分隔符后的子字符串加入结果数组
		result.push_back(str);

		return result;
	}

};
