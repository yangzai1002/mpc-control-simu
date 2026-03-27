/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_FM_FMREPORTEVENTSERVICE_PROXY_H
#define MDC_FM_FMREPORTEVENTSERVICE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/fm/fmreporteventservice_common.h"
#include <string>

namespace mdc {
namespace fm {
namespace proxy {
namespace events {
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId FmReportEventServiceReportFaultId = 14388U; //ReportFault_method_hash
static constexpr ara::com::internal::EntityId FmReportEventServiceReportCheckPointId = 3322U; //ReportCheckPoint_method_hash
static constexpr ara::com::internal::EntityId FmReportEventServiceReportProcStateId = 24315U; //ReportProcState_method_hash


class ReportFault {
public:
    using Output = mdc::fm::methods::ReportFault::Output;

    ReportFault(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::fm::FmFaultData>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::fm::FmFaultData& faultData)
    {
        return method_->operator()(faultData);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::fm::FmFaultData>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::fm::FmFaultData>> method_;
};

class ReportCheckPoint {
public:
    using Output = mdc::fm::methods::ReportCheckPoint::Output;

    ReportCheckPoint(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::String>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::String& procName)
    {
        return method_->operator()(procName);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String>> method_;
};

class ReportProcState {
public:
    using Output = mdc::fm::methods::ReportProcState::Output;

    ReportProcState(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::uint8_t>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::String& procName, const ::uint8_t& state)
    {
        return method_->operator()(procName, state);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::uint8_t>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::uint8_t>> method_;
};
} // namespace methods

class FmReportEventServiceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              ReportFault(proxyAdapter->GetProxy(), methods::FmReportEventServiceReportFaultId, proxyAdapter->GetServiceInterfaceInfo()),
              ReportCheckPoint(proxyAdapter->GetProxy(), methods::FmReportEventServiceReportCheckPointId, proxyAdapter->GetServiceInterfaceInfo()),
              ReportProcState(proxyAdapter->GetProxy(), methods::FmReportEventServiceReportProcStateId, proxyAdapter->GetServiceInterfaceInfo()){
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
            initResult = ReportFault.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = ReportCheckPoint.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = ReportProcState.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        methods::ReportFault GetReportFault() noexcept { return std::move(ReportFault); }
        methods::ReportCheckPoint GetReportCheckPoint() noexcept { return std::move(ReportCheckPoint); }
        methods::ReportProcState GetReportProcState() noexcept { return std::move(ReportProcState); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        methods::ReportFault ReportFault;
        methods::ReportCheckPoint ReportCheckPoint;
        methods::ReportProcState ReportProcState;
    };

    virtual ~FmReportEventServiceProxy()
    {

    }

    explicit FmReportEventServiceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::fm::FmReportEventService::ServiceIdentifier, handle)),
          ReportFault(proxyAdapter->GetProxy(), methods::FmReportEventServiceReportFaultId, proxyAdapter->GetServiceInterfaceInfo()),
          ReportCheckPoint(proxyAdapter->GetProxy(), methods::FmReportEventServiceReportCheckPointId, proxyAdapter->GetServiceInterfaceInfo()),
          ReportProcState(proxyAdapter->GetProxy(), methods::FmReportEventServiceReportProcStateId, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultReportFault = ReportFault.GetMethodAdapter()->Initialize();
            ThrowError(resultReportFault);
            ara::core::Result<void> resultReportCheckPoint = ReportCheckPoint.GetMethodAdapter()->Initialize();
            ThrowError(resultReportCheckPoint);
            ara::core::Result<void> resultReportProcState = ReportProcState.GetMethodAdapter()->Initialize();
            ThrowError(resultReportProcState);
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

    FmReportEventServiceProxy(const FmReportEventServiceProxy&) = delete;
    FmReportEventServiceProxy& operator=(const FmReportEventServiceProxy&) = delete;

    FmReportEventServiceProxy(FmReportEventServiceProxy&&) = default;
    FmReportEventServiceProxy& operator=(FmReportEventServiceProxy&&) = default;
    FmReportEventServiceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          ReportFault(token.GetReportFault()),
          ReportCheckPoint(token.GetReportCheckPoint()),
          ReportProcState(token.GetReportProcState()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::fm::FmReportEventService::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<FmReportEventServiceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::fm::FmReportEventService::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::fm::FmReportEventService::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::fm::FmReportEventService::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::fm::FmReportEventService::ServiceIdentifier, specifier);
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
    methods::ReportFault ReportFault;
    methods::ReportCheckPoint ReportCheckPoint;
    methods::ReportProcState ReportProcState;
};
} // namespace proxy
} // namespace fm
} // namespace mdc

#endif // MDC_FM_FMREPORTEVENTSERVICE_PROXY_H
