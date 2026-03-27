/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_DIAG_DIAGCOMCLIENT_PROXY_H
#define ARA_DIAG_DIAGCOMCLIENT_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "ara/diag/diagcomclient_common.h"
#include <string>

namespace ara {
namespace diag {
namespace proxy {
namespace events {
    using RequestTrigger = ara::com::internal::proxy::event::EventAdapter<::ara::diag::RequestTriggerType>;
    static constexpr ara::com::internal::EntityId DiagComClientRequestTriggerId = 52019U; //RequestTrigger_event_hash
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId DiagComClientGetRequestId = 62768U; //GetRequest_method_hash
static constexpr ara::com::internal::EntityId DiagComClientSendResponseId = 17402U; //SendResponse_method_hash


class GetRequest {
public:
    using Output = ara::diag::methods::GetRequest::Output;

    GetRequest(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::UInt64>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::String& spec, const ::UInt64& serialNumber)
    {
        return method_->operator()(spec, serialNumber);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::UInt64>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::UInt64>> method_;
};

class SendResponse {
public:
    using Output = void;

    SendResponse(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::UInt64, ::Boolean, ::ara::diag::ByteVector>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<void> operator()(const ::String& spec, const ::UInt64& serialNumber, const ::Boolean& isPositive, const ::ara::diag::ByteVector& responseData)
    {
        return method_->operator()(spec, serialNumber, isPositive, responseData);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::UInt64, ::Boolean, ::ara::diag::ByteVector>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::String, ::UInt64, ::Boolean, ::ara::diag::ByteVector>> method_;
};
} // namespace methods

class DiagComClientProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              RequestTrigger(proxyAdapter->GetProxy(), events::DiagComClientRequestTriggerId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()),
              GetRequest(proxyAdapter->GetProxy(), methods::DiagComClientGetRequestId, proxyAdapter->GetServiceInterfaceInfo()),
              SendResponse(proxyAdapter->GetProxy(), methods::DiagComClientSendResponseId, proxyAdapter->GetServiceInterfaceInfo()){
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
            initResult = GetRequest.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = SendResponse.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        events::RequestTrigger GetRequestTrigger() noexcept { return std::move(RequestTrigger); }
        methods::GetRequest GetGetRequest() noexcept { return std::move(GetRequest); }
        methods::SendResponse GetSendResponse() noexcept { return std::move(SendResponse); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::RequestTrigger RequestTrigger;
        methods::GetRequest GetRequest;
        methods::SendResponse SendResponse;
    };

    virtual ~DiagComClientProxy()
    {
        RequestTrigger.UnsetReceiveHandler();
        RequestTrigger.Unsubscribe();

    }

    explicit DiagComClientProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::ara::diag::DiagComClient::ServiceIdentifier, handle)),
          RequestTrigger(proxyAdapter->GetProxy(), events::DiagComClientRequestTriggerId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()),
          GetRequest(proxyAdapter->GetProxy(), methods::DiagComClientGetRequestId, proxyAdapter->GetServiceInterfaceInfo()),
          SendResponse(proxyAdapter->GetProxy(), methods::DiagComClientSendResponseId, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultGetRequest = GetRequest.GetMethodAdapter()->Initialize();
            ThrowError(resultGetRequest);
            ara::core::Result<void> resultSendResponse = SendResponse.GetMethodAdapter()->Initialize();
            ThrowError(resultSendResponse);
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

    DiagComClientProxy(const DiagComClientProxy&) = delete;
    DiagComClientProxy& operator=(const DiagComClientProxy&) = delete;

    DiagComClientProxy(DiagComClientProxy&&) = default;
    DiagComClientProxy& operator=(DiagComClientProxy&&) = default;
    DiagComClientProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          RequestTrigger(token.GetRequestTrigger()),
          GetRequest(token.GetGetRequest()),
          SendResponse(token.GetSendResponse()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::ara::diag::DiagComClient::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<DiagComClientProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::ara::diag::DiagComClient::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::ara::diag::DiagComClient::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::ara::diag::DiagComClient::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::ara::diag::DiagComClient::ServiceIdentifier, specifier);
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
    events::RequestTrigger RequestTrigger;
    methods::GetRequest GetRequest;
    methods::SendResponse SendResponse;
};
} // namespace proxy
} // namespace diag
} // namespace ara

#endif // ARA_DIAG_DIAGCOMCLIENT_PROXY_H
