#ifndef __INNO_RADAR_SHM_PARAM_H__
#define __INNO_RADAR_SHM_PARAM_H__


#include <string>
#include "shm_util/shm_param.h"

class RadarShmParam
{
public:
	RadarShmParam() {

	}
	~RadarShmParam() {

	}


	static void SetRadarModuleParam(const std::string &node_name, int prerotaxis_0, int prerotaxis_1, int prerotaxis_2,
	                                double prerotdegree_0, double prerotdegree_1, double prerotdegree_2,
	                                double tf_x, double tf_y, double tf_z,
	                                double roll, double pitch, double yaw) {
		SHMParamApi::Instance()->AddModuleParam("global");
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_prerotaxis_0", prerotaxis_0);
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_prerotaxis_1", prerotaxis_1);
		SHMParamApi::Instance()->SetIntModuleParam(node_name + "_prerotaxis_2", prerotaxis_2);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "prerotdegree_0", prerotdegree_0);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "prerotdegree_1", prerotdegree_1);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "prerotdegree_2", prerotdegree_2);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "_xoffset", tf_x);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "_yoffset", tf_y);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "_zoffset", tf_z);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "_roll", roll);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "_pitch", pitch);
		SHMParamApi::Instance()->SetDoubleModuleParam(node_name + "_yaw", yaw);
	}


	static void GetRadarModuleParam(const std::string &node_name, int &prerotaxis_0, int &prerotaxis_1, int &prerotaxis_2,
	                                double &prerotdegree_0, double &prerotdegree_1, double &prerotdegree_2,
	                                double &tf_x, double &tf_y, double &tf_z,
	                                double &roll, double &pitch, double &yaw) {
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_prerotaxis_0", prerotaxis_0);
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_prerotaxis_1", prerotaxis_1);
		SHMParamApi::Instance()->QueryIntModuleParam(node_name + "_prerotaxis_2", prerotaxis_2);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "prerotdegree_0", prerotdegree_0);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "prerotdegree_1", prerotdegree_1);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "prerotdegree_2", prerotdegree_2);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "_xoffset", tf_x);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "_yoffset", tf_y);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "_zoffset", tf_z);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "_roll", roll);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "_pitch", pitch);
		SHMParamApi::Instance()->QueryDoubleModuleParam(node_name + "_yaw", yaw);
	}


};

#endif
