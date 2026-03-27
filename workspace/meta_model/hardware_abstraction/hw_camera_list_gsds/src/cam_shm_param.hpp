#ifndef __INNO_CAM_R_SHM_PARAM_H__
#define __INNO_CAM_R_SHM_PARAM_H__


#include <string>
//#include "param_module.h"
// #include "common/xzmqcommon/params/param_module.h"
#include "shm_util/shm_param.h"
#define  MAX_CAM_NUM  14
class CameraShmParam
{
public:
	CameraShmParam() {

	}
	~CameraShmParam() {

	}


	static void SetCameraModuleParam(const std::string &node_name, int main_idx, int nv_camera_pos, std::string channel_en) {
		SHMParamApi::Instance()->AddModuleParam("global");
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_main_idx", main_idx);
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_nv_camera_pos", nv_camera_pos); //0:主，即前。 1:从，即后。默认为0。
		SHMParamApi::Instance()->SetStringModuleParam(node_name + "_channel_en", channel_en); //需要cp图像数据的所有channel，形式为“0101”的字符串，
																								//每一位依次对应各个channel_id[0~15]。长度不足的
																								//认为后续channel都enable，长度超出的忽略。
																								//未接相机的channel配置为0，不允许调过不配置。
	}


	static void GetCameraModuleParam(const std::string &node_name, int &main_idx, int &nv_camera_pos, std::string &channel_en) {
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_main_idx", main_idx);
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_nv_camera_pos", nv_camera_pos);
		SHMParamApi::Instance()->QueryStringModuleParam(node_name + "_channel_en", channel_en);
	}


};

#endif
