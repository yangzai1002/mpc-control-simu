/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_SENSORIMAGESERVICEINTERFACE_SKELETON_H
#define ARA_ADSFI_SENSORIMAGESERVICEINTERFACE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "ara/adsfi/sensorimageserviceinterface_common.h"
#include <cstdint>

namespace ara {
namespace adsfi {
namespace skeleton {
namespace events
{
    using EventSensorImage = ara::com::internal::skeleton::event::EventAdapter<::ara::adsfi::MsgImageData>;
    static constexpr ara::com::internal::EntityId SensorImageServiceInterfaceEventSensorImageId = 12444U; //EventSensorImage_event_hash
}

namespace methods
{
}

namespace fields
{
}

class SensorImageServiceInterfaceSkeleton {
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
        const ara::core::Result<void> resultEventSensorImage = EventSensorImage.Initialize();
        ThrowError(resultEventSensorImage);
    }

    SensorImageServiceInterfaceSkeleton& operator=(const SensorImageServiceInterfaceSkeleton&) = delete;

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
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceId, mode)),
              EventSensorImage(ptr->GetSkeleton(), events::SensorImageServiceInterfaceEventSensorImageId, ::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceId){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceSpec, mode)),
              EventSensorImage(ptr->GetSkeleton(), events::SensorImageServiceInterfaceEventSensorImageId, ::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceSpec){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceContainer, mode)),
              EventSensorImage(ptr->GetSkeleton(), events::SensorImageServiceInterfaceEventSensorImageId, ::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceContainer){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              EventSensorImage(std::move(other.EventSensorImage)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                EventSensorImage = std::move(other.EventSensorImage);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        events::EventSensorImage GetEventSensorImage()
        {
            return std::move(EventSensorImage);
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
                initResult = EventSensorImage.Initialize();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        events::EventSensorImage EventSensorImage;
    };
    explicit SensorImageServiceInterfaceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceId, mode)),
          EventSensorImage(skeletonAdapter->GetSkeleton(), events::SensorImageServiceInterfaceEventSensorImageId, ::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceId){
        ConstructSkeleton(mode);
    }

    explicit SensorImageServiceInterfaceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceSpec, mode)),
          EventSensorImage(skeletonAdapter->GetSkeleton(), events::SensorImageServiceInterfaceEventSensorImageId, ::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceSpec){
        ConstructSkeleton(mode);
    }

    explicit SensorImageServiceInterfaceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceContainer, mode)),
          EventSensorImage(skeletonAdapter->GetSkeleton(), events::SensorImageServiceInterfaceEventSensorImageId, ::ara::adsfi::SensorImageServiceInterface::ServiceIdentifier, instanceContainer){
        ConstructSkeleton(mode);
    }

    SensorImageServiceInterfaceSkeleton(const SensorImageServiceInterfaceSkeleton&) = delete;

    SensorImageServiceInterfaceSkeleton(SensorImageServiceInterfaceSkeleton&&) = default;
    SensorImageServiceInterfaceSkeleton& operator=(SensorImageServiceInterfaceSkeleton&&) = default;
    SensorImageServiceInterfaceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          EventSensorImage(token.GetEventSensorImage()){
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

    virtual ~SensorImageServiceInterfaceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&SensorImageServiceInterfaceSkeleton::E2EErrorHandler, *this);
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

    events::EventSensorImage EventSensorImage;
};
} // namespace skeleton
} // namespace adsfi
} // namespace ara

#endif // ARA_ADSFI_SENSORIMAGESERVICEINTERFACE_SKELETON_H
