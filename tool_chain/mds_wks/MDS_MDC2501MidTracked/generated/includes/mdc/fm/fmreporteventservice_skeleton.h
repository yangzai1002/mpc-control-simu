/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMREPORTEVENTSERVICE_SKELETON_H
#define MDC_FM_FMREPORTEVENTSERVICE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/fm/fmreporteventservice_common.h"
#include <cstdint>

namespace mdc {
namespace fm {
namespace skeleton {
namespace events
{
}

namespace methods
{
    using ReportFaultHandle = ara::com::internal::skeleton::method::MethodAdapter;
    using ReportCheckPointHandle = ara::com::internal::skeleton::method::MethodAdapter;
    using ReportProcStateHandle = ara::com::internal::skeleton::method::MethodAdapter;
    static constexpr ara::com::internal::EntityId FmReportEventServiceReportFaultId = 14388U; //ReportFault_method_hash
    static constexpr ara::com::internal::EntityId FmReportEventServiceReportCheckPointId = 3322U; //ReportCheckPoint_method_hash
    static constexpr ara::com::internal::EntityId FmReportEventServiceReportProcStateId = 24315U; //ReportProcState_method_hash
}

namespace fields
{
}

class FmReportEventServiceSkeleton {
private:
    std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> skeletonAdapter;
    void ConstructSkeleton(const ara::com::MethodCallProcessingMode mode)
    {
        if (mode == ara::com::MethodCallProcessingMode::kEvent) {
            if (!(skeletonAdapter->SetMethodThreadNumber(skeletonAdapter->GetMethodThreadNumber(3U), 1024U))) {
#ifndef NOT_SUPPORT_EXCEPTIONS
                ara::core::ErrorCode errorcode(ara::com::ComErrc::kNetworkBindingFailure);
                throw ara::com::ComException(std::move(errorcode));
#else
                std::cerr << "Error: Not support exception, create skeleton failed!" << std::endl;
#endif
            }
        }
        const ara::core::Result<void> resultReportFault = ReportFaultHandle.Initialize<ara::core::Future<ReportFaultOutput>>();
        ThrowError(resultReportFault);
        const ara::core::Result<void> resultReportCheckPoint = ReportCheckPointHandle.Initialize<ara::core::Future<ReportCheckPointOutput>>();
        ThrowError(resultReportCheckPoint);
        const ara::core::Result<void> resultReportProcState = ReportProcStateHandle.Initialize<ara::core::Future<ReportProcStateOutput>>();
        ThrowError(resultReportProcState);
    }

    FmReportEventServiceSkeleton& operator=(const FmReportEventServiceSkeleton&) = delete;

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
    using ReportFaultOutput = mdc::fm::methods::ReportFault::Output;
    
    using ReportCheckPointOutput = mdc::fm::methods::ReportCheckPoint::Output;
    
    using ReportProcStateOutput = mdc::fm::methods::ReportProcState::Output;
    
    class ConstructionToken {
    public:
        explicit ConstructionToken(const ara::com::InstanceIdentifier& instanceId,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode)),
              ReportFaultHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportFaultId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode),
              ReportCheckPointHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportCheckPointId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode),
              ReportProcStateHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportProcStateId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode)),
              ReportFaultHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportFaultId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode),
              ReportCheckPointHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportCheckPointId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode),
              ReportProcStateHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportProcStateId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode)),
              ReportFaultHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportFaultId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode),
              ReportCheckPointHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportCheckPointId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode),
              ReportProcStateHandle(ptr->GetSkeleton(), methods::FmReportEventServiceReportProcStateId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              ReportFaultHandle(std::move(other.ReportFaultHandle)),
              ReportCheckPointHandle(std::move(other.ReportCheckPointHandle)),
              ReportProcStateHandle(std::move(other.ReportProcStateHandle)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                ReportFaultHandle = std::move(other.ReportFaultHandle);
                ReportCheckPointHandle = std::move(other.ReportCheckPointHandle);
                ReportProcStateHandle = std::move(other.ReportProcStateHandle);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        methods::ReportFaultHandle GetReportFaultHandle()
        {
            return std::move(ReportFaultHandle);
        }
        methods::ReportCheckPointHandle GetReportCheckPointHandle()
        {
            return std::move(ReportCheckPointHandle);
        }
        methods::ReportProcStateHandle GetReportProcStateHandle()
        {
            return std::move(ReportProcStateHandle);
        }
        ara::core::Result<void> Initialize()
        {
            ara::core::Result<void> initResult;
            do {
                if (processMode == ara::com::MethodCallProcessingMode::kEvent) {
                    if(!ptr->SetMethodThreadNumber(ptr->GetMethodThreadNumber(3U), 1024U)) {
                        ara::core::ErrorCode errorCode(ara::com::ComErrc::kNetworkBindingFailure);
                        initResult.EmplaceError(errorCode);
                        break;
                    }
                }
                initResult = ReportFaultHandle.Initialize<ara::core::Future<ReportFaultOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
                initResult = ReportCheckPointHandle.Initialize<ara::core::Future<ReportCheckPointOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
                initResult = ReportProcStateHandle.Initialize<ara::core::Future<ReportProcStateOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        methods::ReportFaultHandle ReportFaultHandle;
        methods::ReportCheckPointHandle ReportCheckPointHandle;
        methods::ReportProcStateHandle ReportProcStateHandle;
    };
    explicit FmReportEventServiceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode)),
          ReportFaultHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportFaultId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode),
          ReportCheckPointHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportCheckPointId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode),
          ReportProcStateHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportProcStateId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceId, mode){
        ConstructSkeleton(mode);
    }

    explicit FmReportEventServiceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode)),
          ReportFaultHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportFaultId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode),
          ReportCheckPointHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportCheckPointId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode),
          ReportProcStateHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportProcStateId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceSpec, mode){
        ConstructSkeleton(mode);
    }

    explicit FmReportEventServiceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode)),
          ReportFaultHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportFaultId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode),
          ReportCheckPointHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportCheckPointId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode),
          ReportProcStateHandle(skeletonAdapter->GetSkeleton(), methods::FmReportEventServiceReportProcStateId, ::mdc::fm::FmReportEventService::ServiceIdentifier, instanceContainer, mode){
        ConstructSkeleton(mode);
    }

    FmReportEventServiceSkeleton(const FmReportEventServiceSkeleton&) = delete;

    FmReportEventServiceSkeleton(FmReportEventServiceSkeleton&&) = default;
    FmReportEventServiceSkeleton& operator=(FmReportEventServiceSkeleton&&) = default;
    FmReportEventServiceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          ReportFaultHandle(token.GetReportFaultHandle()),
          ReportCheckPointHandle(token.GetReportCheckPointHandle()),
          ReportProcStateHandle(token.GetReportProcStateHandle()){
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

    virtual ~FmReportEventServiceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&FmReportEventServiceSkeleton::E2EErrorHandler, *this);
        skeletonAdapter->RegisterMethod(&FmReportEventServiceSkeleton::ReportFault, *this, methods::FmReportEventServiceReportFaultId);
        skeletonAdapter->RegisterMethod(&FmReportEventServiceSkeleton::ReportCheckPoint, *this, methods::FmReportEventServiceReportCheckPointId);
        skeletonAdapter->RegisterMethod(&FmReportEventServiceSkeleton::ReportProcState, *this, methods::FmReportEventServiceReportProcStateId);
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

    virtual ara::core::Future<ReportFaultOutput> ReportFault(const ::mdc::fm::FmFaultData& faultData) = 0;
    virtual ara::core::Future<ReportCheckPointOutput> ReportCheckPoint(const ::String& procName) = 0;
    virtual ara::core::Future<ReportProcStateOutput> ReportProcState(const ::String& procName, const ::uint8_t& state) = 0;
    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode, ara::com::e2e::DataID, ara::com::e2e::MessageCounter){}

    methods::ReportFaultHandle ReportFaultHandle;
    methods::ReportCheckPointHandle ReportCheckPointHandle;
    methods::ReportProcStateHandle ReportProcStateHandle;
};
} // namespace skeleton
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMREPORTEVENTSERVICE_SKELETON_H
