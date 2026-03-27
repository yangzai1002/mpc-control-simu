#ifndef __INNO_CAM_R_SHM_PARAM_H__
#define __INNO_CAM_R_SHM_PARAM_H__


#include <string>
#include "param_module.h"

class CameraShmParam
{
public:
	CameraShmParam() {

	}
	~CameraShmParam() {

	}


	static void SetCameraModuleParam(const std::string &node_name, int main_idx, int nv_camera_pos) {
		SHMParamApi::Instance()->AddModuleParam("global");
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_main_idx", main_idx);
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_nv_camera_pos", nv_camera_pos); //0:主，即前。 1:从，即后。默认为0。
	}

	static void SetCameraModuleMainParam(const std::string &node_name, int main_idx) {
		SHMParamApi::Instance()->AddModuleParam("global");
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_main_idx", main_idx);
	}

	static void GetCameraModuleParam(const std::string &node_name, int &main_idx, int &nv_camera_pos) {
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_main_idx", main_idx);
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_nv_camera_pos", nv_camera_pos);
	}


};

#endif
