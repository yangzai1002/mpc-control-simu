/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACE_SKELETON_H
#define ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "ara/adsfi/sensorradarpacketserviceinterface_common.h"
#include <cstdint>

namespace ara {
namespace adsfi {
namespace skeleton {
namespace events
{
    using EventSensorRadarPacket = ara::com::internal::skeleton::event::EventAdapter<::ara::adsfi::SensorRadarPacket>;
    static constexpr ara::com::internal::EntityId SensorRadarPacketServiceInterfaceEventSensorRadarPacketId = 4226U; //EventSensorRadarPacket_event_hash
}

namespace methods
{
}

namespace fields
{
}

class SensorRadarPacketServiceInterfaceSkeleton {
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
        const ara::core::Result<void> resultEventSensorRadarPacket = EventSensorRadarPacket.Initialize();
        ThrowError(resultEventSensorRadarPacket);
    }

    SensorRadarPacketServiceInterfaceSkeleton& operator=(const SensorRadarPacketServiceInterfaceSkeleton&) = delete;

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
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceId, mode)),
              EventSensorRadarPacket(ptr->GetSkeleton(), events::SensorRadarPacketServiceInterfaceEventSensorRadarPacketId, ::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceId){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceSpec, mode)),
              EventSensorRadarPacket(ptr->GetSkeleton(), events::SensorRadarPacketServiceInterfaceEventSensorRadarPacketId, ::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceSpec){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceContainer, mode)),
              EventSensorRadarPacket(ptr->GetSkeleton(), events::SensorRadarPacketServiceInterfaceEventSensorRadarPacketId, ::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceContainer){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              EventSensorRadarPacket(std::move(other.EventSensorRadarPacket)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                EventSensorRadarPacket = std::move(other.EventSensorRadarPacket);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        events::EventSensorRadarPacket GetEventSensorRadarPacket()
        {
            return std::move(EventSensorRadarPacket);
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
                initResult = EventSensorRadarPacket.Initialize();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        events::EventSensorRadarPacket EventSensorRadarPacket;
    };
    explicit SensorRadarPacketServiceInterfaceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceId, mode)),
          EventSensorRadarPacket(skeletonAdapter->GetSkeleton(), events::SensorRadarPacketServiceInterfaceEventSensorRadarPacketId, ::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceId){
        ConstructSkeleton(mode);
    }

    explicit SensorRadarPacketServiceInterfaceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceSpec, mode)),
          EventSensorRadarPacket(skeletonAdapter->GetSkeleton(), events::SensorRadarPacketServiceInterfaceEventSensorRadarPacketId, ::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceSpec){
        ConstructSkeleton(mode);
    }

    explicit SensorRadarPacketServiceInterfaceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceContainer, mode)),
          EventSensorRadarPacket(skeletonAdapter->GetSkeleton(), events::SensorRadarPacketServiceInterfaceEventSensorRadarPacketId, ::ara::adsfi::SensorRadarPacketServiceInterface::ServiceIdentifier, instanceContainer){
        ConstructSkeleton(mode);
    }

    SensorRadarPacketServiceInterfaceSkeleton(const SensorRadarPacketServiceInterfaceSkeleton&) = delete;

    SensorRadarPacketServiceInterfaceSkeleton(SensorRadarPacketServiceInterfaceSkeleton&&) = default;
    SensorRadarPacketServiceInterfaceSkeleton& operator=(SensorRadarPacketServiceInterfaceSkeleton&&) = default;
    SensorRadarPacketServiceInterfaceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          EventSensorRadarPacket(token.GetEventSensorRadarPacket()){
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

    virtual ~SensorRadarPacketServiceInterfaceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&SensorRadarPacketServiceInterfaceSkeleton::E2EErrorHandler, *this);
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

    events::EventSensorRadarPacket EventSensorRadarPacket;
};
} // namespace skeleton
} // namespace adsfi
} // namespace ara

#endif // ARA_ADSFI_SENSORRADARPACKETSERVICEINTERFACE_SKELETON_H
