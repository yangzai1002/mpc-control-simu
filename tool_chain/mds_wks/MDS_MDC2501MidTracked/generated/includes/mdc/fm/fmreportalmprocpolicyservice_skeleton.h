/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMREPORTALMPROCPOLICYSERVICE_SKELETON_H
#define MDC_FM_FMREPORTALMPROCPOLICYSERVICE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/fm/fmreportalmprocpolicyservice_common.h"
#include <cstdint>

namespace mdc {
namespace fm {
namespace skeleton {
namespace events
{
}

namespace methods
{
    using ReportAlmProcPolicyHandle = ara::com::internal::skeleton::method::MethodAdapter;
    using DisableOneAlarmHandle = ara::com::internal::skeleton::method::MethodAdapter;
    static constexpr ara::com::internal::EntityId FmReportAlmProcPolicyServiceReportAlmProcPolicyId = 6033U; //ReportAlmProcPolicy_method_hash
    static constexpr ara::com::internal::EntityId FmReportAlmProcPolicyServiceDisableOneAlarmId = 37733U; //DisableOneAlarm_method_hash
}

namespace fields
{
}

class FmReportAlmProcPolicyServiceSkeleton {
private:
    std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> skeletonAdapter;
    void ConstructSkeleton(const ara::com::MethodCallProcessingMode mode)
    {
        if (mode == ara::com::MethodCallProcessingMode::kEvent) {
            if (!(skeletonAdapter->SetMethodThreadNumber(skeletonAdapter->GetMethodThreadNumber(2U), 1024U))) {
#ifndef NOT_SUPPORT_EXCEPTIONS
                ara::core::ErrorCode errorcode(ara::com::ComErrc::kNetworkBindingFailure);
                throw ara::com::ComException(std::move(errorcode));
#else
                std::cerr << "Error: Not support exception, create skeleton failed!" << std::endl;
#endif
            }
        }
        const ara::core::Result<void> resultReportAlmProcPolicy = ReportAlmProcPolicyHandle.Initialize<ara::core::Future<ReportAlmProcPolicyOutput>>();
        ThrowError(resultReportAlmProcPolicy);
        const ara::core::Result<void> resultDisableOneAlarm = DisableOneAlarmHandle.Initialize<ara::core::Future<DisableOneAlarmOutput>>();
        ThrowError(resultDisableOneAlarm);
    }

    FmReportAlmProcPolicyServiceSkeleton& operator=(const FmReportAlmProcPolicyServiceSkeleton&) = delete;

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
    using ReportAlmProcPolicyOutput = mdc::fm::methods::ReportAlmProcPolicy::Output;
    
    using DisableOneAlarmOutput = mdc::fm::methods::DisableOneAlarm::Output;
    
    class ConstructionToken {
    public:
        explicit ConstructionToken(const ara::com::InstanceIdentifier& instanceId,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceId, mode)),
              ReportAlmProcPolicyHandle(ptr->GetSkeleton(), methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceId, mode),
              DisableOneAlarmHandle(ptr->GetSkeleton(), methods::FmReportAlmProcPolicyServiceDisableOneAlarmId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceId, mode){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceSpec, mode)),
              ReportAlmProcPolicyHandle(ptr->GetSkeleton(), methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceSpec, mode),
              DisableOneAlarmHandle(ptr->GetSkeleton(), methods::FmReportAlmProcPolicyServiceDisableOneAlarmId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceSpec, mode){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceContainer, mode)),
              ReportAlmProcPolicyHandle(ptr->GetSkeleton(), methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceContainer, mode),
              DisableOneAlarmHandle(ptr->GetSkeleton(), methods::FmReportAlmProcPolicyServiceDisableOneAlarmId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceContainer, mode){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              ReportAlmProcPolicyHandle(std::move(other.ReportAlmProcPolicyHandle)),
              DisableOneAlarmHandle(std::move(other.DisableOneAlarmHandle)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                ReportAlmProcPolicyHandle = std::move(other.ReportAlmProcPolicyHandle);
                DisableOneAlarmHandle = std::move(other.DisableOneAlarmHandle);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        methods::ReportAlmProcPolicyHandle GetReportAlmProcPolicyHandle()
        {
            return std::move(ReportAlmProcPolicyHandle);
        }
        methods::DisableOneAlarmHandle GetDisableOneAlarmHandle()
        {
            return std::move(DisableOneAlarmHandle);
        }
        ara::core::Result<void> Initialize()
        {
            ara::core::Result<void> initResult;
            do {
                if (processMode == ara::com::MethodCallProcessingMode::kEvent) {
                    if(!ptr->SetMethodThreadNumber(ptr->GetMethodThreadNumber(2U), 1024U)) {
                        ara::core::ErrorCode errorCode(ara::com::ComErrc::kNetworkBindingFailure);
                        initResult.EmplaceError(errorCode);
                        break;
                    }
                }
                initResult = ReportAlmProcPolicyHandle.Initialize<ara::core::Future<ReportAlmProcPolicyOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
                initResult = DisableOneAlarmHandle.Initialize<ara::core::Future<DisableOneAlarmOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        methods::ReportAlmProcPolicyHandle ReportAlmProcPolicyHandle;
        methods::DisableOneAlarmHandle DisableOneAlarmHandle;
    };
    explicit FmReportAlmProcPolicyServiceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceId, mode)),
          ReportAlmProcPolicyHandle(skeletonAdapter->GetSkeleton(), methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceId, mode),
          DisableOneAlarmHandle(skeletonAdapter->GetSkeleton(), methods::FmReportAlmProcPolicyServiceDisableOneAlarmId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceId, mode){
        ConstructSkeleton(mode);
    }

    explicit FmReportAlmProcPolicyServiceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceSpec, mode)),
          ReportAlmProcPolicyHandle(skeletonAdapter->GetSkeleton(), methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceSpec, mode),
          DisableOneAlarmHandle(skeletonAdapter->GetSkeleton(), methods::FmReportAlmProcPolicyServiceDisableOneAlarmId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceSpec, mode){
        ConstructSkeleton(mode);
    }

    explicit FmReportAlmProcPolicyServiceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceContainer, mode)),
          ReportAlmProcPolicyHandle(skeletonAdapter->GetSkeleton(), methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceContainer, mode),
          DisableOneAlarmHandle(skeletonAdapter->GetSkeleton(), methods::FmReportAlmProcPolicyServiceDisableOneAlarmId, ::mdc::fm::FmReportAlmProcPolicyService::ServiceIdentifier, instanceContainer, mode){
        ConstructSkeleton(mode);
    }

    FmReportAlmProcPolicyServiceSkeleton(const FmReportAlmProcPolicyServiceSkeleton&) = delete;

    FmReportAlmProcPolicyServiceSkeleton(FmReportAlmProcPolicyServiceSkeleton&&) = default;
    FmReportAlmProcPolicyServiceSkeleton& operator=(FmReportAlmProcPolicyServiceSkeleton&&) = default;
    FmReportAlmProcPolicyServiceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          ReportAlmProcPolicyHandle(token.GetReportAlmProcPolicyHandle()),
          DisableOneAlarmHandle(token.GetDisableOneAlarmHandle()){
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

    virtual ~FmReportAlmProcPolicyServiceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&FmReportAlmProcPolicyServiceSkeleton::E2EErrorHandler, *this);
        skeletonAdapter->RegisterMethod(&FmReportAlmProcPolicyServiceSkeleton::ReportAlmProcPolicy, *this, methods::FmReportAlmProcPolicyServiceReportAlmProcPolicyId);
        skeletonAdapter->RegisterMethod(&FmReportAlmProcPolicyServiceSkeleton::DisableOneAlarm, *this, methods::FmReportAlmProcPolicyServiceDisableOneAlarmId);
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

    virtual ara::core::Future<ReportAlmProcPolicyOutput> ReportAlmProcPolicy(const ::UInt8& retryTimes, const ::UInt8& procPolicy) = 0;
    virtual ara::core::Future<DisableOneAlarmOutput> DisableOneAlarm(const ::UInt16& alarmId, const ::UInt16& alarmObj, const ::UInt16& disable) = 0;
    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode, ara::com::e2e::DataID, ara::com::e2e::MessageCounter){}

    methods::ReportAlmProcPolicyHandle ReportAlmProcPolicyHandle;
    methods::DisableOneAlarmHandle DisableOneAlarmHandle;
};
} // namespace skeleton
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMREPORTALMPROCPOLICYSERVICE_SKELETON_H
