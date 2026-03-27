#pragma once

#include "cusercamerarcvinterface.h"
#include "cuserimgprocessinterface.h"
#include "cusersysinfointerface.h"

namespace avos
{
namespace idpware
{

class CUserMdc610FuncInterface {
public:
    static std::shared_ptr<CUserMdc610FuncInterface> Instance() {
        static std::shared_ptr<CUserMdc610FuncInterface> instance = nullptr;
        if (!instance) {
            static std::once_flag flag;
            std::call_once(
                        flag, [&] { instance.reset(new (std::nothrow) CUserMdc610FuncInterface()); });
        }
        return instance;
    }
    CUserMdc610FuncInterface()
    {
        mdc_sysinfo_interface = systeminfo::CUserSysinfoInterface::Instance();
        mdc_camclient_interface = CUserCamRecvInterface::Instance();
        mdc_imgprocess_interface = CUserImgProcessInterface::Instance();
    }

    //sysinfo
    void idpGetDataPlaneClock(systeminfo::SSysTimeInfoData &time)
    {
        mdc_sysinfo_interface->getDataPlaneClock(time);
    }

    void idpGetManagePlaneClock(systeminfo::SSysTimeInfoData &time)
    {
        mdc_sysinfo_interface->getManagePlaneClock(time);
    }

    std::string idpGetBoardTemperature()
    {
        return idpGetBoardStrinfo(systeminfo::BOARD_TEMPERATURE_TYPE);
    }

    std::string idpGetBoardFanduty()
    {
        return idpGetBoardStrinfo(systeminfo::BOARD_FANDUTY_TYPE);
    }

    std::string idpGetBoardVoltage()
    {
        return idpGetBoardStrinfo(systeminfo::BOARD_VOLTAGE_TYPE);
    }


    //imgprocess
    void idpInitUserParam(avos::idpware::SImgProcess& img_usr_param,int width,int height,avos::idpware::EIdpImgBufType buftype)
    {
        mdc_imgprocess_interface->idpInitUserParam(img_usr_param,width,height,buftype);
    }

    int idpDvppMalloc(avos::idpware::SImgProcessInfo& meminfo)
    {
        return mdc_imgprocess_interface->idpDvppMalloc(meminfo);
    }

    int idpDvppFree(avos::idpware::SImgProcessInfo& meminfo)
    {
        return mdc_imgprocess_interface->idpDvppFree(meminfo);
    }

    int idpImgProcess(avos::idpware::SImgProcess& img_usr_param)
    {
        return mdc_imgprocess_interface->idpImgProcess(img_usr_param);
    }

    //cameraclient
    int idpCamClientInit(avos::idpware::SCameraAllName& camera_name_list)
    {
        return mdc_camclient_interface->idpCamClientInit(&camera_name_list);
    }
    int idpCamClientInit()
    {
        return mdc_camclient_interface->idpCamClientInit(NULL);
    }

    int idpGetCamData(avos::idpware::SCameraClientData& img_usr_param)
    {
        return mdc_camclient_interface->idpGetCamData(img_usr_param);
    }
private:
    std::string idpGetBoardStrinfo(systeminfo::EIdpBoardStrInfoType type)
    {
        return mdc_sysinfo_interface->getBoardStrinfo(type);
    }
private:
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_mdc610func_ptr;
    std::shared_ptr<systeminfo::CUserSysinfoInterface> mdc_sysinfo_interface;
    std::shared_ptr<CUserCamRecvInterface> mdc_camclient_interface;
    std::shared_ptr<CUserImgProcessInterface> mdc_imgprocess_interface;
public:

};

} // namespace idpware
} // namespace avos

