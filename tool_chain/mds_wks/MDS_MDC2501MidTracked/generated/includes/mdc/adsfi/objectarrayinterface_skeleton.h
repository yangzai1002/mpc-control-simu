/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_ADSFI_OBJECTARRAYINTERFACE_SKELETON_H
#define MDC_ADSFI_OBJECTARRAYINTERFACE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/adsfi/objectarrayinterface_common.h"
#include <cstdint>

namespace mdc {
namespace adsfi {
namespace skeleton {
namespace events
{
    using ObjectArrayEvent = ara::com::internal::skeleton::event::EventAdapter<::ara::adsfi::ObjectArray>;
    static constexpr ara::com::internal::EntityId ObjectArrayInterfaceObjectArrayEventId = 26552U; //ObjectArrayEvent_event_hash
}

namespace methods
{
}

namespace fields
{
}

class ObjectArrayInterfaceSkeleton {
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
        const ara::core::Result<void> resultObjectArrayEvent = ObjectArrayEvent.Initialize();
        ThrowError(resultObjectArrayEvent);
    }

    ObjectArrayInterfaceSkeleton& operator=(const ObjectArrayInterfaceSkeleton&) = delete;

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
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceId, mode)),
              ObjectArrayEvent(ptr->GetSkeleton(), events::ObjectArrayInterfaceObjectArrayEventId, ::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceId){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceSpec, mode)),
              ObjectArrayEvent(ptr->GetSkeleton(), events::ObjectArrayInterfaceObjectArrayEventId, ::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceSpec){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceContainer, mode)),
              ObjectArrayEvent(ptr->GetSkeleton(), events::ObjectArrayInterfaceObjectArrayEventId, ::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceContainer){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              ObjectArrayEvent(std::move(other.ObjectArrayEvent)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                ObjectArrayEvent = std::move(other.ObjectArrayEvent);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        events::ObjectArrayEvent GetObjectArrayEvent()
        {
            return std::move(ObjectArrayEvent);
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
                initResult = ObjectArrayEvent.Initialize();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        events::ObjectArrayEvent ObjectArrayEvent;
    };
    explicit ObjectArrayInterfaceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceId, mode)),
          ObjectArrayEvent(skeletonAdapter->GetSkeleton(), events::ObjectArrayInterfaceObjectArrayEventId, ::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceId){
        ConstructSkeleton(mode);
    }

    explicit ObjectArrayInterfaceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceSpec, mode)),
          ObjectArrayEvent(skeletonAdapter->GetSkeleton(), events::ObjectArrayInterfaceObjectArrayEventId, ::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceSpec){
        ConstructSkeleton(mode);
    }

    explicit ObjectArrayInterfaceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceContainer, mode)),
          ObjectArrayEvent(skeletonAdapter->GetSkeleton(), events::ObjectArrayInterfaceObjectArrayEventId, ::mdc::adsfi::ObjectArrayInterface::ServiceIdentifier, instanceContainer){
        ConstructSkeleton(mode);
    }

    ObjectArrayInterfaceSkeleton(const ObjectArrayInterfaceSkeleton&) = delete;

    ObjectArrayInterfaceSkeleton(ObjectArrayInterfaceSkeleton&&) = default;
    ObjectArrayInterfaceSkeleton& operator=(ObjectArrayInterfaceSkeleton&&) = default;
    ObjectArrayInterfaceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          ObjectArrayEvent(token.GetObjectArrayEvent()){
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

    virtual ~ObjectArrayInterfaceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&ObjectArrayInterfaceSkeleton::E2EErrorHandler, *this);
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

    events::ObjectArrayEvent ObjectArrayEvent;
};
} // namespace skeleton
} // namespace adsfi
} // namespace mdc

#endif // MDC_ADSFI_OBJECTARRAYINTERFACE_SKELETON_H
