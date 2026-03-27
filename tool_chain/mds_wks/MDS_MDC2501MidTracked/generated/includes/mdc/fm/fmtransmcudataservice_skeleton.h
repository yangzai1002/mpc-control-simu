/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMTRANSMCUDATASERVICE_SKELETON_H
#define MDC_FM_FMTRANSMCUDATASERVICE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/fm/fmtransmcudataservice_common.h"
#include <cstdint>

namespace mdc {
namespace fm {
namespace skeleton {
namespace events
{
}

namespace methods
{
    using ReportMcuDataHandle = ara::com::internal::skeleton::method::MethodAdapter;
    static constexpr ara::com::internal::EntityId FmTransMcuDataServiceReportMcuDataId = 31537U; //ReportMcuData_method_hash
}

namespace fields
{
}

class FmTransMcuDataServiceSkeleton {
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
        const ara::core::Result<void> resultReportMcuData = ReportMcuDataHandle.Initialize<ara::core::Future<ReportMcuDataOutput>>();
        ThrowError(resultReportMcuData);
    }

    FmTransMcuDataServiceSkeleton& operator=(const FmTransMcuDataServiceSkeleton&) = delete;

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
    using ReportMcuDataOutput = mdc::fm::methods::ReportMcuData::Output;
    
    class ConstructionToken {
    public:
        explicit ConstructionToken(const ara::com::InstanceIdentifier& instanceId,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceId, mode)),
              ReportMcuDataHandle(ptr->GetSkeleton(), methods::FmTransMcuDataServiceReportMcuDataId, ::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceId, mode){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceSpec, mode)),
              ReportMcuDataHandle(ptr->GetSkeleton(), methods::FmTransMcuDataServiceReportMcuDataId, ::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceSpec, mode){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceContainer, mode)),
              ReportMcuDataHandle(ptr->GetSkeleton(), methods::FmTransMcuDataServiceReportMcuDataId, ::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceContainer, mode){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              ReportMcuDataHandle(std::move(other.ReportMcuDataHandle)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                ReportMcuDataHandle = std::move(other.ReportMcuDataHandle);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        methods::ReportMcuDataHandle GetReportMcuDataHandle()
        {
            return std::move(ReportMcuDataHandle);
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
                initResult = ReportMcuDataHandle.Initialize<ara::core::Future<ReportMcuDataOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        methods::ReportMcuDataHandle ReportMcuDataHandle;
    };
    explicit FmTransMcuDataServiceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceId, mode)),
          ReportMcuDataHandle(skeletonAdapter->GetSkeleton(), methods::FmTransMcuDataServiceReportMcuDataId, ::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceId, mode){
        ConstructSkeleton(mode);
    }

    explicit FmTransMcuDataServiceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceSpec, mode)),
          ReportMcuDataHandle(skeletonAdapter->GetSkeleton(), methods::FmTransMcuDataServiceReportMcuDataId, ::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceSpec, mode){
        ConstructSkeleton(mode);
    }

    explicit FmTransMcuDataServiceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceContainer, mode)),
          ReportMcuDataHandle(skeletonAdapter->GetSkeleton(), methods::FmTransMcuDataServiceReportMcuDataId, ::mdc::fm::FmTransMcuDataService::ServiceIdentifier, instanceContainer, mode){
        ConstructSkeleton(mode);
    }

    FmTransMcuDataServiceSkeleton(const FmTransMcuDataServiceSkeleton&) = delete;

    FmTransMcuDataServiceSkeleton(FmTransMcuDataServiceSkeleton&&) = default;
    FmTransMcuDataServiceSkeleton& operator=(FmTransMcuDataServiceSkeleton&&) = default;
    FmTransMcuDataServiceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          ReportMcuDataHandle(token.GetReportMcuDataHandle()){
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

    virtual ~FmTransMcuDataServiceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&FmTransMcuDataServiceSkeleton::E2EErrorHandler, *this);
        skeletonAdapter->RegisterMethod(&FmTransMcuDataServiceSkeleton::ReportMcuData, *this, methods::FmTransMcuDataServiceReportMcuDataId);
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

    virtual ara::core::Future<ReportMcuDataOutput> ReportMcuData(const ::mdc::fm::FmMcuDataHeader& header, const ::Uint8Vector& dataBuf) = 0;
    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode, ara::com::e2e::DataID, ara::com::e2e::MessageCounter){}

    methods::ReportMcuDataHandle ReportMcuDataHandle;
};
} // namespace skeleton
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMTRANSMCUDATASERVICE_SKELETON_H
