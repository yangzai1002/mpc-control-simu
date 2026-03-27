/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ADSFI_TRAFFICSIGNALINTF_SKELETON_H
#define ADSFI_TRAFFICSIGNALINTF_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "adsfi/trafficsignalintf_common.h"
#include <cstdint>

namespace adsfi {
namespace skeleton {
namespace events
{
    using TrafficSignalArray = ara::com::internal::skeleton::event::EventAdapter<::adsfi::TrafficSignalArray>;
    static constexpr ara::com::internal::EntityId TrafficSignalIntfTrafficSignalArrayId = 9692U; //TrafficSignalArray_event_hash
}

namespace methods
{
}

namespace fields
{
}

class TrafficSignalIntfSkeleton {
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
        const ara::core::Result<void> resultTrafficSignalArray = TrafficSignalArray.Initialize();
        ThrowError(resultTrafficSignalArray);
    }

    TrafficSignalIntfSkeleton& operator=(const TrafficSignalIntfSkeleton&) = delete;

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
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceId, mode)),
              TrafficSignalArray(ptr->GetSkeleton(), events::TrafficSignalIntfTrafficSignalArrayId, ::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceId){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceSpec, mode)),
              TrafficSignalArray(ptr->GetSkeleton(), events::TrafficSignalIntfTrafficSignalArrayId, ::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceSpec){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceContainer, mode)),
              TrafficSignalArray(ptr->GetSkeleton(), events::TrafficSignalIntfTrafficSignalArrayId, ::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceContainer){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              TrafficSignalArray(std::move(other.TrafficSignalArray)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                TrafficSignalArray = std::move(other.TrafficSignalArray);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        events::TrafficSignalArray GetTrafficSignalArray()
        {
            return std::move(TrafficSignalArray);
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
                initResult = TrafficSignalArray.Initialize();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        events::TrafficSignalArray TrafficSignalArray;
    };
    explicit TrafficSignalIntfSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceId, mode)),
          TrafficSignalArray(skeletonAdapter->GetSkeleton(), events::TrafficSignalIntfTrafficSignalArrayId, ::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceId){
        ConstructSkeleton(mode);
    }

    explicit TrafficSignalIntfSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceSpec, mode)),
          TrafficSignalArray(skeletonAdapter->GetSkeleton(), events::TrafficSignalIntfTrafficSignalArrayId, ::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceSpec){
        ConstructSkeleton(mode);
    }

    explicit TrafficSignalIntfSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceContainer, mode)),
          TrafficSignalArray(skeletonAdapter->GetSkeleton(), events::TrafficSignalIntfTrafficSignalArrayId, ::adsfi::TrafficSignalIntf::ServiceIdentifier, instanceContainer){
        ConstructSkeleton(mode);
    }

    TrafficSignalIntfSkeleton(const TrafficSignalIntfSkeleton&) = delete;

    TrafficSignalIntfSkeleton(TrafficSignalIntfSkeleton&&) = default;
    TrafficSignalIntfSkeleton& operator=(TrafficSignalIntfSkeleton&&) = default;
    TrafficSignalIntfSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          TrafficSignalArray(token.GetTrafficSignalArray()){
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

    virtual ~TrafficSignalIntfSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&TrafficSignalIntfSkeleton::E2EErrorHandler, *this);
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

    events::TrafficSignalArray TrafficSignalArray;
};
} // namespace skeleton
} // namespace adsfi

#endif // ADSFI_TRAFFICSIGNALINTF_SKELETON_H
