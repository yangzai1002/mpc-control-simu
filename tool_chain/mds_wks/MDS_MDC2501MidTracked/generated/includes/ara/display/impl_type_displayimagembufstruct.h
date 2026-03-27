/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_DISPLAY_IMPL_TYPE_DISPLAYIMAGEMBUFSTRUCT_H
#define ARA_DISPLAY_IMPL_TYPE_DISPLAYIMAGEMBUFSTRUCT_H

#include "impl_type_uint32_t.h"
#include "impl_type_uint64_t.h"
#include "impl_type_rawbuffer.h"
#include "impl_type_uint8_t.h"
#include "ara/display/impl_type_imageformattypee.h"
#include "ara/display/impl_type_rotatemodee.h"

namespace ara {
namespace display {
struct DisplayImageMbufStruct {
    ::uint32_t Height{ 0U };
    ::uint32_t Width{ 0U };
    ::uint64_t SendTimeStamp{ 0U };
    ::uint32_t DataSize{ 0U };
    ::rawBuffer *RawData{ nullptr };
    ::uint32_t Seq{ 0U };
    ::uint8_t ChannelId{ 0U };
    ::ara::display::ImageFormatTypeE InputFormat{ ::ara::display::ImageFormatTypeE::IMAGE_FORMAT_RGB888 };
    ::ara::display::ImageFormatTypeE OutputFormat{ ::ara::display::ImageFormatTypeE::IMAGE_FORMAT_RGB888 };
    ::ara::display::RotateModeE RotateMode{ ::ara::display::RotateModeE::DO_NOT_ROTATE };

    static bool IsPlane()
    {
        return true;
    }

    ::rawBuffer* GetMbufPtr() const
    {
        return RawData;
    }

    void SetMbufPtr(::rawBuffer *p)
    {
        RawData = p;
    }

    using IsDpRawDataTag = void;
    using IsEnumerableTag = void;
    template<typename F>
    void enumerate(F& fun)
    {
        fun(Height);
        fun(Width);
        fun(SendTimeStamp);
        fun(DataSize);
        fun(Seq);
        fun(ChannelId);
        fun(InputFormat);
        fun(OutputFormat);
        fun(RotateMode);
    }

    template<typename F>
    void enumerate(F& fun) const
    {
        fun(Height);
        fun(Width);
        fun(SendTimeStamp);
        fun(DataSize);
        fun(Seq);
        fun(ChannelId);
        fun(InputFormat);
        fun(OutputFormat);
        fun(RotateMode);
    }

    template<typename F>
    void enumerate_internal(F& fun)
    {
        fun("Height", Height);
        fun("Width", Width);
        fun("SendTimeStamp", SendTimeStamp);
        fun("DataSize", DataSize);
        fun("Seq", Seq);
        fun("ChannelId", ChannelId);
        fun("InputFormat", InputFormat);
        fun("OutputFormat", OutputFormat);
        fun("RotateMode", RotateMode);
    }

    template<typename F>
    void enumerate_internal(F& fun) const
    {
        fun("Height", Height);
        fun("Width", Width);
        fun("SendTimeStamp", SendTimeStamp);
        fun("DataSize", DataSize);
        fun("Seq", Seq);
        fun("ChannelId", ChannelId);
        fun("InputFormat", InputFormat);
        fun("OutputFormat", OutputFormat);
        fun("RotateMode", RotateMode);
    }

    friend bool operator==(const ::ara::display::DisplayImageMbufStruct& lhs, const ::ara::display::DisplayImageMbufStruct& rhs) noexcept
    {
        return (lhs.Height == rhs.Height) && (lhs.Width == rhs.Width) && (lhs.SendTimeStamp == rhs.SendTimeStamp) && (lhs.DataSize == rhs.DataSize) && (lhs.RawData == rhs.RawData) && (lhs.Seq == rhs.Seq) && (lhs.ChannelId == rhs.ChannelId) && (lhs.InputFormat == rhs.InputFormat) && (lhs.OutputFormat == rhs.OutputFormat) && (lhs.RotateMode == rhs.RotateMode);
    }
};
} // namespace display
} // namespace ara


#endif // ARA_DISPLAY_IMPL_TYPE_DISPLAYIMAGEMBUFSTRUCT_H
