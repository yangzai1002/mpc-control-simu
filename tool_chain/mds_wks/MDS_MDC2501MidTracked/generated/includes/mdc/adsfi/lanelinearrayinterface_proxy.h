/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_ADSFI_LANELINEARRAYINTERFACE_PROXY_H
#define MDC_ADSFI_LANELINEARRAYINTERFACE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/adsfi/lanelinearrayinterface_common.h"
#include <string>

namespace mdc {
namespace adsfi {
namespace proxy {
namespace events {
    using LaneLineArrayEvent = ara::com::internal::proxy::event::EventAdapter<::ara::adsfi::LaneLineArray>;
    static constexpr ara::com::internal::EntityId LaneLineArrayInterfaceLaneLineArrayEventId = 52873U; //LaneLineArrayEvent_event_hash
}

namespace fields {
}

namespace methods {

} // namespace methods

class LaneLineArrayInterfaceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              LaneLineArrayEvent(proxyAdapter->GetProxy(), events::LaneLineArrayInterfaceLaneLineArrayEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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
        events::LaneLineArrayEvent GetLaneLineArrayEvent() noexcept { return std::move(LaneLineArrayEvent); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::LaneLineArrayEvent LaneLineArrayEvent;
    };

    virtual ~LaneLineArrayInterfaceProxy()
    {
        LaneLineArrayEvent.UnsetReceiveHandler();
        LaneLineArrayEvent.Unsubscribe();

    }

    explicit LaneLineArrayInterfaceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::adsfi::LaneLineArrayInterface::ServiceIdentifier, handle)),
          LaneLineArrayEvent(proxyAdapter->GetProxy(), events::LaneLineArrayInterfaceLaneLineArrayEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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

    LaneLineArrayInterfaceProxy(const LaneLineArrayInterfaceProxy&) = delete;
    LaneLineArrayInterfaceProxy& operator=(const LaneLineArrayInterfaceProxy&) = delete;

    LaneLineArrayInterfaceProxy(LaneLineArrayInterfaceProxy&&) = default;
    LaneLineArrayInterfaceProxy& operator=(LaneLineArrayInterfaceProxy&&) = default;
    LaneLineArrayInterfaceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          LaneLineArrayEvent(token.GetLaneLineArrayEvent()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::adsfi::LaneLineArrayInterface::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<LaneLineArrayInterfaceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::adsfi::LaneLineArrayInterface::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::adsfi::LaneLineArrayInterface::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::adsfi::LaneLineArrayInterface::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::adsfi::LaneLineArrayInterface::ServiceIdentifier, specifier);
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
    events::LaneLineArrayEvent LaneLineArrayEvent;
};
} // namespace proxy
} // namespace adsfi
} // namespace mdc

#endif // MDC_ADSFI_LANELINEARRAYINTERFACE_PROXY_H
