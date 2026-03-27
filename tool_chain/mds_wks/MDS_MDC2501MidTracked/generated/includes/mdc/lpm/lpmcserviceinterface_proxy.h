/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_LPM_LPMCSERVICEINTERFACE_PROXY_H
#define MDC_LPM_LPMCSERVICEINTERFACE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/lpm/lpmcserviceinterface_common.h"
#include <string>

namespace mdc {
namespace lpm {
namespace proxy {
namespace events {
    using MdcWakeupEvent = ara::com::internal::proxy::event::EventAdapter<::mdc::lpm::MdcWakeupEventData>;
    static constexpr ara::com::internal::EntityId LpmcServiceInterfaceMdcWakeupEventId = 53522U; //MdcWakeupEvent_event_hash
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId LpmcServiceInterfaceSetLowPowerModeId = 51001U; //SetLowPowerMode_method_hash
static constexpr ara::com::internal::EntityId LpmcServiceInterfaceSetEnterDeepSleepTimeId = 33809U; //SetEnterDeepSleepTime_method_hash


class SetLowPowerMode {
public:
    using Output = mdc::lpm::methods::SetLowPowerMode::Output;

    SetLowPowerMode(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
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

class SetEnterDeepSleepTime {
public:
    using Output = mdc::lpm::methods::SetEnterDeepSleepTime::Output;

    SetEnterDeepSleepTime(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::uint32_t>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::uint32_t& enterDeepSleepTime)
    {
        return method_->operator()(enterDeepSleepTime);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::uint32_t>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::uint32_t>> method_;
};
} // namespace methods

class LpmcServiceInterfaceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              MdcWakeupEvent(proxyAdapter->GetProxy(), events::LpmcServiceInterfaceMdcWakeupEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()),
              SetLowPowerMode(proxyAdapter->GetProxy(), methods::LpmcServiceInterfaceSetLowPowerModeId, proxyAdapter->GetServiceInterfaceInfo()),
              SetEnterDeepSleepTime(proxyAdapter->GetProxy(), methods::LpmcServiceInterfaceSetEnterDeepSleepTimeId, proxyAdapter->GetServiceInterfaceInfo()){
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
            initResult = SetLowPowerMode.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = SetEnterDeepSleepTime.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        events::MdcWakeupEvent GetMdcWakeupEvent() noexcept { return std::move(MdcWakeupEvent); }
        methods::SetLowPowerMode GetSetLowPowerMode() noexcept { return std::move(SetLowPowerMode); }
        methods::SetEnterDeepSleepTime GetSetEnterDeepSleepTime() noexcept { return std::move(SetEnterDeepSleepTime); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        events::MdcWakeupEvent MdcWakeupEvent;
        methods::SetLowPowerMode SetLowPowerMode;
        methods::SetEnterDeepSleepTime SetEnterDeepSleepTime;
    };

    virtual ~LpmcServiceInterfaceProxy()
    {
        MdcWakeupEvent.UnsetReceiveHandler();
        MdcWakeupEvent.Unsubscribe();

    }

    explicit LpmcServiceInterfaceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::lpm::LpmcServiceInterface::ServiceIdentifier, handle)),
          MdcWakeupEvent(proxyAdapter->GetProxy(), events::LpmcServiceInterfaceMdcWakeupEventId, proxyAdapter->GetHandle(), proxyAdapter->GetServiceInterfaceInfo()),
          SetLowPowerMode(proxyAdapter->GetProxy(), methods::LpmcServiceInterfaceSetLowPowerModeId, proxyAdapter->GetServiceInterfaceInfo()),
          SetEnterDeepSleepTime(proxyAdapter->GetProxy(), methods::LpmcServiceInterfaceSetEnterDeepSleepTimeId, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultSetLowPowerMode = SetLowPowerMode.GetMethodAdapter()->Initialize();
            ThrowError(resultSetLowPowerMode);
            ara::core::Result<void> resultSetEnterDeepSleepTime = SetEnterDeepSleepTime.GetMethodAdapter()->Initialize();
            ThrowError(resultSetEnterDeepSleepTime);
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

    LpmcServiceInterfaceProxy(const LpmcServiceInterfaceProxy&) = delete;
    LpmcServiceInterfaceProxy& operator=(const LpmcServiceInterfaceProxy&) = delete;

    LpmcServiceInterfaceProxy(LpmcServiceInterfaceProxy&&) = default;
    LpmcServiceInterfaceProxy& operator=(LpmcServiceInterfaceProxy&&) = default;
    LpmcServiceInterfaceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          MdcWakeupEvent(token.GetMdcWakeupEvent()),
          SetLowPowerMode(token.GetSetLowPowerMode()),
          SetEnterDeepSleepTime(token.GetSetEnterDeepSleepTime()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::lpm::LpmcServiceInterface::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<LpmcServiceInterfaceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::lpm::LpmcServiceInterface::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::lpm::LpmcServiceInterface::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::lpm::LpmcServiceInterface::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::lpm::LpmcServiceInterface::ServiceIdentifier, specifier);
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
    events::MdcWakeupEvent MdcWakeupEvent;
    methods::SetLowPowerMode SetLowPowerMode;
    methods::SetEnterDeepSleepTime SetEnterDeepSleepTime;
};
} // namespace proxy
} // namespace lpm
} // namespace mdc

#endif // MDC_LPM_LPMCSERVICEINTERFACE_PROXY_H
