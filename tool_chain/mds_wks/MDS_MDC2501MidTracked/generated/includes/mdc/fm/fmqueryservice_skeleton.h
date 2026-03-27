/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMQUERYSERVICE_SKELETON_H
#define MDC_FM_FMQUERYSERVICE_SKELETON_H

#include "ara/com/internal/skeleton/skeleton_adapter.h"
#include "ara/com/internal/skeleton/event_adapter.h"
#include "ara/com/internal/skeleton/field_adapter.h"
#include "ara/com/internal/skeleton/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/fm/fmqueryservice_common.h"
#include <cstdint>

namespace mdc {
namespace fm {
namespace skeleton {
namespace events
{
}

namespace methods
{
    using QueryFaultDetailHandle = ara::com::internal::skeleton::method::MethodAdapter;
    using QueryFaultOnFlagHandle = ara::com::internal::skeleton::method::MethodAdapter;
    using QueryFaultStatisticHandle = ara::com::internal::skeleton::method::MethodAdapter;
    using QuerySystemFaultStatusHandle = ara::com::internal::skeleton::method::MethodAdapter;
    static constexpr ara::com::internal::EntityId FmQueryServiceQueryFaultDetailId = 36459U; //QueryFaultDetail_method_hash
    static constexpr ara::com::internal::EntityId FmQueryServiceQueryFaultOnFlagId = 43267U; //QueryFaultOnFlag_method_hash
    static constexpr ara::com::internal::EntityId FmQueryServiceQueryFaultStatisticId = 26435U; //QueryFaultStatistic_method_hash
    static constexpr ara::com::internal::EntityId FmQueryServiceQuerySystemFaultStatusId = 43039U; //QuerySystemFaultStatus_method_hash
}

namespace fields
{
}

class FmQueryServiceSkeleton {
private:
    std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> skeletonAdapter;
    void ConstructSkeleton(const ara::com::MethodCallProcessingMode mode)
    {
        if (mode == ara::com::MethodCallProcessingMode::kEvent) {
            if (!(skeletonAdapter->SetMethodThreadNumber(skeletonAdapter->GetMethodThreadNumber(4U), 1024U))) {
#ifndef NOT_SUPPORT_EXCEPTIONS
                ara::core::ErrorCode errorcode(ara::com::ComErrc::kNetworkBindingFailure);
                throw ara::com::ComException(std::move(errorcode));
#else
                std::cerr << "Error: Not support exception, create skeleton failed!" << std::endl;
#endif
            }
        }
        const ara::core::Result<void> resultQueryFaultDetail = QueryFaultDetailHandle.Initialize<ara::core::Future<QueryFaultDetailOutput>>();
        ThrowError(resultQueryFaultDetail);
        const ara::core::Result<void> resultQueryFaultOnFlag = QueryFaultOnFlagHandle.Initialize<ara::core::Future<QueryFaultOnFlagOutput>>();
        ThrowError(resultQueryFaultOnFlag);
        const ara::core::Result<void> resultQueryFaultStatistic = QueryFaultStatisticHandle.Initialize<ara::core::Future<QueryFaultStatisticOutput>>();
        ThrowError(resultQueryFaultStatistic);
        const ara::core::Result<void> resultQuerySystemFaultStatus = QuerySystemFaultStatusHandle.Initialize<ara::core::Future<QuerySystemFaultStatusOutput>>();
        ThrowError(resultQuerySystemFaultStatus);
    }

    FmQueryServiceSkeleton& operator=(const FmQueryServiceSkeleton&) = delete;

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
    using QueryFaultDetailOutput = mdc::fm::methods::QueryFaultDetail::Output;
    
    using QueryFaultOnFlagOutput = mdc::fm::methods::QueryFaultOnFlag::Output;
    
    using QueryFaultStatisticOutput = mdc::fm::methods::QueryFaultStatistic::Output;
    
    using QuerySystemFaultStatusOutput = mdc::fm::methods::QuerySystemFaultStatus::Output;
    
    class ConstructionToken {
    public:
        explicit ConstructionToken(const ara::com::InstanceIdentifier& instanceId,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode)),
              QueryFaultDetailHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultDetailId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode),
              QueryFaultOnFlagHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultOnFlagId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode),
              QueryFaultStatisticHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultStatisticId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode),
              QuerySystemFaultStatusHandle(ptr->GetSkeleton(), methods::FmQueryServiceQuerySystemFaultStatusId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode){
        }

        explicit ConstructionToken(const ara::core::InstanceSpecifier& instanceSpec,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode)),
              QueryFaultDetailHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultDetailId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode),
              QueryFaultOnFlagHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultOnFlagId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode),
              QueryFaultStatisticHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultStatisticId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode),
              QuerySystemFaultStatusHandle(ptr->GetSkeleton(), methods::FmQueryServiceQuerySystemFaultStatusId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode){
        }

        explicit ConstructionToken(const ara::com::InstanceIdentifierContainer instanceContainer,
            const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
            : processMode(mode),
              ptr(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode)),
              QueryFaultDetailHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultDetailId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode),
              QueryFaultOnFlagHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultOnFlagId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode),
              QueryFaultStatisticHandle(ptr->GetSkeleton(), methods::FmQueryServiceQueryFaultStatisticId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode),
              QuerySystemFaultStatusHandle(ptr->GetSkeleton(), methods::FmQueryServiceQuerySystemFaultStatusId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode){
        }

        ConstructionToken(ConstructionToken&& other)
            : processMode(std::move(other.processMode)),
              ptr(std::move(other.ptr)),
              QueryFaultDetailHandle(std::move(other.QueryFaultDetailHandle)),
              QueryFaultOnFlagHandle(std::move(other.QueryFaultOnFlagHandle)),
              QueryFaultStatisticHandle(std::move(other.QueryFaultStatisticHandle)),
              QuerySystemFaultStatusHandle(std::move(other.QuerySystemFaultStatusHandle)){
        }
        ConstructionToken& operator=(ConstructionToken && other)
        {
            if (&other != this) {
                processMode = std::move(other.processMode);
                ptr = std::move(other.ptr);
                QueryFaultDetailHandle = std::move(other.QueryFaultDetailHandle);
                QueryFaultOnFlagHandle = std::move(other.QueryFaultOnFlagHandle);
                QueryFaultStatisticHandle = std::move(other.QueryFaultStatisticHandle);
                QuerySystemFaultStatusHandle = std::move(other.QuerySystemFaultStatusHandle);
            }
            return *this;
        }
        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> GetSkeletonAdapter()
        {
            return std::move(ptr);
        }
        methods::QueryFaultDetailHandle GetQueryFaultDetailHandle()
        {
            return std::move(QueryFaultDetailHandle);
        }
        methods::QueryFaultOnFlagHandle GetQueryFaultOnFlagHandle()
        {
            return std::move(QueryFaultOnFlagHandle);
        }
        methods::QueryFaultStatisticHandle GetQueryFaultStatisticHandle()
        {
            return std::move(QueryFaultStatisticHandle);
        }
        methods::QuerySystemFaultStatusHandle GetQuerySystemFaultStatusHandle()
        {
            return std::move(QuerySystemFaultStatusHandle);
        }
        ara::core::Result<void> Initialize()
        {
            ara::core::Result<void> initResult;
            do {
                if (processMode == ara::com::MethodCallProcessingMode::kEvent) {
                    if(!ptr->SetMethodThreadNumber(ptr->GetMethodThreadNumber(4U), 1024U)) {
                        ara::core::ErrorCode errorCode(ara::com::ComErrc::kNetworkBindingFailure);
                        initResult.EmplaceError(errorCode);
                        break;
                    }
                }
                initResult = QueryFaultDetailHandle.Initialize<ara::core::Future<QueryFaultDetailOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
                initResult = QueryFaultOnFlagHandle.Initialize<ara::core::Future<QueryFaultOnFlagOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
                initResult = QueryFaultStatisticHandle.Initialize<ara::core::Future<QueryFaultStatisticOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
                initResult = QuerySystemFaultStatusHandle.Initialize<ara::core::Future<QuerySystemFaultStatusOutput>>();
                if (!initResult.HasValue()) {
                    break;
                }
            } while(false);

            return initResult;
        }
    private:
        ara::com::MethodCallProcessingMode processMode;
        std::unique_ptr<ara::com::internal::skeleton::SkeletonAdapter> ptr;
        methods::QueryFaultDetailHandle QueryFaultDetailHandle;
        methods::QueryFaultOnFlagHandle QueryFaultOnFlagHandle;
        methods::QueryFaultStatisticHandle QueryFaultStatisticHandle;
        methods::QuerySystemFaultStatusHandle QuerySystemFaultStatusHandle;
    };
    explicit FmQueryServiceSkeleton(const ara::com::InstanceIdentifier& instanceId,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode)),
          QueryFaultDetailHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultDetailId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode),
          QueryFaultOnFlagHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultOnFlagId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode),
          QueryFaultStatisticHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultStatisticId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode),
          QuerySystemFaultStatusHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQuerySystemFaultStatusId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceId, mode){
        ConstructSkeleton(mode);
    }

    explicit FmQueryServiceSkeleton(const ara::core::InstanceSpecifier& instanceSpec,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode)),
          QueryFaultDetailHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultDetailId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode),
          QueryFaultOnFlagHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultOnFlagId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode),
          QueryFaultStatisticHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultStatisticId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode),
          QuerySystemFaultStatusHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQuerySystemFaultStatusId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceSpec, mode){
        ConstructSkeleton(mode);
    }

    explicit FmQueryServiceSkeleton(const ara::com::InstanceIdentifierContainer instanceContainer,
                           const ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        :skeletonAdapter(std::make_unique<ara::com::internal::skeleton::SkeletonAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode)),
          QueryFaultDetailHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultDetailId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode),
          QueryFaultOnFlagHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultOnFlagId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode),
          QueryFaultStatisticHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQueryFaultStatisticId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode),
          QuerySystemFaultStatusHandle(skeletonAdapter->GetSkeleton(), methods::FmQueryServiceQuerySystemFaultStatusId, ::mdc::fm::FmQueryService::ServiceIdentifier, instanceContainer, mode){
        ConstructSkeleton(mode);
    }

    FmQueryServiceSkeleton(const FmQueryServiceSkeleton&) = delete;

    FmQueryServiceSkeleton(FmQueryServiceSkeleton&&) = default;
    FmQueryServiceSkeleton& operator=(FmQueryServiceSkeleton&&) = default;
    FmQueryServiceSkeleton(ConstructionToken&& token) noexcept
        : skeletonAdapter(token.GetSkeletonAdapter()),
          QueryFaultDetailHandle(token.GetQueryFaultDetailHandle()),
          QueryFaultOnFlagHandle(token.GetQueryFaultOnFlagHandle()),
          QueryFaultStatisticHandle(token.GetQueryFaultStatisticHandle()),
          QuerySystemFaultStatusHandle(token.GetQuerySystemFaultStatusHandle()){
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

    virtual ~FmQueryServiceSkeleton()
    {
        StopOfferService();
    }

    void OfferService()
    {
        skeletonAdapter->RegisterE2EErrorHandler(&FmQueryServiceSkeleton::E2EErrorHandler, *this);
        skeletonAdapter->RegisterMethod(&FmQueryServiceSkeleton::QueryFaultDetail, *this, methods::FmQueryServiceQueryFaultDetailId);
        skeletonAdapter->RegisterMethod(&FmQueryServiceSkeleton::QueryFaultOnFlag, *this, methods::FmQueryServiceQueryFaultOnFlagId);
        skeletonAdapter->RegisterMethod(&FmQueryServiceSkeleton::QueryFaultStatistic, *this, methods::FmQueryServiceQueryFaultStatisticId);
        skeletonAdapter->RegisterMethod(&FmQueryServiceSkeleton::QuerySystemFaultStatus, *this, methods::FmQueryServiceQuerySystemFaultStatusId);
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

    virtual ara::core::Future<QueryFaultDetailOutput> QueryFaultDetail() = 0;
    virtual ara::core::Future<QueryFaultOnFlagOutput> QueryFaultOnFlag(const ::UInt32& flag) = 0;
    virtual ara::core::Future<QueryFaultStatisticOutput> QueryFaultStatistic(const ::UInt32& flag) = 0;
    virtual ara::core::Future<QuerySystemFaultStatusOutput> QuerySystemFaultStatus(const ::UInt32& qryFlag) = 0;
    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode, ara::com::e2e::DataID, ara::com::e2e::MessageCounter){}

    methods::QueryFaultDetailHandle QueryFaultDetailHandle;
    methods::QueryFaultOnFlagHandle QueryFaultOnFlagHandle;
    methods::QueryFaultStatisticHandle QueryFaultStatisticHandle;
    methods::QuerySystemFaultStatusHandle QuerySystemFaultStatusHandle;
};
} // namespace skeleton
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMQUERYSERVICE_SKELETON_H
