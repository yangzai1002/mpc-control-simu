#include "shm_util/shm_param.h"
#include <sstream>


int main(int argc, char const *argv[])
{
	
	if(argc == 2 && std::string(argv[1]) == "list" ){
		std::string param_name = std::string(argv[3]);
		double data;
		SHMParamApi::Instance()->Print();
		return 0;
	}
	if(argc == 4 && std::string(argv[1]) == "get" && std::string(argv[2]) == "-d"){
		std::string param_name = std::string(argv[3]);
		double data;
		SHMParamApi::Instance()->QueryDoubleModuleParam(param_name, data);
		std::cout << "[get]\t"<<param_name<<":"<<data<<std::endl;
		return 0;
	}

	if(argc == 4 && std::string(argv[1]) == "get" && std::string(argv[2]) == "-s"){
		std::string param_name = std::string(argv[3]);
		std::string data;
		SHMParamApi::Instance()->QueryStringModuleParam(param_name, data);
		std::cout << "[get]\t"<<param_name<<":"<<data<<std::endl;
		return 0;
	}

	if(argc == 4 && std::string(argv[1]) == "get" && std::string(argv[2]) == "-i"){
		std::string param_name = std::string(argv[3]);
		int data;
		SHMParamApi::Instance()->QueryIntModuleParam(param_name, data);
		std::cout << "[get]\t"<<param_name<<":"<<data<<std::endl;
		return 0;
	}


	if(argc == 5 && std::string(argv[1]) == "set" && std::string(argv[2]) == "-d"){
		std::string param_name = std::string(argv[3]);
		std::stringstream ss;
		ss << std::string(argv[4]);
		double data;
		ss >> data;
		SHMParamApi::Instance()->SetDoubleModuleParam(param_name, data);
		std::cout << "[set]\t"<<param_name<<":"<<data<<std::endl;
		return 0;
	}

	if(argc == 5 && std::string(argv[1]) == "set" && std::string(argv[2]) == "-s"){
		std::string param_name = std::string(argv[3]);
		std::string data;
		SHMParamApi::Instance()->SetStringModuleParam(param_name, data);
		std::cout << "[set]\t"<<param_name<<":"<<data<<std::endl;
		return 0;
	}

	if(argc == 5 && std::string(argv[1]) == "set" && std::string(argv[2]) == "-i"){
		std::string param_name = std::string(argv[3]);
		int data = atoi(argv[4]);
		SHMParamApi::Instance()->SetIntModuleParam(param_name, data);
		std::cout << "[set]\t"<<param_name<<":"<<data<<std::endl;
		return 0;
	}



	std::cout << "usage:"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" [list]: list all param"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" get -d [param_name]: get double param"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" get -s [param_name]: get String param"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" get -i [param_name]: get int param"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" set -d [param_name]: set double param"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" set -s [param_name]: set String param"<<std::endl;
	std::cout << "\t"<< std::string(argv[0]) <<" set -i [param_name]: set int param"<<std::endl;

	return 0;
}