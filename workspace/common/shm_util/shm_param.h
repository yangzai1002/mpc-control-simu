#ifndef __AVOS_SHM_PARAM_H__
#define __AVOS_SHM_PARAM_H__

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <string.h>
#include "shared_memory.h"
typedef struct DoubleParam
{
#define DOUBLE_KEY_MSG_SIZE 52
	char double_key[DOUBLE_KEY_MSG_SIZE];
	double double_value;

	DoubleParam() {
		double_value = 0.0;
		memset(double_key, 0, DOUBLE_KEY_MSG_SIZE);
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "double_key|double_value:" << GetKey() << "|" << double_value << std::endl; // std::hex << std::setfill('0') << std::setw(4)  << error_code << "|" << std::dec << value << "|" << GetFaultMessage() << std::endl;
	}


	std::string GetKey() {
		return double_key;
	}


	void SetKey(const std::string &str) {
		memset(double_key, 0, DOUBLE_KEY_MSG_SIZE);
		memcpy(double_key, str.c_str(), str.size() > (DOUBLE_KEY_MSG_SIZE - 1) ? (DOUBLE_KEY_MSG_SIZE - 1) : str.size());
	}

} DoubleParam;


typedef struct IntParam
{
#define INT_KEY_MSG_SIZE 52
	char int_key[INT_KEY_MSG_SIZE];
	int int_value;

	IntParam() {
		int_value = 0.0;
		memset(int_key, 0, INT_KEY_MSG_SIZE);
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "int_key|int_value:" << GetKey() << "|" << int_value << std::endl; // std::hex << std::setfill('0') << std::setw(4)  << error_code << "|" << std::dec << value << "|" << GetFaultMessage() << std::endl;
	}


	std::string GetKey() {
		return int_key;
	}


	void SetKey(const std::string &str) {
		memset(int_key, 0, INT_KEY_MSG_SIZE);
		memcpy(int_key, str.c_str(), str.size() > (INT_KEY_MSG_SIZE - 1) ? (INT_KEY_MSG_SIZE - 1) : str.size());
	}

} IntParam;



typedef struct StringParam
{
#define STRING_KEY_MSG_SIZE 52
#define STRING_VALUE_SIZE 512
	char string_key[STRING_KEY_MSG_SIZE];
	char string_value[STRING_VALUE_SIZE];

	StringParam() {
		memset(string_key, 0, STRING_KEY_MSG_SIZE);
		memset(string_value, 0, STRING_VALUE_SIZE);
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "string_key|string_value:" << GetKey() << "|" << GetValue() << std::endl;
	}


	std::string GetKey() {
		return string_key;
	}


	void SetKey(const std::string &str) {
		memset(string_key, 0, STRING_KEY_MSG_SIZE);
		memcpy(string_key, str.c_str(), str.size() > (STRING_KEY_MSG_SIZE - 1) ? (STRING_KEY_MSG_SIZE - 1) : str.size());
	}

	std::string GetValue() {
		return string_value;
	}


	void SetValue(const std::string &str) {
		memset(string_value, 0, STRING_VALUE_SIZE);
		memcpy(string_value, str.c_str(), str.size() > (STRING_VALUE_SIZE - 1) ? (STRING_VALUE_SIZE - 1) : str.size());
	}

} StringParam;


typedef struct ParamPool {
#define MODULE_MSG_SIZE 52
#define AREA_SIZE 50

	char module_name[MODULE_MSG_SIZE];
	int double_area_size;
	DoubleParam double_param_area[AREA_SIZE];
	int int_area_size;
	IntParam int_param_area[AREA_SIZE];

	int string_area_size;
	StringParam string_param_area[AREA_SIZE];

	ParamPool() {
		memset(module_name, 0, MODULE_MSG_SIZE);
		double_area_size = 0;
		int_area_size = 0;
		string_area_size = 0;
	}

	bool SetAddStringContent(const std::string &key, const  std::string &value ) {

		if (SetStringContent(key, value) == true) {
			return true;
		}
		return AddStringParam(key, value);
	}

	bool QueryStringContent(const std::string &key, std::string &value ) {
		for (int i = 0; i < string_area_size; i++) {
			if (string_param_area[i].GetKey() == key) {
				value = string_param_area[i].GetValue();
				return true;
			}
		}
		return false;
	}

	bool SetStringContent(const std::string &key, const  std::string &value ) {
		for (int i = 0; i < string_area_size; i++) {
			if (string_param_area[i].GetKey() == key) {
				string_param_area[i].SetValue(value);
				return true;
			}
		}
		return false;
	}

	bool SetAddDoubleContent(const std::string &key, const double &value ) {

		if (SetDoubleContent(key, value) == true) {
			return true;
		}
		return AddDoubleParam(key, value);
	}
	bool QueryDoubleContent(const std::string &key, double &value ) {
		for (int i = 0; i < double_area_size; i++) {
			if (double_param_area[i].GetKey() == key) {
				value = double_param_area[i].double_value;
				return true;
			}
		}
		return false;
	}

	bool SetDoubleContent(const std::string &key, const double &value ) {
		for (int i = 0; i < double_area_size; i++) {
			if (double_param_area[i].GetKey() == key) {
				double_param_area[i].double_value = value;
				return true;
			}
		}
		return false;
	}


	bool SetAddIntContent(const std::string &key, const int &value ) {

		if (SetIntContent(key, value) == true) {
			return true;
		}
		return AddIntParam(key, value);
	}

	bool QueryIntContent(const std::string &key, int &value ) {
		for (int i = 0; i < int_area_size; i++) {
			if (int_param_area[i].GetKey() == key) {
				value = int_param_area[i].int_value;
				return true;
			}
		}
		return false;
	}

	bool SetIntContent(const std::string &key, const int &value ) {
		for (int i = 0; i < int_area_size; i++) {
			if (int_param_area[i].GetKey() == key) {
				int_param_area[i].int_value = value;
				return true;
			}
		}
		return false;
	}

	bool AddDoubleParam(const std::string &key, double value) {
		if (double_area_size > AREA_SIZE - 5) {
			return false;
		}

		double_param_area[double_area_size].SetKey(key);
		double_param_area[double_area_size].double_value = value;
		double_area_size++;
		return true;
	}

	bool AddIntParam(const std::string &key, int value) {
		if (int_area_size > AREA_SIZE - 5) {
			return false;
		}

		int_param_area[int_area_size].SetKey(key);
		int_param_area[int_area_size].int_value = value;
		int_area_size++;
		return true;
	}

	bool AddStringParam(const std::string &key, const  std::string &value) {
		if (string_area_size > AREA_SIZE - 5) {
			return false;
		}

		string_param_area[string_area_size].SetKey(key);
		string_param_area[string_area_size].SetValue(value);// = value;
		string_area_size++;
		return true;
	}


	void Print(const std::string &prefix = "") {
		std::cout << prefix << "module_name:" << GetModuleName()  << std::endl;
		std::cout << prefix << "double_area_size:" << double_area_size << std::endl;
		for (int i = 0; i < double_area_size; i++) {
			double_param_area[i].Print(prefix + "\t");
		}

		std::cout << prefix << "int_area_size:" << int_area_size << std::endl;
		for (int i = 0; i < int_area_size; i++) {
			int_param_area[i].Print(prefix + "\t");
		}

		std::cout << prefix << "string_area_size:" << string_area_size << std::endl;
		for (int i = 0; i < string_area_size; i++) {
			string_param_area[i].Print(prefix + "\t");
		}

	}

	std::string GetModuleName() {
		return module_name;
	}


	void SetModuleName(const std::string &str) {
		memset(module_name, 0, MODULE_MSG_SIZE);
		memcpy(module_name, str.c_str(), str.size() > (MODULE_MSG_SIZE - 1) ? (MODULE_MSG_SIZE - 1) : str.size());
	}

} ParamPool;


typedef struct MultiModuleShmPool {
#define MODULE_AREA_SIZE 1000
	int module_pool_size;
	ParamPool param[MODULE_AREA_SIZE];

	MultiModuleShmPool() {
		module_pool_size = 0;
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "module_pool_size:" << module_pool_size << std::endl;
		for (int i = 0; i < module_pool_size; i++) {
			param[i].Print(prefix + "\t");
		}
	}

	bool AddModuleParam(const std::string &module_name) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return true;
			}
		}
		if (module_pool_size > MODULE_AREA_SIZE - 3) {
			return false;
		}
		param[module_pool_size].SetModuleName(module_name);
		module_pool_size++;
		return true;
	}

	bool SetDoubleModuleParam(const std::string &module_name, const std::string &key, const double value) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return param[i].SetAddDoubleContent(key, value);
			}
		}
		return false;
	}

	bool SetIntModuleParam(const std::string &module_name, const std::string &key, const int value) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return param[i].SetAddIntContent(key, value);
			}
		}
		return false;
	}

	bool SetStringModuleParam(const std::string &module_name, const std::string &key, const std::string value) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return param[i].SetAddStringContent(key, value);
			}
		}
		return false;
	}


	bool QueryDoubleModuleParam(const std::string &module_name, const std::string &key, double &value) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return param[i].QueryDoubleContent(key, value);
			}
		}
		return false;
	}

	bool QueryIntModuleParam(const std::string &module_name, const std::string &key, int &value) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return param[i].QueryIntContent(key, value);
			}
		}
		return false;
	}

	bool QueryStringModuleParam(const std::string &module_name, const std::string &key, std::string &value) {
		for (int i = 0; i < module_pool_size; i++) {
			if (param[i].GetModuleName() == module_name) {
				return param[i].QueryStringContent(key, value);
			}
		}
		return false;
	}

} MultiModuleShmPool;


class SHMParamApi
{

public:


	~SHMParamApi() {
		std::cout << "Uninit SHMParamApi" << std::endl;
		avos::common::SharedMemory::sharedMemoryDetatch(shm_param_api_);
		shm_param_api_ = nullptr;
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

public:
	bool AddModuleParam(const std::string &module_name) {
		return shm_param_api_->AddModuleParam(module_name);
	}

	bool SetDoubleModuleParam( const std::string &key, const double value) {
		return shm_param_api_->SetDoubleModuleParam("global", key, value);
	}

	bool SetIntModuleParam( const std::string &key, const int value) {
		return shm_param_api_->SetIntModuleParam("global", key, value);
	}

	bool SetStringModuleParam( const std::string &key, const std::string value) {
		return shm_param_api_->SetStringModuleParam("global", key, value);
	}


	bool QueryDoubleModuleParam( const std::string &key, double &value) {
		return shm_param_api_->QueryDoubleModuleParam("global", key, value);
	}

	bool QueryIntModuleParam( const std::string &key, int &value) {
		return shm_param_api_->QueryIntModuleParam("global", key, value);
	}

	bool QueryStringModuleParam( const std::string &key, std::string &value) {
		return shm_param_api_->QueryStringModuleParam("global", key, value);
	}



	bool SetDoubleModuleParam(const std::string &module_name, const std::string &key, const double value) {
		return shm_param_api_->SetDoubleModuleParam(module_name, key, value);
	}

	bool SetIntModuleParam(const std::string &module_name, const std::string &key, const int value) {
		return shm_param_api_->SetIntModuleParam(module_name, key, value);
	}

	bool SetStringModuleParam(const std::string &module_name, const std::string &key, const std::string value) {
		return shm_param_api_->SetStringModuleParam(module_name, key, value);
	}


	bool QueryDoubleModuleParam(const std::string &module_name, const std::string &key, double &value) {
		return shm_param_api_->QueryDoubleModuleParam(module_name, key, value);
	}

	bool QueryIntModuleParam(const std::string &module_name, const std::string &key, int &value) {
		return shm_param_api_->QueryIntModuleParam(module_name, key, value);
	}

	bool QueryStringModuleParam(const std::string &module_name, const std::string &key, std::string &value) {
		return shm_param_api_->QueryStringModuleParam(module_name, key, value);
	}

	void Print(const std::string &prefix = "") {
		shm_param_api_->Print(prefix);
	}

private:


	bool ExeLinuxCmd(const std::string& cmd) {
		pid_t status = system(cmd.c_str());
		if (-1 == status) {
			return false;
		}

		if (WIFEXITED(status)) {
			if (0 == WEXITSTATUS(status)) {
				return true;
			}

			std::cout << "ExeLinuxCmdBlock " << cmd << "error returned " << WEXITSTATUS(status) << std::endl;
			return false;
		}
		return true;
	}

	SHMParamApi() {
		// ExeLinuxCmd("ipcs -m | awk '{if($6==0) print $2,$4}' | grep 666 | awk '{print $1}' | xargs -I {} ipcrm -m {}");
		// std::cout << "SHMParamApi" << std::endl;
		current_shmid = avos::common::SharedMemory::sharedMemoryCreateOrGet(avos::common::SharedMemory::BKDRHash("avos_shm_param_pool"), sizeof(MultiModuleShmPool) / 4096 * 4096 + 4096);
		// std::cout << "shmid:" << current_shmid << std::endl;
		if (current_shmid == -1) {
			std::cout << "3 error to alloc current_shmid " << current_shmid << std::endl;
			exit(0);
		}

		shm_param_api_ = (MultiModuleShmPool *)avos::common::SharedMemory::sharedMemoryAttach(current_shmid);
		AddModuleParam("global");
	}

	// BASE_DECLARE_SINGLETON(SHMParamApi)
public:
	static SHMParamApi *Instance() {
		static SHMParamApi *instance = nullptr;
		if (!instance) {
			instance = new  SHMParamApi();

		}
		return instance;
	}

private:
	SHMParamApi(const SHMParamApi &) = delete;
	SHMParamApi &operator=(const SHMParamApi &) = delete;



private:
	MultiModuleShmPool *shm_param_api_;
	int current_shmid;

};



#endif

