#pragma once
#include <iostream>
#include <unistd.h>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "ara/log/logging.h"
#include "debug_config.h"
#include "shm_util/shm_param.h"


class ApLogInterface
{
private:
	ApLogInterface() {
		std::string log_config_path = getLogEnvironmentVariable();
		std::cout << "load log config :" << log_config_path << std::endl;
		if (log_config_info.DeserializationFromFile(log_config_path + "/log.conf") < 0) {
			std::cout << "error get log config path " << log_config_path + "/log.conf" << std::endl;
		}

		// std::string shm_key = p.node_name + "_mode";
		// SHMParamApi::Instance()->SetIntModuleParam(shm_key, 0);

		for (auto &p : log_config_info.debug_info) {
			std::string shm_key = p.node_name + "_mode";
			if (p.debug_mode == 0) {
				SHMParamApi::Instance()->SetIntModuleParam(shm_key, 1);
			} else {
				SHMParamApi::Instance()->SetIntModuleParam(shm_key, 0);
			}
		}
		// log_config_info.Print();
	}


	std::string getLogEnvironmentVariable() {
		const char* value = std::getenv("PROJ_CFG_PATH");
		return value ? std::string(value) : "/opt/usr/log/";
	}
	std::string getCurrentDate() {
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm* now_tm = std::localtime(&now_time);

		std::ostringstream oss;
		oss << std::put_time(now_tm, "%Y%m%d");
		return oss.str();
	}

public:
	void Init(const std::string &node_name) {

		ara::log::LogLevel level;
		std::string log_path = log_config_info.log_save_path;
		std::string cmd = std::string("mkdir -p ") + log_path + "/" + getCurrentDate();
		system(cmd.c_str());
		int log_level = log_config_info.GetLogLevel(node_name);
		switch (log_level) {
		case 0: level = ara::log::LogLevel::kInfo; break;
		case 1: level = ara::log::LogLevel::kWarn; break;
		case 2: level = ara::log::LogLevel::kError; break;
		default: level = ara::log::LogLevel::kWarn; break;
		}
		ara::log::InitLogging(log_config_info.GetLogPrefix(node_name), "", level, ara::log::LogMode::kFile, log_path + "/" + getCurrentDate());
	}


	ara::log::Logger* LogCtx(const std::string &ctx, const ara::log::LogLevel &level = ara::log::LogLevel::kVerbose) {

		auto it = log_ctx_map.find(ctx);
		if (it == log_ctx_map.end()) {
			log_ctx_map[ctx] = &ara::log::CreateLogger(ctx, "", level);
		}

		return log_ctx_map[ctx];
	}

	int GetDebugMode(const std::string &node_name) {
		return log_config_info.GetDebugLevel(node_name);
	}



public:
	static ApLogInterface *Instance() {
		static ApLogInterface * ptr = nullptr;
		if (ptr == nullptr) {
			ptr = new ApLogInterface();
		}
		return ptr;
	}


private:
	std::map<std::string, ara::log::Logger*> log_ctx_map;
	NodeDebugInfoVec log_config_info;

};



#define ApInfo(ctx) ApLogInterface::Instance()->LogCtx(ctx, ara::log::LogLevel::kInfo)->LogInfo()
#define ApWarn(ctx) ApLogInterface::Instance()->LogCtx(ctx, ara::log::LogLevel::kWarn)->LogWarn()
#define ApError(ctx) ApLogInterface::Instance()->LogCtx(ctx, ara::log::LogLevel::kError)->LogError()
#define ApFatal(ctx) ApLogInterface::Instance()->LogCtx(ctx, ara::log::LogLevel::kFatal)->LogFatal()

