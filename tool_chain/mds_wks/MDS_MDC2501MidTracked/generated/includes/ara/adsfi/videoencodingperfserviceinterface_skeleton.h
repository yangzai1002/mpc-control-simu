/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_VIDEOENCODINGPERFSERVICEINTERFACE_SKELETON_H
#define ARA_ADSFI_VIDEOENCODINGPERFSERVICEINTERFACE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "ara/adsfi/videoencodingperfserviceinterface_common.h"
#include <cstdint>

namespace ara {
namespace adsfi {
namespace skeleton {
namespace events
{
    using EventVideoEncodingPerf = ara::com::internal::skeleton::event::EventAdapter<::ara::adsfi::VideoEncodingPerf>;
    static constexpr ara::com::internal::EntityId VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId = 41490U; //EventVideoEncodingPerf_event_hash
}

namespace methods
{
}

namespace fields
{
}

class VideoEncodingPerfServiceInterfaceSkeleton {
private:
    std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> skeletonAdapter;
    void ConstructSkeleton(const ara::com::MethodCallProcessingMode mode)
    {
        if (mode == ara::com::MethodCallProcessingMode::kEvent) {
            if (!(skeletonAdapter->SetMethodThreadNumber(skeletonAdapter->GetMethodThreadNumber(0U), 1024U))) {
#ifndef NOT_SUPPORT_EXCEPTIONS
                ara::core::ErrorCode errorcode(ara::com::ComErrc::kNetworkBindingFailure);
                throw ara::com::ComException(std::move(errorcode));
#else
                std::cerr << "Error: Not support exception, create skeleton failed!" << std::endl;
#endif
            }
        }
        const ara::core::Result<void> resultEventVideoEncodingPerf = EventVideoEncodingPerf.Initialize();
        ThrowError(resultEventVideoEncodingPerf);
    }

    VideoEncodingPerfServiceInterfaceSkeleton& operator=(const VideoEncodingPerfServiceInterfaceSkeleton&) = delete;

    static void ThrowError(const ara::core::Result<void>& result)
    {
        if (!(result.HasValue())) {
#ifndef NOT_SUPPORT_EXCEPTIONS
            ara::core::ErrorCode errorcode(result.Error());
            throw ara::com::ComException(std::move(errorcode));
#else
            std::cerr << "Error: Not support exception, create skeleton failed!" << std::endl;
#endif
        }
    }
public:
    class ConstructionToken {
    public:
        explicit ConstructionToken(const ara::com::InstanceIdentifier& instanceId,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceId, mode)),
              EventVideoEncodingPerf(ptr->GetSkeleton(), events::VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId, ::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceId){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceSpec, mode)),
              EventVideoEncodingPerf(ptr->GetSkeleton(), events::VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId, ::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceSpec){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceContainer, mode)),
              EventVideoEncodingPerf(ptr->GetSkeleton(), events::VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId, ::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceContainer){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              EventVideoEncodingPerf(std::move(other.EventVideoEncodingPerf)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                EventVideoEncodingPerf = std::move(other.EventVideoEncodingPerf);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        events::EventVideoEncodingPerf GetEventVideoEncodingPerf()
        {
            return std::move(EventVideoEncodingPerf);
        }
        ara::core::Result<void> Initialize()
        {
            ara::core::Result<void> initResult;
            do {
                if (processMode == ara::com::MethodCallProcessingMode::kEvent) {
                    if(!ptr->SetMethodThreadNumber(ptr->GetMethodThreadNumber(0U), 1024U)) {
                        ara::core::ErrorCode errorCode(ara::com::ComErrc::kNetworkBindingFailure);
                        initResult.EmplaceError(errorCode);
                        break;
                    }
                }
                initResult = EventVideoEncodingPerf.Initialize();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        events::EventVideoEncodingPerf EventVideoEncodingPerf;
    };
    explicit VideoEncodingPerfServiceInterfaceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceId, mode)),
          EventVideoEncodingPerf(skeletonAdapter->GetSkeleton(), events::VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId, ::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceId){
        ConstructSkeleton(mode);
    }

    explicit VideoEncodingPerfServiceInterfaceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceSpec, mode)),
          EventVideoEncodingPerf(skeletonAdapter->GetSkeleton(), events::VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId, ::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceSpec){
        ConstructSkeleton(mode);
    }

    explicit VideoEncodingPerfServiceInterfaceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceContainer, mode)),
          EventVideoEncodingPerf(skeletonAdapter->GetSkeleton(), events::VideoEncodingPerfServiceInterfaceEventVideoEncodingPerfId, ::ara::adsfi::VideoEncodingPerfServiceInterface::ServiceIdentifier, instanceContainer){
        ConstructSkeleton(mode);
    }

    VideoEncodingPerfServiceInterfaceSkeleton(const VideoEncodingPerfServiceInterfaceSkeleton&) = delete;

    VideoEncodingPerfServiceInterfaceSkeleton(VideoEncodingPerfServiceInterfaceSkeleton&&) = default;
    VideoEncodingPerfServiceInterfaceSkeleton& operator=(VideoEncodingPerfServiceInterfaceSkeleton&&) = default;
    VideoEncodingPerfServiceInterfaceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          EventVideoEncodingPerf(token.GetEventVideoEncodingPerf()){
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        ara::com::InstanceIdentifier instanceId,
        const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
    {
        ConstructionToken token(instanceId, mode);
        const auto initResult = token.Initialize();
        ara::core::Result<ConstructionToken> result(std::move(token));
        if (!initResult.HasValue()) {
            const ara::core::ErrorCode errorCode(initResult.Error());
            result.EmplaceError(errorCode);
        }
        return result;
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        ara::core::InstanceSpecifier instanceSpec,
        const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
    {
        ConstructionToken token(instanceSpec, mode);
        const auto initResult = token.Initialize();
        ara::core::Result<ConstructionToken> result(std::move(token));
        if (!initResult.HasValue()) {
            const ara::core::ErrorCode errorCode(initResult.Error());
            result.EmplaceError(errorCode);
        }
        return result;
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        ara::com::InstanceIdentifierContainer instanceIdContainer,
        const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
    {
        ConstructionToken token(instanceIdContainer, mode);
        const auto initResult = token.Initialize();
        ara::core::Result<ConstructionToken> result(std::move(token));
        if (!initResult.HasValue()) {
            const ara::core::ErrorCode errorCode(initResult.Error());
            result.EmplaceError(errorCode);
        }
        return result;
    }

    virtual ~VideoEncodingPerfServiceInterfaceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&VideoEncodingPerfServiceInterfaceSkeleton::E2EErrorHandler, *this);
        skeletonAdapter->OfferService();
    }
    void StopOfferService()
    {
        skeletonAdapter->StopOfferService();
    }
    ara::core::Future<bool> ProcessNextMethodCall()
    {
        return skeletonAdapter->ProcessNextMethodCall();
    }
    bool SetMethodThreadNumber(const std::uint16_t& number, const std::uint16_t& queueSize)
    {
        return skeletonAdapter->SetMethodThreadNumber(number, queueSize);
    }

    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode, ara::com::e2e::DataID, ara::com::e2e::MessageCounter){}

    events::EventVideoEncodingPerf EventVideoEncodingPerf;
};
} // namespace skeleton
} // namespace adsfi
} // namespace ara

#endif // ARA_ADSFI_VIDEOENCODINGPERFSERVICEINTERFACE_SKELETON_H
