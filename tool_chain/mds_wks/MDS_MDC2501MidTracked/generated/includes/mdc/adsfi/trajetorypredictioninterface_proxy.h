/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_ADSFI_TRAJETORYPREDICTIONINTERFACE_PROXY_H
#define MDC_ADSFI_TRAJETORYPREDICTIONINTERFACE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/adsfi/trajetorypredictioninterface_common.h"
#include <string>

namespace mdc {
namespace adsfi {
namespace proxy {
namespace events {
    using TrajetoryPredictionEvent = ara::com::internal::proxy::event::EventAdapter<::ara::adsfi::ObjPredictionOutArray>;
    static constexpr ara::com::internal::EntityId TrajetoryPredictionInterfaceTrajetoryPredictionEventId = 43381U; //TrajetoryPredictionEvent_event_hash
}

namespace fields {
}

namespace methods {

} // namespace methods

class TrajetoryPredictionInterfaceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              TrajetoryPredictionEvent(proxyAdapter->GetProxy(), events::TrajetoryPredictionInterfaceTrajetoryPredictionEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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
        events::TrajetoryPredictionEvent GetTrajetoryPredictionEvent() noexcept { return std::move(TrajetoryPredictionEvent); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::TrajetoryPredictionEvent TrajetoryPredictionEvent;
    };

    virtual ~TrajetoryPredictionInterfaceProxy()
    {
        TrajetoryPredictionEvent.UnsetReceiveHandler();
        TrajetoryPredictionEvent.Unsubscribe();

    }

    explicit TrajetoryPredictionInterfaceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::adsfi::TrajetoryPredictionInterface::ServiceIdentifier, handle)),
          TrajetoryPredictionEvent(proxyAdapter->GetProxy(), events::TrajetoryPredictionInterfaceTrajetoryPredictionEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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

    TrajetoryPredictionInterfaceProxy(const TrajetoryPredictionInterfaceProxy&) = delete;
    TrajetoryPredictionInterfaceProxy& operator=(const TrajetoryPredictionInterfaceProxy&) = delete;

    TrajetoryPredictionInterfaceProxy(TrajetoryPredictionInterfaceProxy&&) = default;
    TrajetoryPredictionInterfaceProxy& operator=(TrajetoryPredictionInterfaceProxy&&) = default;
    TrajetoryPredictionInterfaceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          TrajetoryPredictionEvent(token.GetTrajetoryPredictionEvent()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::adsfi::TrajetoryPredictionInterface::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<TrajetoryPredictionInterfaceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::adsfi::TrajetoryPredictionInterface::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::adsfi::TrajetoryPredictionInterface::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::adsfi::TrajetoryPredictionInterface::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::adsfi::TrajetoryPredictionInterface::ServiceIdentifier, specifier);
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
    events::TrajetoryPredictionEvent TrajetoryPredictionEvent;
};
} // namespace proxy
} // namespace adsfi
} // namespace mdc

#endif // MDC_ADSFI_TRAJETORYPREDICTIONINTERFACE_PROXY_H
