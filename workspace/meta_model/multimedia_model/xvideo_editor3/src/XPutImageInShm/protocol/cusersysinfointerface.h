#pragma once

#include "stddef.h"
#include "sysplatform2usrprotocol.h"
#include "./cidpabstractplatform.h"
#include "./cidpabstractplatforminterface.h"

namespace avos
{
namespace idpware
{
namespace systeminfo
{
class CUserSysinfoInterface {
public:
    static std::shared_ptr<CUserSysinfoInterface> Instance() {
        static std::shared_ptr<CUserSysinfoInterface> instance = nullptr;
        if (!instance) {
            static std::once_flag flag;
            std::call_once(
                        flag, [&] { instance.reset(new (std::nothrow) CUserSysinfoInterface()); });
        }
        return instance;
    }
    CUserSysinfoInterface()
    {
        char *pathvar = getenv("KTD_ROOT");
        if (strlen(pathvar) == 0) {
            std::cout << "please set env KTD_ROOT"<< std::endl;
        }
        std::string so_file_path = std::string(pathvar) + "/lib/idpware/mdc610plugin/libsystem_plugin.so";

        mdc_sysinfo_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        mdc_sysinfo_ptr->open(so_file_path, RTLD_LAZY);
    }

    void getDataPlaneClock(SSysTimeInfoData &time)
    {
        mdc_sysinfo_ptr->ioctl(IOCTL_GET_DPCN,&time);
    }

    void getManagePlaneClock(SSysTimeInfoData &time)
    {
        mdc_sysinfo_ptr->ioctl(IOCTL_GET_MPCN,&time);
    }

    std::string getBoardStrinfo(EIdpBoardStrInfoType type)
    {
        SSysBoardStrinfoData strsysinfo;
        strsysinfo.board_str_type = type;

        mdc_sysinfo_ptr->ioctl(IOCTL_GET_BOARDINFO,&strsysinfo);
        std::cout << "strsysinfo.info_len = " << strsysinfo.info_len <<std::endl;
        return std::string(strsysinfo.info,strsysinfo.info_len);
    }


    std::string getBoardTemperature()
    {
        return getBoardStrinfo(BOARD_TEMPERATURE_TYPE);
    }

    std::string getBoardFanduty()
    {
        return getBoardStrinfo(BOARD_FANDUTY_TYPE);
    }

    std::string getBoardVoltage()
    {
        return getBoardStrinfo(BOARD_VOLTAGE_TYPE);
    }
private:
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_sysinfo_ptr;

public:

};
}
} // namespace idpware
} // namespace avos

