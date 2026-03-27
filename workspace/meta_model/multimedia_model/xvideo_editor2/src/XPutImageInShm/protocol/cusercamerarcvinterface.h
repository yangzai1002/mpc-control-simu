#pragma once

#include "stddef.h"
#include "./cameraplatform2usrprotocol.h"
#include "./cidpabstractplatform.h"
#include "./cidpabstractplatforminterface.h"

namespace avos
{
namespace idpware
{
class CUserCamRecvInterface {
public:
    static std::shared_ptr<CUserCamRecvInterface> Instance() {
        static std::shared_ptr<CUserCamRecvInterface> instance = nullptr;
        if (!instance) {
            static std::once_flag flag;
            std::call_once(
                        flag, [&] { instance.reset(new (std::nothrow) CUserCamRecvInterface()); });
        }
        return instance;
    }
    CUserCamRecvInterface()
    {
        char *pathvar = getenv("KTD_ROOT");
        if (strlen(pathvar) == 0) {
            std::cout << "please set env KTD_ROOT"<< std::endl;
        }
        std::string so_file_path = std::string(pathvar) + "/lib/idpware/mdc610plugin/libcamera_recv_v20_3.so";

        mdc_camrecv_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        mdc_camrecv_ptr->open(so_file_path, RTLD_LAZY);
    }


    int idpCamClientInit(void* param)
    {
        return mdc_camrecv_ptr->ioctl(avos::idpware::EIdpCameraPlatformIOType::IOCTL_INIT_CLIENT_CAMERA, param);
    }

    int idpGetCamData(avos::idpware::SCameraClientData& img_usr_param)
    {
        return mdc_camrecv_ptr->ioctl(avos::idpware::EIdpCameraPlatformIOType::IOCTL_GET_CLIENT_CAMERADATA, &img_usr_param);
    }
private:
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_camrecv_ptr;

};
} // namespace idpware
} // namespace avos

