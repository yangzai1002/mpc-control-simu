/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_IMPL_TYPE_MSGIMAGEDATA_H
#define ARA_ADSFI_IMPL_TYPE_MSGIMAGEDATA_H

#include "ara/adsfi/impl_type_msghaftime.h"
#include "impl_type_string.h"
#include "impl_type_uint32_t.h"
#include "impl_type_uint8_t.h"
#include "ara/adsfi/impl_type_idpuint8vec.h"
#include "impl_type_int64_t.h"

namespace ara {
namespace adsfi {
struct MsgImageData {
    ::ara::adsfi::MsgHafTime timestamp;
    ::String frameID;
    ::uint32_t width{ 0U };
    ::uint32_t height{ 0U };
    ::uint32_t dataSize{ 0U };
    ::uint8_t bufferType{ 0U };
    ::uint8_t imageType{ 0U };
    ::uint32_t seq{ 0U };
    ::ara::adsfi::IdpUint8Vec rawData;
    ::int64_t mbufData{ 0 };

    static bool IsPlane()
    {
        return false;
    }


    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(timestamp);
        fun(frameID);
        fun(width);
        fun(height);
        fun(dataSize);
        fun(bufferType);
        fun(imageType);
        fun(seq);
        fun(rawData);
        fun(mbufData);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(timestamp);
        fun(frameID);
        fun(width);
        fun(height);
        fun(dataSize);
        fun(bufferType);
        fun(imageType);
        fun(seq);
        fun(rawData);
        fun(mbufData);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("width", width);
        fun("height", height);
        fun("dataSize", dataSize);
        fun("bufferType", bufferType);
        fun("imageType", imageType);
        fun("seq", seq);
        fun("rawData", rawData);
        fun("mbufData", mbufData);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("timestamp", timestamp);
        fun("frameID", frameID);
        fun("width", width);
        fun("height", height);
        fun("dataSize", dataSize);
        fun("bufferType", bufferType);
        fun("imageType", imageType);
        fun("seq", seq);
        fun("rawData", rawData);
        fun("mbufData", mbufData);
    }

    friend bool operator==(const ::ara::adsfi::MsgImageData& lhs, const ::ara::adsfi::MsgImageData& rhs) noexcept
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.frameID == rhs.frameID) && (lhs.width == rhs.width) && (lhs.height == rhs.height) && (lhs.dataSize == rhs.dataSize) && (lhs.bufferType == rhs.bufferType) && (lhs.imageType == rhs.imageType) && (lhs.seq == rhs.seq) && (lhs.rawData == rhs.rawData) && (lhs.mbufData == rhs.mbufData);
    }
};
} // namespace adsfi
} // namespace ara


#endif // ARA_ADSFI_IMPL_TYPE_MSGIMAGEDATA_H
