#pragma once

#include "stddef.h"
#include "cidpabstractplatform.h"
#include "cidpabstractplatforminterface.h"
#include "imgprocessplatform2usrprotocol.h"


namespace avos
{
namespace idpware
{
class CUserImgProcessInterface {
public:
    static std::shared_ptr<CUserImgProcessInterface> Instance() {
        static std::shared_ptr<CUserImgProcessInterface> instance = nullptr;
        if (!instance) {
            static std::once_flag flag;
            std::call_once(
                        flag, [&] { instance.reset(new (std::nothrow) CUserImgProcessInterface()); });
        }
        return instance;
    }
    CUserImgProcessInterface()
    {
        char *pathvar = getenv("KTD_ROOT");
        if (strlen(pathvar) == 0) {
            std::cout << "please set env KTD_ROOT"<< std::endl;
        }
        std::string so_file_path = std::string(pathvar) + "/lib/idpware/mdc610plugin/libmdc610_imgprocess_plugin.so";
        std::cout<<"CUserImgProcessInterface so_file_path "<<so_file_path<<"\n";

        mdc_imgproc_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        mdc_imgproc_ptr->open(so_file_path, RTLD_LAZY);
    }

    void idpInitUserParam(avos::idpware::SImgProcess& img_usr_param,int width,int height,avos::idpware::EIdpImgBufType buftype)
    {
        img_usr_param.m_out_img.m_dest_pic_info.m_buf_imgtype = avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420;
        img_usr_param.m_out_img.m_dest_pic_info.m_buf_len = height*width*3/2;
        img_usr_param.m_out_img.m_dest_pic_info.m_buf_memtype = buftype;
        img_usr_param.m_out_img.m_dest_pic_info.m_img_height = height;
        img_usr_param.m_out_img.m_dest_pic_info.m_img_width = width;

        img_usr_param.m_out_img.crop_region.top_offset = 0;
        img_usr_param.m_out_img.crop_region.left_offset = 0;
        img_usr_param.m_out_img.crop_region.crop_height = height;
        img_usr_param.m_out_img.crop_region.crop_width = width;

        img_usr_param.m_out_img.resize_info.resize_height = height;
        img_usr_param.m_out_img.resize_info.resize_width = width;
        img_usr_param.m_out_img.dest_left_offset = 0;
        img_usr_param.m_out_img.dest_top_offset = 0;
    }

    int idpDvppMalloc(avos::idpware::SImgProcessInfo& meminfo)
    {
        return mdc_imgproc_ptr->ioctl(avos::idpware::IOCTL_DVPP_MALLOC, &meminfo);
    }

    int idpDvppFree(avos::idpware::SImgProcessInfo& meminfo)
    {
        return mdc_imgproc_ptr->ioctl(avos::idpware::IOCTL_DVPP_FREE, &meminfo);
    }

    int idpImgProcess(avos::idpware::SImgProcess& img_usr_param)
    {
        return mdc_imgproc_ptr->ioctl(avos::idpware::IOCTL_IMGCROP_RESIZE_PASTE_FUNC, &img_usr_param);
    }
private:
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_imgproc_ptr;

};
} // namespace idpware
} // namespace avos

