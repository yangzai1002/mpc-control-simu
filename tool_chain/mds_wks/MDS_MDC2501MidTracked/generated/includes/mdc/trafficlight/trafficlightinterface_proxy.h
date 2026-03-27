/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_TRAFFICLIGHT_TRAFFICLIGHTINTERFACE_PROXY_H
#define MDC_TRAFFICLIGHT_TRAFFICLIGHTINTERFACE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/trafficlight/trafficlightinterface_common.h"
#include <string>

namespace mdc {
namespace trafficlight {
namespace proxy {
namespace events {
    using TrafficLightArrayEvent = ara::com::internal::proxy::event::EventAdapter<::ara::trafficlight::TrafficLightArray>;
    static constexpr ara::com::internal::EntityId TrafficLightInterfaceTrafficLightArrayEventId = 15969U; //TrafficLightArrayEvent_event_hash
}

namespace fields {
}

namespace methods {

} // namespace methods

class TrafficLightInterfaceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              TrafficLightArrayEvent(proxyAdapter->GetProxy(), events::TrafficLightInterfaceTrafficLightArrayEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        events::TrafficLightArrayEvent GetTrafficLightArrayEvent() noexcept { return std::move(TrafficLightArrayEvent); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::TrafficLightArrayEvent TrafficLightArrayEvent;
    };

    virtual ~TrafficLightInterfaceProxy()
    {
        TrafficLightArrayEvent.UnsetReceiveHandler();
        TrafficLightArrayEvent.Unsubscribe();

    }

    explicit TrafficLightInterfaceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::trafficlight::TrafficLightInterface::ServiceIdentifier, handle)),
          TrafficLightArrayEvent(proxyAdapter->GetProxy(), events::TrafficLightInterfaceTrafficLightArrayEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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

    TrafficLightInterfaceProxy(const TrafficLightInterfaceProxy&) = delete;
    TrafficLightInterfaceProxy& operator=(const TrafficLightInterfaceProxy&) = delete;

    TrafficLightInterfaceProxy(TrafficLightInterfaceProxy&&) = default;
    TrafficLightInterfaceProxy& operator=(TrafficLightInterfaceProxy&&) = default;
    TrafficLightInterfaceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          TrafficLightArrayEvent(token.GetTrafficLightArrayEvent()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::trafficlight::TrafficLightInterface::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<TrafficLightInterfaceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::trafficlight::TrafficLightInterface::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::trafficlight::TrafficLightInterface::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::trafficlight::TrafficLightInterface::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::trafficlight::TrafficLightInterface::ServiceIdentifier, specifier);
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
    events::TrafficLightArrayEvent TrafficLightArrayEvent;
};
} // namespace proxy
} // namespace trafficlight
} // namespace mdc

#endif // MDC_TRAFFICLIGHT_TRAFFICLIGHTINTERFACE_PROXY_H
