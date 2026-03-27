/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_SM_PLATFORMSTATEAGENT_PROXY_H
#define ARA_SM_PLATFORMSTATEAGENT_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "ara/sm/platformstateagent_common.h"
#include <string>

namespace ara {
namespace sm {
namespace proxy {
namespace events {
    using PlatformStateEvent = ara::com::internal::proxy::event::EventAdapter<::ara::sm::PlatformStateMsg>;
    static constexpr ara::com::internal::EntityId PlatformStateAgentPlatformStateEventId = 43376U; //PlatformStateEvent_event_hash
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId PlatformStateAgentQueryPlatformStateId = 44471U; //QueryPlatformState_method_hash
static constexpr ara::com::internal::EntityId PlatformStateAgentRequestPlatformStateId = 37348U; //RequestPlatformState_method_hash


class QueryPlatformState {
public:
    using Output = ara::sm::methods::QueryPlatformState::Output;

    QueryPlatformState(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
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

class RequestPlatformState {
public:
    using Output = ara::sm::methods::RequestPlatformState::Output;

    RequestPlatformState(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::uint8_t, ::Uint32Vector>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::uint8_t& platformStateIn, const ::Uint32Vector& data)
    {
        return method_->operator()(platformStateIn, data);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::uint8_t, ::Uint32Vector>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::uint8_t, ::Uint32Vector>> method_;
};
} // namespace methods

class PlatformStateAgentProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              PlatformStateEvent(proxyAdapter->GetProxy(), events::PlatformStateAgentPlatformStateEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()),
              QueryPlatformState(proxyAdapter->GetProxy(), methods::PlatformStateAgentQueryPlatformStateId, proxyAdapter->GetServiceInterfaceInfo()),
              RequestPlatformState(proxyAdapter->GetProxy(), methods::PlatformStateAgentRequestPlatformStateId, proxyAdapter->GetServiceInterfaceInfo()){
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
            initResult = QueryPlatformState.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = RequestPlatformState.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        events::PlatformStateEvent GetPlatformStateEvent() noexcept { return std::move(PlatformStateEvent); }
        methods::QueryPlatformState GetQueryPlatformState() noexcept { return std::move(QueryPlatformState); }
        methods::RequestPlatformState GetRequestPlatformState() noexcept { return std::move(RequestPlatformState); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::PlatformStateEvent PlatformStateEvent;
        methods::QueryPlatformState QueryPlatformState;
        methods::RequestPlatformState RequestPlatformState;
    };

    virtual ~PlatformStateAgentProxy()
    {
        PlatformStateEvent.UnsetReceiveHandler();
        PlatformStateEvent.Unsubscribe();

    }

    explicit PlatformStateAgentProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::ara::sm::PlatformStateAgent::ServiceIdentifier, handle)),
          PlatformStateEvent(proxyAdapter->GetProxy(), events::PlatformStateAgentPlatformStateEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()),
          QueryPlatformState(proxyAdapter->GetProxy(), methods::PlatformStateAgentQueryPlatformStateId, proxyAdapter->GetServiceInterfaceInfo()),
          RequestPlatformState(proxyAdapter->GetProxy(), methods::PlatformStateAgentRequestPlatformStateId, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultQueryPlatformState = QueryPlatformState.GetMethodAdapter()->Initialize();
            ThrowError(resultQueryPlatformState);
            ara::core::Result<void> resultRequestPlatformState = RequestPlatformState.GetMethodAdapter()->Initialize();
            ThrowError(resultRequestPlatformState);
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

    PlatformStateAgentProxy(const PlatformStateAgentProxy&) = delete;
    PlatformStateAgentProxy& operator=(const PlatformStateAgentProxy&) = delete;

    PlatformStateAgentProxy(PlatformStateAgentProxy&&) = default;
    PlatformStateAgentProxy& operator=(PlatformStateAgentProxy&&) = default;
    PlatformStateAgentProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          PlatformStateEvent(token.GetPlatformStateEvent()),
          QueryPlatformState(token.GetQueryPlatformState()),
          RequestPlatformState(token.GetRequestPlatformState()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::ara::sm::PlatformStateAgent::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<PlatformStateAgentProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::ara::sm::PlatformStateAgent::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::ara::sm::PlatformStateAgent::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::ara::sm::PlatformStateAgent::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::ara::sm::PlatformStateAgent::ServiceIdentifier, specifier);
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
    events::PlatformStateEvent PlatformStateEvent;
    methods::QueryPlatformState QueryPlatformState;
    methods::RequestPlatformState RequestPlatformState;
};
} // namespace proxy
} // namespace sm
} // namespace ara

#endif // ARA_SM_PLATFORMSTATEAGENT_PROXY_H
