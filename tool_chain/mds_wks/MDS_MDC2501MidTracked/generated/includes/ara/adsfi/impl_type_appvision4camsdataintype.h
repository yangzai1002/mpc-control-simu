/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_APPVISION4CAMSDATAINTYPE_H
#define ARA_ADSFI_IMPL_TYPE_APPVISION4CAMSDATAINTYPE_H

#include "ara/adsfi/impl_type_appimage.h"

namespace ara {
namespace adsfi {
struct AppVision4CamsDataInType {
    bool image_front_h30_valid{ false };
    ::ara::adsfi::AppImage image_front_h30;
    bool image_front_h60_valid{ false };
    ::ara::adsfi::AppImage image_front_h60;
    bool image_left_h90_valid{ false };
    ::ara::adsfi::AppImage image_left_h90;
    bool image_right_h90_valid{ false };
    ::ara::adsfi::AppImage image_right_h90;
    bool image_pano_left_valid{ false };
    ::ara::adsfi::AppImage image_pano_left;
    bool image_pano_right_valid{ false };
    ::ara::adsfi::AppImage image_pano_right;
    bool image_pano_back_valid{ false };
    ::ara::adsfi::AppImage image_pano_back;

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(image_front_h30_valid);
        fun(image_front_h30);
        fun(image_front_h60_valid);
        fun(image_front_h60);
        fun(image_left_h90_valid);
        fun(image_left_h90);
        fun(image_right_h90_valid);
        fun(image_right_h90);
        fun(image_pano_left_valid);
        fun(image_pano_left);
        fun(image_pano_right_valid);
        fun(image_pano_right);
        fun(image_pano_back_valid);
        fun(image_pano_back);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(image_front_h30_valid);
        fun(image_front_h30);
        fun(image_front_h60_valid);
        fun(image_front_h60);
        fun(image_left_h90_valid);
        fun(image_left_h90);
        fun(image_right_h90_valid);
        fun(image_right_h90);
        fun(image_pano_left_valid);
        fun(image_pano_left);
        fun(image_pano_right_valid);
        fun(image_pano_right);
        fun(image_pano_back_valid);
        fun(image_pano_back);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("image_front_h30_valid", image_front_h30_valid);
        fun("image_front_h30", image_front_h30);
        fun("image_front_h60_valid", image_front_h60_valid);
        fun("image_front_h60", image_front_h60);
        fun("image_left_h90_valid", image_left_h90_valid);
        fun("image_left_h90", image_left_h90);
        fun("image_right_h90_valid", image_right_h90_valid);
        fun("image_right_h90", image_right_h90);
        fun("image_pano_left_valid", image_pano_left_valid);
        fun("image_pano_left", image_pano_left);
        fun("image_pano_right_valid", image_pano_right_valid);
        fun("image_pano_right", image_pano_right);
        fun("image_pano_back_valid", image_pano_back_valid);
        fun("image_pano_back", image_pano_back);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("image_front_h30_valid", image_front_h30_valid);
        fun("image_front_h30", image_front_h30);
        fun("image_front_h60_valid", image_front_h60_valid);
        fun("image_front_h60", image_front_h60);
        fun("image_left_h90_valid", image_left_h90_valid);
        fun("image_left_h90", image_left_h90);
        fun("image_right_h90_valid", image_right_h90_valid);
        fun("image_right_h90", image_right_h90);
        fun("image_pano_left_valid", image_pano_left_valid);
        fun("image_pano_left", image_pano_left);
        fun("image_pano_right_valid", image_pano_right_valid);
        fun("image_pano_right", image_pano_right);
        fun("image_pano_back_valid", image_pano_back_valid);
        fun("image_pano_back", image_pano_back);
    }

    friend bool operator==(const ::ara::adsfi::AppVision4CamsDataInType& lhs, const ::ara::adsfi::AppVision4CamsDataInType& rhs) noexcept
    {
        return (lhs.image_front_h30_valid == rhs.image_front_h30_valid) && (lhs.image_front_h30 == rhs.image_front_h30) && (lhs.image_front_h60_valid == rhs.image_front_h60_valid) && (lhs.image_front_h60 == rhs.image_front_h60) && (lhs.image_left_h90_valid == rhs.image_left_h90_valid) && (lhs.image_left_h90 == rhs.image_left_h90) && (lhs.image_right_h90_valid == rhs.image_right_h90_valid) && (lhs.image_right_h90 == rhs.image_right_h90) && (lhs.image_pano_left_valid == rhs.image_pano_left_valid) && (lhs.image_pano_left == rhs.image_pano_left) && (lhs.image_pano_right_valid == rhs.image_pano_right_valid) && (lhs.image_pano_right == rhs.image_pano_right) && (lhs.image_pano_back_valid == rhs.image_pano_back_valid) && (lhs.image_pano_back == rhs.image_pano_back);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_APPVISION4CAMSDATAINTYPE_H
