#ifndef __BASE_ADSFI_INTERFACE_H__
#define __BASE_ADSFI_INTERFACE_H__

#include <iostream>
#include <string>

#include "avos_base/avos_codetag/ccodetag_onlyhead.hpp"
#include "converter_interface/cusrap2gsdspubinterface.h"
#include "ap_log/ap_log_interface.h"
#include "shm_util/shm_param.h"
#include "custom_stack.h"

class BaseAdsfiInterface {
public:
    BaseAdsfiInterface() {
        SetScheduleInfo("message");
    }
    virtual ~BaseAdsfiInterface() {
    }

    virtual void Init() {
        std::cout << "base adsfi interface init" << std::endl;
    }

    virtual void DeInit() {
        std::cout << "base adsfi interface DeInit" << std::endl;
    }

    void SetScheduleInfo(const std::string &method, int ms = 100) {
        schedule_method = method;
        timmer_ms = ms;
    }

    void GetScheduleInfo(std::string &method, int &ms) {
        method = schedule_method;
        ms = timmer_ms;
    }

private:
    std::string schedule_method;
    int         timmer_ms;
};

#define ADSFI_TO_SEC(sec, nsec) (double)(sec + (double)nsec / 1e9)
#define ADSFI_GET_SEC(sec) (unsigned int)sec
#define ADSFI_GET_NSEC(sec) (unsigned int)((sec - (unsigned int)sec)*1e9)


#define BASE_TEMPLATE_FUNCION 					\
template<typename T>							\
void Callback(const T &ptr){					\
	std::cout << "base callbak"<<std::endl;		\
}												\
												\
template<typename T>							\
void Callback(const std::string &name, const T &ptr){					\
	std::cout << "recv:" << name << " base callbak"<<std::endl;		\
}												\
												\
												\
template<typename T>							\
int Process(const std::string &topic, T &ptr){	\
	std::cout << "send:" << topic << " base process"<<std::endl;		\
	return -1;									\
}												\
												\
template<typename T>							\
int Process(T &ptr){							\
	std::cout << "base process"<<std::endl;		\
	return -1;									\
}												\




#define PRE_TIMMER_SENTENCE						\
	auto start = std::chrono::high_resolution_clock::now();


#define END_TIMMER_SENTENCE										\
	 auto end = std::chrono::high_resolution_clock::now();		\
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
    if(AdsfiDataManager::Instance()->IsTimmer() && (elapsed >= 0) && (elapsed < AdsfiDataManager::Instance()->GetTimmerMs())) { \
        std::this_thread::sleep_for(std::chrono::milliseconds(AdsfiDataManager::Instance()->GetTimmerMs() - elapsed));			\
    }																															\
    																							\


#define LOAD_ALL_CONFIG															\
    char *node_pathvar = nullptr;													\
	node_pathvar = getenv("ICAOS_NODE_MY_ROOT");											\
	std::cout << "getenv(ICAOS_NODE_MY_ROOT )" << std::string(node_pathvar) << std::endl; \
	if (node_pathvar == nullptr)														\
	{																			\
		std::cout << "please set ICAOS_NODE_MY_ROOT" << std::endl;					\
		exit(0);																\
	}																			\
    std::string usr_config_path = std::string(node_pathvar);									\
    std::cout << "usr node configpath = " << usr_config_path << std::endl;		\
    																			\
    auto ptr = CustomStack::Instance();											\
	ptr->SetNodeConfigPath(usr_config_path);								\
    ptr->LoadModuleConfig(usr_config_path+"/config/global.conf");						\
    																			\
	char *pathvar = nullptr;													\
	pathvar = getenv("PROJ_CFG_PATH");											\
	std::cout << "getenv(PROJ_CFG_PATH )" << std::string(pathvar) << std::endl; \
	if (pathvar == nullptr)														\
	{																			\
		std::cout << "please set PROJ_CFG_PATH" << std::endl;					\
		exit(0);																\
	}																			\
	std::string prefix = std::string(pathvar);									\
	ptr->SetProjectConfigPath(prefix);																			\
	ptr->LoadAllProjectConfig(prefix);											\



#define AVOS_MSGPACK_PUBLISH(node_name, topic, type , value)  \
	int debug_mode = 0;											\
	std::string shm_key = std::string(node_name)+std::string("_mode");				\
	SHMParamApi::Instance()->QueryIntModuleParam(shm_key, debug_mode);			\
	if(ApLogInterface::Instance()->GetDebugMode(node_name) == 0 || debug_mode == 1){		\
		ApInfo("avos_debug")<< "send topic "<< node_name<<" "<< topic<<" "<<type; \
		CUsrAp2GsdsPubInterface::Instance()->publish((std::string(node_name)+"_"+topic), (type), (value));	\
	}		\
    


#endif
