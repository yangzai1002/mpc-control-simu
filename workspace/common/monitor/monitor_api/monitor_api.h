#ifndef __AVOS_MONITOR_POOL_H__
#define __AVOS_MONITOR_POOL_H__
#include <iostream>
#include <numeric>
#include <iterator>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <atomic>
#include <memory>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdlib>



#include "time/node_time.h"
#include "shm_util/shared_memory.h"


// #include "monitor_table_config.pb.h"


#define SELF_DRIVER_LABEL "self_drive"
#define DISAABLE_MONITOR_LEVEL_OFFSET  1
namespace avos {
namespace common {


typedef struct SHMFaultInfo
{
#define FAULT_MSG_SIZE 52
#define FAULT_MODULE_NAME_SIZE 52
	double fault_time;
	unsigned int error_code;
	int value;
	char message[FAULT_MSG_SIZE];
	char module_name[FAULT_MODULE_NAME_SIZE];

	SHMFaultInfo() {
		value = 0;
		fault_time = 0.0;
		error_code = 0;
		memset(message, 0, FAULT_MSG_SIZE);
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "module_name|error_code|value|msg:" << GetFaultModuleName() << "|" << std::hex << std::setfill('0') << std::setw(4)  << error_code << "|" << std::dec << value << "|" << GetFaultMessage() << std::endl;
	}


	void SetFaultMessage(const std::string &str) {
		memset(message, 0, FAULT_MSG_SIZE);
		memcpy(message, str.c_str(), str.size() > (FAULT_MSG_SIZE - 1) ? (FAULT_MSG_SIZE - 1) : str.size());
	}

	std::string GetFaultMessage() {
		return message;
	}


	void SetFaultModuleName(const std::string &str) {
		memset(module_name, 0, FAULT_MODULE_NAME_SIZE);
		memcpy(module_name, str.c_str(), str.size() > (FAULT_MODULE_NAME_SIZE - 1) ? (FAULT_MODULE_NAME_SIZE - 1) : str.size());
	}

	std::string GetFaultModuleName() {
		return module_name;
	}
} SHMFaultInfo;


typedef struct SHMFaultCodeBarrel
{
#define BARREL_FAULT_SIZE 40
	std::atomic<int> size;
	SHMFaultInfo fault_info[BARREL_FAULT_SIZE];

	SHMFaultCodeBarrel() {
		size.store(0);
	}

	void Print() {
		for (int i = 0; i < size.load(); i++) {
			fault_info[i].Print();
		}
	}

	void GetAllModuleFaultInfo(std::map<std::string, std::vector<SHMFaultInfo> > &modules_fault_map) {
		for (int i = 0; i < size.load(); i++) {
			if (fault_info[i].value != 0) {
				modules_fault_map[fault_info[i].GetFaultModuleName()].push_back(fault_info[i]);
			}
		}
	}

	void GetModuleFault(const std::string &module_name, std::vector<SHMFaultInfo> &module_info_vec) {
		module_info_vec.clear();
		for (int i = 0; i < size.load(); i++) {
			if (fault_info[i].GetFaultModuleName() == module_name && fault_info[i].value != 0) {
				module_info_vec.push_back(fault_info[i]);
			}
		}
	}
} SHMFaultCodeBarrel;


typedef struct SHMFaultCodePool
{
#define POOL_FAULT_SIZE 50
	double last_update_time;
	int current_fault_size;
	SHMFaultCodeBarrel fault_barrel[POOL_FAULT_SIZE];

	SHMFaultCodePool() {
		last_update_time = 0;
	}

	void UpdateSize() {
		last_update_time = 0;
		for (int i = 0; i < POOL_FAULT_SIZE; i++) {
			last_update_time += fault_barrel[i].size;
		}
	}

	void Print() {
		for (int i = 0; i < POOL_FAULT_SIZE; i++) {
			fault_barrel[i].Print();
		}
	}

	void GetAllModuleFaultInfo(std::map<std::string, std::vector<SHMFaultInfo> > &modules_fault_map) {
		for (int i = 0; i < POOL_FAULT_SIZE; i++) {
			fault_barrel[i].GetAllModuleFaultInfo(modules_fault_map);
		}
	}

	void GetModuleFault(const std::string &module_name, std::vector<SHMFaultInfo> &module_info_vec) {
		module_info_vec.clear();
		for (int i = 0; i < POOL_FAULT_SIZE; i++) {
			std::vector<SHMFaultInfo> barrel_module_info_vec;
			fault_barrel[i].GetModuleFault(module_name, barrel_module_info_vec);
			if (barrel_module_info_vec.size() != 0) {
				module_info_vec.insert(module_info_vec.end(), barrel_module_info_vec.begin(), barrel_module_info_vec.end());
			}
		}
	}

	int GetErrorInfo(unsigned int error_code, SHMFaultInfo &info) {
		std::hash<std::string> hash_fn;
		int index = hash_fn("hash" + std::to_string(error_code) + "end_line") % POOL_FAULT_SIZE;
		for (int i = 0; i < fault_barrel[index].size.load(); i++) {
			const SHMFaultInfo &fault_info = fault_barrel[index].fault_info[i];
			if (fault_info.error_code == error_code) {
				info.fault_time = fault_info.fault_time;
				info.error_code = fault_info.error_code;
				info.value = fault_info.value;
				info.SetFaultMessage(fault_info.message);
				info.SetFaultModuleName(fault_info.module_name);
				return 0;
			}
		}
		return -1;
	}

	int SetFaultItemInfo(double time_stamp, unsigned int error_code, const std::string &str, const std::string &module_name = " ") {
		std::hash<std::string> hash_fn;
		int index = hash_fn("hash" + std::to_string(error_code) + "end_line") % POOL_FAULT_SIZE;
		for (int i = 0; i < fault_barrel[index].size.load(); i++) {
			if (fault_barrel[index].fault_info[i].error_code == error_code) {
				int before_value = fault_barrel[index].fault_info[i].value;
				fault_barrel[index].fault_info[i].value = 1;
				fault_barrel[index].fault_info[i].fault_time = time_stamp;
				fault_barrel[index].fault_info[i].SetFaultMessage(str);
				fault_barrel[index].fault_info[i].SetFaultModuleName(module_name);
				if (before_value == 0) return 1;
				return 0;
			}
		}
		int current_size = fault_barrel[index].size.load();
		fault_barrel[index].size.store(fault_barrel[index].size.load() + 1);
		if (fault_barrel[index].size.load() > BARREL_FAULT_SIZE - 1) {
			std::cout << "fault_barrel is full, please check " << fault_barrel[index].size.load() << " max size is " << BARREL_FAULT_SIZE << std::endl;
			exit(0);
		}

		fault_barrel[index].fault_info[current_size].error_code = error_code;
		fault_barrel[index].fault_info[current_size].value = 1;
		fault_barrel[index].fault_info[current_size].fault_time = time_stamp;
		fault_barrel[index].fault_info[current_size].SetFaultMessage(str);
		fault_barrel[index].fault_info[current_size].SetFaultModuleName(module_name);
		return 1;

	}


	int ResetFaultItemInfo(double time_stamp, unsigned int error_code, const std::string &str = "") {
		std::hash<std::string> hash_fn;
		int index = hash_fn("hash" + std::to_string(error_code) + "end_line") % POOL_FAULT_SIZE;
		for (int i = 0; i < fault_barrel[index].size.load(); i++) {
			if (fault_barrel[index].fault_info[i].error_code == error_code) {
				if (fault_barrel[index].fault_info[i].value == 0) return 0;
				fault_barrel[index].fault_info[i].value = 0;
				fault_barrel[index].fault_info[i].fault_time = time_stamp;
				fault_barrel[index].fault_info[i].SetFaultMessage(str);
				return 1;
			}
		}
		fault_barrel[index].fault_info[fault_barrel[index].size.load()].error_code = error_code;
		fault_barrel[index].fault_info[fault_barrel[index].size.load()].value = 0;
		fault_barrel[index].fault_info[fault_barrel[index].size.load()].fault_time = time_stamp;
		fault_barrel[index].fault_info[fault_barrel[index].size.load()].SetFaultMessage(str);

		fault_barrel[index].size.store(fault_barrel[index].size.load() + 1);
		return 1;
	}
} SHMFaultCodePool;


typedef struct ModuleHeartbeatInfo
{
#define MODULE_NAME_SIZE 52
	char module_name[MODULE_NAME_SIZE];
	int pid;
	int heart_beat_time; // ms
	double last_hear_beat_time;

	ModuleHeartbeatInfo() {
		pid = 0;
		heart_beat_time = 0;
		last_hear_beat_time = 0.0;
		memset(module_name, 0, MODULE_NAME_SIZE);
	}

	void SetModuleName(const std::string &str) {
		memset(module_name, 0, MODULE_NAME_SIZE);
		memcpy(module_name, str.c_str(), str.size() > (MODULE_NAME_SIZE - 1) ? (MODULE_NAME_SIZE - 1) : str.size());
	}

	std::string GetModuleName() {
		return module_name;
	}

} ModuleHearbeatInfo;

typedef struct ModuleHeartbeatPool
{
#define MODULE_MAX_SIZE 100
	std::atomic<int> size;
	ModuleHeartbeatInfo info_pool[MODULE_MAX_SIZE];
	ModuleHeartbeatPool() {
		size.store(0);
	}

	void Init(const std::string module_name, int heart_beat_time) {
		for (int i = 0; i < size.load(); i++) {
			if (info_pool[i].GetModuleName() == module_name) {
				std::cout << "module_name is already regest, please check " << module_name << std::endl;
				// exit(0);
				info_pool[i].heart_beat_time = heart_beat_time;
				return;
			}
		}
		if (size.load() > MODULE_MAX_SIZE - 1) {
			std::cout << "shm module history is full, please check " << size.load() << " max size is " << MODULE_MAX_SIZE ;
			exit(0);
		}
		int index = size.load();
		size.store(size.load() + 1);
		info_pool[index].SetModuleName(module_name);
		info_pool[index].heart_beat_time = heart_beat_time;
		// pid = po
	}

	void UpdateHeartBeat(const std::string module_name) {
		for (int i = 0; i < size.load(); i++) {
			if (info_pool[i].GetModuleName() == module_name) {
				info_pool[i].last_hear_beat_time = avos::common::NodeTime::Now().ToSecond();
				return;
			}
		}

		std::cout << "use  HeartBeat must use Init first" << std::endl;
	}

	void GetAllRegesterdModule(std::vector<ModuleHeartbeatInfo> &modules) {
		modules.clear();
		for (int i = 0; i < size.load(); i++) {
			ModuleHeartbeatInfo temp;
			temp.SetModuleName(info_pool[i].GetModuleName());
			temp.heart_beat_time = info_pool[i].heart_beat_time;
			temp.last_hear_beat_time = info_pool[i].last_hear_beat_time;
			modules.push_back(temp);
		}
	}

	void GetExceptionModule(std::vector<ModuleHeartbeatInfo> &modules) {
		modules.clear();
		double ratio = 5.0;
		for (int i = 0; i < size.load(); i++) {
			if (avos::common::NodeTime::Now().ToSecond() - info_pool[i].last_hear_beat_time > ratio * info_pool[i].heart_beat_time / 1000.0) {
				ModuleHeartbeatInfo temp;
				temp.SetModuleName(info_pool[i].GetModuleName());
				temp.heart_beat_time = info_pool[i].heart_beat_time;
				temp.last_hear_beat_time = info_pool[i].last_hear_beat_time;
				modules.push_back(temp);
			}
		}
	}

} ModuleHeartbeatPool;


typedef struct FaultErrorItem
{
	unsigned int error_code;
	int fault_level;

	FaultErrorItem() {
		error_code = 0;
		fault_level = 0;
	}

	// FaultErrorItem(const avos::common::MonitorFaultItem &item) {
	// 	error_code = item.error_code();
	// 	fault_level = item.fault_level();
	// }


	// void GetFromProto(const avos::common::MonitorFaultItem &item) {
	// 	error_code = item.error_code();
	// 	fault_level = item.fault_level();
	// }

	void Print(const std::string & prefix = "") {
		std::cout << prefix << "error_code|fault_level: 0x" << std::hex << std::setfill('0') << std::setw(4)  << error_code << "|" <<  std::dec << fault_level << std::endl;
	}


} FaultErrorItem;

struct FaultErrorTable
{
#define TABLE_NAME_SIZE 52
#define TABLE_ITEM_SIZE 1000
	char table_name[TABLE_NAME_SIZE];

	std::atomic<int> current_table_fault_level;
	std::atomic<int> size;
	FaultErrorItem fault_error_info[TABLE_ITEM_SIZE];
	FaultErrorTable() {
		memset(table_name, 0, TABLE_NAME_SIZE);
		size.store(0);
		current_table_fault_level.store(0);
	}

	void SetTableName(const std::string &str) {
		memset(table_name, 0, TABLE_NAME_SIZE);
		memcpy(table_name, str.c_str(), str.size() > (TABLE_NAME_SIZE - 1) ? (TABLE_NAME_SIZE - 1) : str.size());
	}

	std::string GetTableName() {
		return table_name;
	}

	void PushFaultErrorItem(const FaultErrorItem & item) {
		int index = size.load();
		if (index > TABLE_ITEM_SIZE - 1) {
			std::cout << "FaultErrorTable is full, please check " << size.load() << " max size is " << TABLE_ITEM_SIZE ;
			exit(0);
		}
		size.store(size.load() + 1);
		fault_error_info[index].error_code = item.error_code;
		fault_error_info[index].fault_level = item.fault_level;
	}

	// void GetFromProto(const avos::common::MonitorFaultErrorTable &table) {
	// 	SetTableName(table.table_name());

	// 	for (int i = 0; i < table.fault_info_size(); i++) {
	// 		PushFaultErrorItem(FaultErrorItem(table.fault_info(i)));
	// 	}
	// }

	int GetFaultLevel( SHMFaultCodePool &shm_fault_pool) {
		int max_fault_level = 0;
		for (int i = 0; i < size.load(); i++) {
			SHMFaultInfo info;
			shm_fault_pool.GetErrorInfo(fault_error_info[i].error_code, info);
			if (info.value != 0) {
				max_fault_level = fault_error_info[i].fault_level > max_fault_level ? fault_error_info[i].fault_level : max_fault_level;
			}
		}
		return max_fault_level;
	}


	void Print(const std::string & prefix = "") {
		std::cout << prefix << "current_table_fault_level:" << current_table_fault_level << std::endl;
		std::cout << prefix << "size:" << size << std::endl;
		std::cout << prefix << "name:" << table_name << std::endl;
		std::cout << prefix << "-FaultTable:" << std::endl;
		for (int i = 0; i < size.load(); i++) {
			fault_error_info[i].Print(prefix + "\t");
		}
	}


};


typedef struct MultiFaultErrorTable
{
#define ERROR_TABLE_SIZE 50
	int size;
	FaultErrorTable error_tables[ERROR_TABLE_SIZE];
	MultiFaultErrorTable() {
		size = 0;
	}

	void AddErrorTable(FaultErrorTable &error_table) {
		if (size > ERROR_TABLE_SIZE - 1) {
			std::cout << "ERROR_TABLE_SIZE is full, please check " << size << " max size is " << ERROR_TABLE_SIZE ;
			exit(0);
		}
		std::string name = error_table.GetTableName();
		std::cout << "name|size:" << name << "|" << size  << std::endl;
		for (int i = 0; i < size; i++) {
			if (error_tables[i].GetTableName() == name) {
				std::cout << "cannot add exist table:" << name << std::endl;
				return;
			}
		}
		error_tables[size].SetTableName(name);
		error_tables[size].current_table_fault_level = 0;
		for (int i = 0; i < error_table.size.load(); i++) {
			error_tables[size].PushFaultErrorItem(error_table.fault_error_info[i]);
		}
		size++;
	}

	// void GetFromProto(const avos::common::MonitorTableConf &table_list) {
	// 	for (int i = 0; i < table_list.table_size(); i++) {
	// 		std::shared_ptr<FaultErrorTable> temp = std::make_shared<FaultErrorTable>();
	// 		temp->GetFromProto(table_list.table(i));
	// 		AddErrorTable(*temp);
	// 	}
	// }

	void Print(const std::string & prefix = "") {
		std::cout << prefix << "table_size:" << size << std::endl;
		std::cout << prefix << "-MultiFaultErrorTable:" << std::endl;
		for (int i = 0; i < size; i++) {
			std::cout << prefix << "----"  << std::endl;
			error_tables[i].Print(prefix + "\t");
		}

	}
} MultiFaultErrorTable;


typedef struct MonitorShmPool
{
	char global_config[1024];
	SHMFaultCodePool shm_fault_pool;
	ModuleHeartbeatPool heart_beat_pool;
	MultiFaultErrorTable fault_table_pool;

} MonitorShmPool;


class MonitorApi
{

public:
	// int InitMonitorTable(const std::string &config_file_name) {
	// 	avos::common::MonitorTableConf config;
	// 	if (GetProtoFromASCIIFile(config_file_name, &config) == false) {
	// 		std::cout << "cannot read config file:" << config_file_name;
	// 		return -1;
	// 	}

	// 	monitor_shm_pool_->fault_table_pool.GetFromProto(config);  //从配置文件中取得错误码，填充错误表
	// 	return 0;
	// }

	// int QueryFaultLevelInTable(unsigned int error_code, const std::string &fault_table_name = SELF_DRIVER_LABEL) {
	// 	for (int i = 0; i < monitor_shm_pool_->fault_table_pool.size; i++) {
	// 		if (monitor_shm_pool_->fault_table_pool.error_tables[i].GetTableName() != fault_table_name) {
	// 			continue;
	// 		}

	// 		for (int j = 0; j < monitor_shm_pool_->fault_table_pool.error_tables[i].size.load(); j++) {
	// 			if (error_code == monitor_shm_pool_->fault_table_pool.error_tables[i].fault_error_info[j].error_code) {
	// 				return monitor_shm_pool_->fault_table_pool.error_tables[i].fault_error_info[j].fault_level;
	// 			}
	// 		}
	// 	}
	// 	return 0;
	// }


	void InitMonitor(const std::string &module_name, int time_setting_ms) {

		std::cout << "InitMonitor module_name = " << module_name << std::endl;
		char* ros_host_name = getenv("BOARD_IP");
		if (ros_host_name)
		{
			current_module_name_ = std::string(ros_host_name) + ":" + module_name;
		} else {
			current_module_name_ = std::string("127.0.0.1") + ":" + module_name;
		}
		std::cout << "current_module_name_ = " << current_module_name_ << std::endl;
		if (time_setting_ms > 0) {
			monitor_shm_pool_->heart_beat_pool.Init(current_module_name_, time_setting_ms);
		}

		std::vector<SHMFaultInfo> module_info_vec;
		GetModuleFault(module_info_vec);
		for (int i = 0; i < module_info_vec.size(); i++) {
			ResetFaultInfo(0, module_info_vec[i].error_code);
		}
	}

	void EnableMonitorLevel() {
		monitor_shm_pool_->global_config[DISAABLE_MONITOR_LEVEL_OFFSET] = 0;
	}

	void DisableMonitorLevel() {
		monitor_shm_pool_->global_config[DISAABLE_MONITOR_LEVEL_OFFSET] = 1;
	}

	int MonitorLevelStatus() {
		return (int)monitor_shm_pool_->global_config[DISAABLE_MONITOR_LEVEL_OFFSET];
	}

	void HeartBeat() {
		monitor_shm_pool_->heart_beat_pool.UpdateHeartBeat(current_module_name_);
	}

	void GetExceptionModule(std::vector<ModuleHeartbeatInfo> &modules) {
		monitor_shm_pool_->heart_beat_pool.GetExceptionModule(modules);
	}

	void GetAllRegesterdModule(std::vector<ModuleHeartbeatInfo> &modules) {
		monitor_shm_pool_->heart_beat_pool.GetAllRegesterdModule(modules);
	}

	void GetAllFaultErrorItem(std::vector<FaultErrorItem> &items) {
		items.clear();
		for (int i = 0; i < monitor_shm_pool_->fault_table_pool.size; i++) {
			for (int j = 0; j < monitor_shm_pool_->fault_table_pool.error_tables[i].size; j++) {
				items.emplace_back(monitor_shm_pool_->fault_table_pool.error_tables[i].fault_error_info[j]);
			}
		}
	}

	void GetAllFaultInfo(std::vector<SHMFaultInfo> &infos) {
		infos.clear();
		for (int i = 0; i < POOL_FAULT_SIZE; i++) {
			for (int j = 0; j < monitor_shm_pool_->shm_fault_pool.fault_barrel[i].size.load(); j++) {
				infos.emplace_back(monitor_shm_pool_->shm_fault_pool.fault_barrel[i].fault_info[j]);
			}
		}
	}

	void SetFaultInfo(double time_stamp, unsigned int error_code, const std::string &str) {
		monitor_shm_pool_->shm_fault_pool.SetFaultItemInfo(time_stamp, error_code, str, current_module_name_);
		// if (monitor_shm_pool_->shm_fault_pool.SetFaultItemInfo(time_stamp, error_code, str, current_module_name_)) {
		// 	std::stringstream ss;
		// 	ss << "set" << "\t" << GetCurrentTimeStr() << "\t" << std::setprecision(10) << time_stamp << "\t" << current_module_name_ << "\t 0x" << std::hex << std::setfill('0') << std::setw(4) << error_code << "\t" << std::dec << str;
		// 	lock_log_ptr->write(ss.str());
		// }
	}

	void SetFaultInfo(double time_stamp, unsigned int error_code, const std::string &str, const std::string &_module_name) {
		monitor_shm_pool_->shm_fault_pool.SetFaultItemInfo(time_stamp, error_code, str, _module_name);
		// if (monitor_shm_pool_->shm_fault_pool.SetFaultItemInfo(time_stamp, error_code, str, _module_name)) {
		// 	std::stringstream ss;
		// 	ss << "set" << "\t" << GetCurrentTimeStr() << "\t" << std::setprecision(10) << time_stamp << "\t" << _module_name << "\t 0x" << std::hex << std::setfill('0') << std::setw(4) << error_code << "\t" << std::dec << str;
		// 	lock_log_ptr->write(ss.str());
		// }
	}

	int GetFaultInfo(unsigned int error_code, SHMFaultInfo &info) {
		return monitor_shm_pool_->shm_fault_pool.GetErrorInfo(error_code, info);
	}

	void ResetFaultInfo(double time_stamp, unsigned int error_code, const std::string &str = "") {
		monitor_shm_pool_->shm_fault_pool.ResetFaultItemInfo(time_stamp, error_code, str);
		//if (monitor_shm_pool_->shm_fault_pool.ResetFaultItemInfo(time_stamp, error_code, str)) {
		//	std::stringstream ss;
		//	ss << "reset" << "\t" << GetCurrentTimeStr() << "\t" << std::setprecision(10) << time_stamp << "\t" << current_module_name_ << "\t 0x" << std::hex << std::setfill('0') << std::setw(4) << error_code << "\t" << std::dec << str;
		//	lock_log_ptr->write(ss.str());
		//}
	}

	void GetAllModuleFaultInfo(std::map<std::string, std::vector<SHMFaultInfo> > &modules_fault_map) {
		modules_fault_map.clear();
		std::vector<ModuleHeartbeatInfo> modules;
		GetAllRegesterdModule(modules);
		for (int i = 0; i < modules.size(); i++) {
			modules_fault_map[modules[i].GetModuleName()].clear();
		}
		monitor_shm_pool_->shm_fault_pool.GetAllModuleFaultInfo(modules_fault_map);
	}

	void GetModuleFault( std::vector<SHMFaultInfo> &module_info_vec, std::string module_name = "") {
		module_info_vec.clear();
		if (module_name == "") {
			module_name = current_module_name_;
		}
		std::cout << "module_name.size = " << module_name.size()
		          << ",module_name = " << module_name
		          << ",current_module_name_ = " << current_module_name_
		          << std::endl;
		monitor_shm_pool_->shm_fault_pool.GetModuleFault(module_name, module_info_vec);
	}

	void PrintModuleFault(std::string module_name = "") {
		std::vector<SHMFaultInfo> module_info_vec;
		if (module_name == "") {
			module_name = current_module_name_;
		}
		GetModuleFault( module_info_vec, module_name);
		for (int i = 0; i < module_info_vec.size(); i++) {
			module_info_vec[i].Print();
		}
	}

	void GetErrorItem(std::vector<SHMFaultInfo> &vec_error_item) {
		vec_error_item.clear();
		for (int i = 0; i < POOL_FAULT_SIZE; i++) {
			for (int j = 0; j < monitor_shm_pool_->shm_fault_pool.fault_barrel[i].size.load(); j++) {
				if (monitor_shm_pool_->shm_fault_pool.fault_barrel[i].fault_info[j].value == 1) {
					vec_error_item.emplace_back(monitor_shm_pool_->shm_fault_pool.fault_barrel[i].fault_info[j]);
				}
			}
		}
	}


	int GetSelfDriveTableLevel() {
		return GetFaultTableLevel(SELF_DRIVER_LABEL);
	}

	int GetFaultTableLevel(std::string table_name) {
		if (monitor_shm_pool_->global_config[DISAABLE_MONITOR_LEVEL_OFFSET] == 1) {
			// AINFO_EVERY(20) << "[monitor_api] getlevel is disable " ;
			return 0;
		}
		for (int i = 0; i < monitor_shm_pool_->fault_table_pool.size; i++) {
			if (monitor_shm_pool_->fault_table_pool.error_tables[i].GetTableName() == table_name) {
				return monitor_shm_pool_->fault_table_pool.error_tables[i].GetFaultLevel(monitor_shm_pool_->shm_fault_pool);
			}
		}
		return 0;
	}

	void GetAutomaticInitTable112(std::vector<unsigned int> &init_error_codes) {
		init_error_codes.clear();
		for (int i = 0; i < monitor_shm_pool_->fault_table_pool.size; i++) {
			if (monitor_shm_pool_->fault_table_pool.error_tables[i].GetTableName() == "automatic_init_112") {
				// std::cout << "monitor_shm_pool_->fault_table_pool.error_tables[i].size: " << monitor_shm_pool_->fault_table_pool.error_tables[i].size << std::endl;
				for (int j = 0; j < monitor_shm_pool_->fault_table_pool.error_tables[i].size; j++) {
					init_error_codes.push_back(monitor_shm_pool_->fault_table_pool.error_tables[i].fault_error_info[j].error_code);
				}
			}
		}
	}
	void GetAutomaticInitTable113(std::vector<unsigned int> &init_error_codes) {
		init_error_codes.clear();
		for (int i = 0; i < monitor_shm_pool_->fault_table_pool.size; i++) {
			if (monitor_shm_pool_->fault_table_pool.error_tables[i].GetTableName() == "automatic_init_113") {
				// std::cout << "monitor_shm_pool_->fault_table_pool.error_tables[i].size: " << monitor_shm_pool_->fault_table_pool.error_tables[i].size << std::endl;
				for (int j = 0; j < monitor_shm_pool_->fault_table_pool.error_tables[i].size; j++) {
					init_error_codes.push_back(monitor_shm_pool_->fault_table_pool.error_tables[i].fault_error_info[j].error_code);
				}
			}
		}
	}
	void GetAutomaticInitTable115(std::vector<unsigned int> &init_error_codes) {
		init_error_codes.clear();
		for (int i = 0; i < monitor_shm_pool_->fault_table_pool.size; i++) {
			if (monitor_shm_pool_->fault_table_pool.error_tables[i].GetTableName() == "automatic_init_115") {
				// std::cout << "monitor_shm_pool_->fault_table_pool.error_tables[i].size: " << monitor_shm_pool_->fault_table_pool.error_tables[i].size << std::endl;
				for (int j = 0; j < monitor_shm_pool_->fault_table_pool.error_tables[i].size; j++) {
					init_error_codes.push_back(monitor_shm_pool_->fault_table_pool.error_tables[i].fault_error_info[j].error_code);
				}
			}
		}
	}

	void Print() {
		monitor_shm_pool_->shm_fault_pool.Print();
	}


	void PrintFaultTable() {
		monitor_shm_pool_->fault_table_pool.Print();
	}

	~MonitorApi() {
		std::cout << "Uninit MonitorApi" << std::endl;
		avos::common::SharedMemory::sharedMemoryDetatch(monitor_shm_pool_);
		monitor_shm_pool_ = nullptr;
		avos::common::ShmInfo info;
		if (avos::common::SharedMemory::sharedMemoryInfo(current_shmid, info) < 0) {
			std::cout << "cannot get shmid info " << current_shmid << std::endl;
			exit(0);
		}

		if (info.shm_nattach == 0) {
			avos::common::SharedMemory::sharedMemoryDelete(current_shmid);
			std::cout << "delete shm" << std::endl;
		}
	}

private:
	// template <typename MessageType>
	// bool GetProtoFromASCIIFile(const std::string &file_name, MessageType *message) {
	// 	using google::protobuf::io::ZeroCopyInputStream;
	// 	using google::protobuf::io::FileInputStream;
	// 	using google::protobuf::TextFormat;
	// 	int file_descriptor = open(file_name.c_str(), O_RDONLY);
	// 	if (file_descriptor < 0) {
	// 		std::cout << "Failed to open file " << file_name;
	// 		// Failed to open;
	// 		return false;
	// 	}

	// 	ZeroCopyInputStream *input = new FileInputStream(file_descriptor);
	// 	bool success = TextFormat::Parse(input, message);
	// 	if (!success) {
	// 		std::cout << "Failed to parse file " << file_name;
	// 	}
	// 	delete input;
	// 	close(file_descriptor);
	// 	return success;
	// }

	bool ExeLinuxCmd(const std::string& cmd) {
		pid_t status = system(cmd.c_str());
		if (-1 == status) {
			return false;
		}

		if (WIFEXITED(status)) {
			if (0 == WEXITSTATUS(status)) {
				return true;
			}

			std::cout << "ExeLinuxCmdBlock " << cmd << "error returned " << WEXITSTATUS(status);
			return false;
		}
		return true;
	}

	MonitorApi() {
		std::cout << "api init" << std::endl;
		ExeLinuxCmd("ipcs -m | awk '{if($6==0) print $2,$4}' | grep 666 | awk '{print $1}' | xargs -I {} ipcrm -m {}");
		current_shmid = avos::common::SharedMemory::sharedMemoryCreateOrGet(avos::common::SharedMemory::BKDRHash("monitorapi_avos_fault_monitor_pool2"), sizeof(MonitorShmPool) / 4096 * 4096 + 4096);
		std::cout << "shmid:" << current_shmid << std::endl;
		if (current_shmid == -1) {
			std::cout << "4 error to alloc current_shmid " << current_shmid << std::endl;
			exit(0);
		}

		current_module_name_ = "";
		monitor_shm_pool_ = (MonitorShmPool *)avos::common::SharedMemory::sharedMemoryAttach(current_shmid);
	}

	std::string GetCurrentTimeStr()
	{
		static time_t timep;
		time (&timep);
		static char tmp[64];
		memset(tmp, 0, sizeof(tmp));
		strftime(tmp, sizeof(tmp), "%Y%m%d-%H%M%S", localtime(&timep) );
		return tmp;
	}




public:
	static MonitorApi* Instance() {
		static MonitorApi *instance = nullptr;
		if (!instance)
		{
			instance = new MonitorApi();
		}
		return instance;
	}



	MonitorApi(const MonitorApi &) = delete;
	MonitorApi &operator=(const MonitorApi &) = delete;



private:

	MonitorShmPool *monitor_shm_pool_;
	std::string current_module_name_;
	int current_shmid;

};



}
}


#endif