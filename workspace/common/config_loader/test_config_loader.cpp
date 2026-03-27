#include "custom_stack.h"
#include "monitor/faulthandle_api.hpp"


int main(int argc, char const *argv[])
{

	FaultHandle::FaultApi::Instance()->SetFaultCode("23468");
	/* code */
	auto ptr = CustomStack::Instance();
	ptr->LoadAllProjectConfig("/media/idriver/work/tzbu/avos_code/projects/MDC2501MidTracked/config");

	ptr->LoadModuleConfig("/media/idriver/work/tzbu/avos_code/projects/MDC2501MidTracked/Huawei_FR671V2_U-100_bin/config/global.conf");


	std::string str;
	ptr->GetProjectConfigValue("control/latcontrol/latctrl_type", str);

	double value;
	ptr->GetProjectConfigValue("control/latcontrol/k_preview", value);

	std::cout << "str:" << str << "," << value << std::endl;



	std::string module_str;
	int module_int;
	ptr->GetConfig("HuaweiFR671V2:U-1/this", "radar_name", module_str);
	ptr->GetConfig("HuaweiFR671V2:U-1/this", "debug", module_int);
	std::cout << "module_str:" << module_str << "," << module_int << std::endl;


	return 0;
}