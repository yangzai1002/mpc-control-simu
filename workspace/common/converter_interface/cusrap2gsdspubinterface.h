#ifndef __CUSR_AP2GSDSPUB_INTERFACE_H__
#define __CUSR_AP2GSDSPUB_INTERFACE_H__

#include <map>
#include <string>

#include "cidpabstractplatforminterface.h"
#include "cap2gsdspubprotocol.h"


class CUsrAp2GsdsPubInterface
{

public:
	static CUsrAp2GsdsPubInterface* Instance() {
		static CUsrAp2GsdsPubInterface* ptr = nullptr;
		if (ptr == nullptr) {
			ptr = new CUsrAp2GsdsPubInterface();
		}
		return ptr;
	}

	CUsrAp2GsdsPubInterface(const CUsrAp2GsdsPubInterface&) = delete;
	CUsrAp2GsdsPubInterface& operator=(const CUsrAp2GsdsPubInterface&) = delete;

private:
	CUsrAp2GsdsPubInterface() {
        char *pathvar = getenv("KTD_ROOT");
        if (strlen(pathvar) == 0) {
            std::cout << "please set env KTD_ROOT"<< std::endl;
        }
        std::string so_file_path = std::string(pathvar) + "/lib/idpware/mdc610plugin/libap2gsdspubplugin.so";

        mdc_sysinfo_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        mdc_sysinfo_ptr->open(so_file_path, RTLD_LAZY);
	}
	~CUsrAp2GsdsPubInterface() = default;



public:
	int publish(const std::string &topic, const std::string &type, void *msg)
	{
		avos::idpware::SAp2GsdsPublishParam publish_param;
		publish_param.topic = topic;
		publish_param.type = type;
		publish_param.msg = msg;
		return mdc_sysinfo_ptr->ioctl(avos::idpware::IOCTL_AP2GSDSPUB_PUBLISH,&publish_param);
	}

private:
	std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_sysinfo_ptr;


};

#define AP2GSDS_PUBLISH(topic, type, msg) \
    CUsrAp2GsdsPubInterface::Instance()->publish((topic), (type), static_cast<void*>(msg))


#endif

