/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMQUERYSERVICE_PROXY_H
#define MDC_FM_FMQUERYSERVICE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/fm/fmqueryservice_common.h"
#include <string>

namespace mdc {
namespace fm {
namespace proxy {
namespace events {
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId FmQueryServiceQueryFaultDetailId = 36459U; //QueryFaultDetail_method_hash
static constexpr ara::com::internal::EntityId FmQueryServiceQueryFaultOnFlagId = 43267U; //QueryFaultOnFlag_method_hash
static constexpr ara::com::internal::EntityId FmQueryServiceQueryFaultStatisticId = 26435U; //QueryFaultStatistic_method_hash
static constexpr ara::com::internal::EntityId FmQueryServiceQuerySystemFaultStatusId = 43039U; //QuerySystemFaultStatus_method_hash


class QueryFaultDetail {
public:
    using Output = mdc::fm::methods::QueryFaultDetail::Output;

    QueryFaultDetail(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()()
    {
        return method_->operator()();
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output>> method_;
};

class QueryFaultOnFlag {
public:
    using Output = mdc::fm::methods::QueryFaultOnFlag::Output;

    QueryFaultOnFlag(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::UInt32& flag)
    {
        return method_->operator()(flag);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>> method_;
};

class QueryFaultStatistic {
public:
    using Output = mdc::fm::methods::QueryFaultStatistic::Output;

    QueryFaultStatistic(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::UInt32& flag)
    {
        return method_->operator()(flag);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>> method_;
};

class QuerySystemFaultStatus {
public:
    using Output = mdc::fm::methods::QuerySystemFaultStatus::Output;

    QuerySystemFaultStatus(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::UInt32& qryFlag)
    {
        return method_->operator()(qryFlag);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::UInt32>> method_;
};
} // namespace methods

class FmQueryServiceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              QueryFaultDetail(proxyAdapter->GetProxy(), methods::FmQueryServiceQueryFaultDetailId, proxyAdapter->GetServiceInterfaceInfo()),
              QueryFaultOnFlag(proxyAdapter->GetProxy(), methods::FmQueryServiceQueryFaultOnFlagId, proxyAdapter->GetServiceInterfaceInfo()),
              QueryFaultStatistic(proxyAdapter->GetProxy(), methods::FmQueryServiceQueryFaultStatisticId, proxyAdapter->GetServiceInterfaceInfo()),
              QuerySystemFaultStatus(proxyAdapter->GetProxy(), methods::FmQueryServiceQuerySystemFaultStatusId, proxyAdapter->GetServiceInterfaceInfo()){
        }

        ConstructionToken(ConstructionToken&& other) = default;
        ConstructionToken& operator=(ConstructionToken && other) = default;

        ConstructionToken(const ConstructionToken&) = delete;
        ConstructionToken& operator = (const ConstructionToken&) = delete;
        
        virtual ~ConstructionToken() = default;

        /* Do not use such internal interfaces!!! */
        ara::core::Result<void> InitializeAll()
        {
            ara::core::Result<void> initResult;
            initResult = QueryFaultDetail.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = QueryFaultOnFlag.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = QueryFaultStatistic.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = QuerySystemFaultStatus.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        methods::QueryFaultDetail GetQueryFaultDetail() noexcept { return std::move(QueryFaultDetail); }
        methods::QueryFaultOnFlag GetQueryFaultOnFlag() noexcept { return std::move(QueryFaultOnFlag); }
        methods::QueryFaultStatistic GetQueryFaultStatistic() noexcept { return std::move(QueryFaultStatistic); }
        methods::QuerySystemFaultStatus GetQuerySystemFaultStatus() noexcept { return std::move(QuerySystemFaultStatus); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        methods::QueryFaultDetail QueryFaultDetail;
        methods::QueryFaultOnFlag QueryFaultOnFlag;
        methods::QueryFaultStatistic QueryFaultStatistic;
        methods::QuerySystemFaultStatus QuerySystemFaultStatus;
    };

    virtual ~FmQueryServiceProxy()
    {

    }

    explicit FmQueryServiceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::fm::FmQueryService::ServiceIdentifier, handle)),
          QueryFaultDetail(proxyAdapter->GetProxy(), methods::FmQueryServiceQueryFaultDetailId, proxyAdapter->GetServiceInterfaceInfo()),
          QueryFaultOnFlag(proxyAdapter->GetProxy(), methods::FmQueryServiceQueryFaultOnFlagId, proxyAdapter->GetServiceInterfaceInfo()),
          QueryFaultStatistic(proxyAdapter->GetProxy(), methods::FmQueryServiceQueryFaultStatisticId, proxyAdapter->GetServiceInterfaceInfo()),
          QuerySystemFaultStatus(proxyAdapter->GetProxy(), methods::FmQueryServiceQuerySystemFaultStatusId, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultQueryFaultDetail = QueryFaultDetail.GetMethodAdapter()->Initialize();
            ThrowError(resultQueryFaultDetail);
            ara::core::Result<void> resultQueryFaultOnFlag = QueryFaultOnFlag.GetMethodAdapter()->Initialize();
            ThrowError(resultQueryFaultOnFlag);
            ara::core::Result<void> resultQueryFaultStatistic = QueryFaultStatistic.GetMethodAdapter()->Initialize();
            ThrowError(resultQueryFaultStatistic);
            ara::core::Result<void> resultQuerySystemFaultStatus = QuerySystemFaultStatus.GetMethodAdapter()->Initialize();
            ThrowError(resultQuerySystemFaultStatus);
        }

    void ThrowError(const ara::core::Result<void>& result) const
    {
        if (!(result.HasValue())) {
#ifndef NOT_SUPPORT_EXCEPTIONS
            ara::core::ErrorCode errorcode(result.Error());
            throw ara::com::ComException(std::move(errorcode));
#else
            std::cerr << "Error: Not support exception, create proxy failed!"<< std::endl;
#endif
        }
    }

    FmQueryServiceProxy(const FmQueryServiceProxy&) = delete;
    FmQueryServiceProxy& operator=(const FmQueryServiceProxy&) = delete;

    FmQueryServiceProxy(FmQueryServiceProxy&&) = default;
    FmQueryServiceProxy& operator=(FmQueryServiceProxy&&) = default;
    FmQueryServiceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          QueryFaultDetail(token.GetQueryFaultDetail()),
          QueryFaultOnFlag(token.GetQueryFaultOnFlag()),
          QueryFaultStatistic(token.GetQueryFaultStatistic()),
          QuerySystemFaultStatus(token.GetQuerySystemFaultStatus()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::fm::FmQueryService::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<FmQueryServiceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::fm::FmQueryService::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::fm::FmQueryService::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::fm::FmQueryService::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::fm::FmQueryService::ServiceIdentifier, specifier);
    }

    static void StopFindService(const ara::com::FindServiceHandle& handle)
    {
        ara::com::internal::proxy::ProxyAdapter::StopFindService(handle);
    }

    HandleType GetHandle() const
    {
        return proxyAdapter->GetHandle();
    }
    bool SetEventThreadNumber(const std::uint16_t number, const std::uint16_t queueSize)
    {
        return proxyAdapter->SetEventThreadNumber(number, queueSize);
    }
    methods::QueryFaultDetail QueryFaultDetail;
    methods::QueryFaultOnFlag QueryFaultOnFlag;
    methods::QueryFaultStatistic QueryFaultStatistic;
    methods::QuerySystemFaultStatus QuerySystemFaultStatus;
};
} // namespace proxy
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMQUERYSERVICE_PROXY_H
