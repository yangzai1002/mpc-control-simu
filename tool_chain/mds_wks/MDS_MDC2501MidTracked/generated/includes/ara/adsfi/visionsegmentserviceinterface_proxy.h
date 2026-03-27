/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACE_PROXY_H
#define ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "ara/adsfi/visionsegmentserviceinterface_common.h"
#include <string>

namespace ara {
namespace adsfi {
namespace proxy {
namespace events {
    using EventVisionSegment = ara::com::internal::proxy::event::EventAdapter<::ara::adsfi::MsgCameraVPResult>;
    static constexpr ara::com::internal::EntityId VisionSegmentServiceInterfaceEventVisionSegmentId = 24805U; //EventVisionSegment_event_hash
}

namespace fields {
}

namespace methods {

} // namespace methods

class VisionSegmentServiceInterfaceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              EventVisionSegment(proxyAdapter->GetProxy(), events::VisionSegmentServiceInterfaceEventVisionSegmentId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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
        events::EventVisionSegment GetEventVisionSegment() noexcept { return std::move(EventVisionSegment); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::EventVisionSegment EventVisionSegment;
    };

    virtual ~VisionSegmentServiceInterfaceProxy()
    {
        EventVisionSegment.UnsetReceiveHandler();
        EventVisionSegment.Unsubscribe();

    }

    explicit VisionSegmentServiceInterfaceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::ara::adsfi::VisionSegmentServiceInterface::ServiceIdentifier, handle)),
          EventVisionSegment(proxyAdapter->GetProxy(), events::VisionSegmentServiceInterfaceEventVisionSegmentId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()){
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

    VisionSegmentServiceInterfaceProxy(const VisionSegmentServiceInterfaceProxy&) = delete;
    VisionSegmentServiceInterfaceProxy& operator=(const VisionSegmentServiceInterfaceProxy&) = delete;

    VisionSegmentServiceInterfaceProxy(VisionSegmentServiceInterfaceProxy&&) = default;
    VisionSegmentServiceInterfaceProxy& operator=(VisionSegmentServiceInterfaceProxy&&) = default;
    VisionSegmentServiceInterfaceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          EventVisionSegment(token.GetEventVisionSegment()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::ara::adsfi::VisionSegmentServiceInterface::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<VisionSegmentServiceInterfaceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::ara::adsfi::VisionSegmentServiceInterface::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::ara::adsfi::VisionSegmentServiceInterface::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::ara::adsfi::VisionSegmentServiceInterface::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::ara::adsfi::VisionSegmentServiceInterface::ServiceIdentifier, specifier);
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
    events::EventVisionSegment EventVisionSegment;
};
} // namespace proxy
} // namespace adsfi
} // namespace ara

#endif // ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACE_PROXY_H
