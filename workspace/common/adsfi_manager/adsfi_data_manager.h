#ifndef __ADSFI_DATA_MANAGER_H__
#define __ADSFI_DATA_MANAGER_H__

#include "adsfi_interface/adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "config_loader/custom_stack.h"
#include <map>
#include <string>




class AdsfiDataManager
{

public:
	static AdsfiDataManager* Instance() {
		static AdsfiDataManager* ptr = nullptr;
		if (ptr == nullptr) {
			ptr = new AdsfiDataManager();
		}
		return ptr;
	}

	AdsfiDataManager(const AdsfiDataManager&) = delete;
	AdsfiDataManager& operator=(const AdsfiDataManager&) = delete;

private:
	AdsfiDataManager() {

	}
	~AdsfiDataManager() = default;



public:
	bool IsTimmer() {
		std::string method;
		int ms;
		adsfi_interface.GetScheduleInfo(method, ms);
		if (method == "timmer") {
			return true;
		}
		return false;
	}


	int GetTimmerMs() {
		std::string method;
		int ms;
		adsfi_interface.GetScheduleInfo(method, ms);
		return ms;
	}

public:
	AdsfiInterface adsfi_interface;


};





#endif

