/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_EX_WDG_EXWDGLOWPOWERNOTIFYSERVICE_SKELETON_H
#define MDC_EX_WDG_EXWDGLOWPOWERNOTIFYSERVICE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/ex_wdg/exwdglowpowernotifyservice_common.h"
#include <cstdint>

namespace mdc {
namespace ex_wdg {
namespace skeleton {
namespace events
{
}

namespace methods
{
    using LowPowerNotifyHandle = ara::com::internal::skeleton::method::MethodAdapter;
    static constexpr ara::com::internal::EntityId ExwdgLowPowerNotifyServiceLowPowerNotifyId = 41126U; //LowPowerNotify_method_hash
}

namespace fields
{
}

class ExwdgLowPowerNotifyServiceSkeleton {
private:
    std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> skeletonAdapter;
    void ConstructSkeleton(const ara::com::MethodCallProcessingMode mode)
    {
        if (mode == ara::com::MethodCallProcessingMode::kEvent) {
            if (!(skeletonAdapter->SetMethodThreadNumber(skeletonAdapter->GetMethodThreadNumber(1U), 1024U))) {
#ifndef NOT_SUPPORT_EXCEPTIONS
                ara::core::ErrorCode errorcode(ara::com::ComErrc::kNetworkBindingFailure);
                throw ara::com::ComException(std::move(errorcode));
#else
                std::cerr << "Error: Not support exception, create skeleton failed!" << std::endl;
#endif
            }
        }
        const ara::core::Result<void> resultLowPowerNotify = LowPowerNotifyHandle.Initialize<ara::core::Future<LowPowerNotifyOutput>>();
        ThrowError(resultLowPowerNotify);
    }

    ExwdgLowPowerNotifyServiceSkeleton& operator=(const ExwdgLowPowerNotifyServiceSkeleton&) = delete;

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
    using LowPowerNotifyOutput = mdc::ex_wdg::methods::LowPowerNotify::Output;
    
    class ConstructionToken {
    public:
        explicit ConstructionToken(const ara::com::InstanceIdentifier& instanceId,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceId, mode)),
              LowPowerNotifyHandle(ptr->GetSkeleton(), methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId, ::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceId, mode){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceSpec, mode)),
              LowPowerNotifyHandle(ptr->GetSkeleton(), methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId, ::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceSpec, mode){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceContainer, mode)),
              LowPowerNotifyHandle(ptr->GetSkeleton(), methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId, ::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceContainer, mode){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              LowPowerNotifyHandle(std::move(other.LowPowerNotifyHandle)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                LowPowerNotifyHandle = std::move(other.LowPowerNotifyHandle);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        methods::LowPowerNotifyHandle GetLowPowerNotifyHandle()
        {
            return std::move(LowPowerNotifyHandle);
        }
        ara::core::Result<void> Initialize()
        {
            ara::core::Result<void> initResult;
            do {
                if (processMode == ara::com::MethodCallProcessingMode::kEvent) {
                    if(!ptr->SetMethodThreadNumber(ptr->GetMethodThreadNumber(1U), 1024U)) {
                        ara::core::ErrorCode errorCode(ara::com::ComErrc::kNetworkBindingFailure);
                        initResult.EmplaceError(errorCode);
                        break;
                    }
                }
                initResult = LowPowerNotifyHandle.Initialize<ara::core::Future<LowPowerNotifyOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        methods::LowPowerNotifyHandle LowPowerNotifyHandle;
    };
    explicit ExwdgLowPowerNotifyServiceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceId, mode)),
          LowPowerNotifyHandle(skeletonAdapter->GetSkeleton(), methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId, ::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceId, mode){
        ConstructSkeleton(mode);
    }

    explicit ExwdgLowPowerNotifyServiceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceSpec, mode)),
          LowPowerNotifyHandle(skeletonAdapter->GetSkeleton(), methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId, ::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceSpec, mode){
        ConstructSkeleton(mode);
    }

    explicit ExwdgLowPowerNotifyServiceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceContainer, mode)),
          LowPowerNotifyHandle(skeletonAdapter->GetSkeleton(), methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId, ::mdc::ex_wdg::ExwdgLowPowerNotifyService::ServiceIdentifier, instanceContainer, mode){
        ConstructSkeleton(mode);
    }

    ExwdgLowPowerNotifyServiceSkeleton(const ExwdgLowPowerNotifyServiceSkeleton&) = delete;

    ExwdgLowPowerNotifyServiceSkeleton(ExwdgLowPowerNotifyServiceSkeleton&&) = default;
    ExwdgLowPowerNotifyServiceSkeleton& operator=(ExwdgLowPowerNotifyServiceSkeleton&&) = default;
    ExwdgLowPowerNotifyServiceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          LowPowerNotifyHandle(token.GetLowPowerNotifyHandle()){
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

    virtual ~ExwdgLowPowerNotifyServiceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&ExwdgLowPowerNotifyServiceSkeleton::E2EErrorHandler, *this);
        skeletonAdapter->RegisterMethod(&ExwdgLowPowerNotifyServiceSkeleton::LowPowerNotify, *this, methods::ExwdgLowPowerNotifyServiceLowPowerNotifyId);
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

    virtual ara::core::Future<LowPowerNotifyOutput> LowPowerNotify(const ::uint32_t& powerState) = 0;
    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode, ara::com::e2e::DataID, ara::com::e2e::MessageCounter){}

    methods::LowPowerNotifyHandle LowPowerNotifyHandle;
};
} // namespace skeleton
} // namespace ex_wdg
} // namespace mdc

#endif // MDC_EX_WDG_EXWDGLOWPOWERNOTIFYSERVICE_SKELETON_H
