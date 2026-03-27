/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_DEVM_DEVMC_DEVMCENTERSERVICEINTERFACE_PROXY_H
#define MDC_DEVM_DEVMC_DEVMCENTERSERVICEINTERFACE_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/devm/devmc/devmcenterserviceinterface_common.h"
#include <string>

namespace mdc {
namespace devm {
namespace devmc {
namespace proxy {
namespace events {
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId DevmCenterServiceInterfaceMsgPipeQueryId = 29020U; //MsgPipeQuery_method_hash
static constexpr ara::com::internal::EntityId DevmCenterServiceInterfaceMsgPipeCtrlId = 17506U; //MsgPipeCtrl_method_hash
static constexpr ara::com::internal::EntityId DevmCenterServiceInterfaceMsgPipeUpdateId = 37011U; //MsgPipeUpdate_method_hash
static constexpr ara::com::internal::EntityId DevmCenterServiceInterfaceMsgPipe1Id = 4161U; //MsgPipe1_method_hash
static constexpr ara::com::internal::EntityId DevmCenterServiceInterfaceMsgPipe2Id = 4744U; //MsgPipe2_method_hash
static constexpr ara::com::internal::EntityId DevmCenterServiceInterfaceMsgPipe3Id = 60620U; //MsgPipe3_method_hash


class MsgPipeQuery {
public:
    using Output = mdc::devm::devmc::methods::MsgPipeQuery::Output;

    MsgPipeQuery(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::devm::Uint8List& in)
    {
        return method_->operator()(in);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> method_;
};

class MsgPipeCtrl {
public:
    using Output = mdc::devm::devmc::methods::MsgPipeCtrl::Output;

    MsgPipeCtrl(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::devm::Uint8List& in)
    {
        return method_->operator()(in);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> method_;
};

class MsgPipeUpdate {
public:
    using Output = mdc::devm::devmc::methods::MsgPipeUpdate::Output;

    MsgPipeUpdate(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::devm::Uint8List& in)
    {
        return method_->operator()(in);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> method_;
};

class MsgPipe1 {
public:
    using Output = mdc::devm::devmc::methods::MsgPipe1::Output;

    MsgPipe1(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::devm::Uint8List& in)
    {
        return method_->operator()(in);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> method_;
};

class MsgPipe2 {
public:
    using Output = mdc::devm::devmc::methods::MsgPipe2::Output;

    MsgPipe2(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::devm::Uint8List& in)
    {
        return method_->operator()(in);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> method_;
};

class MsgPipe3 {
public:
    using Output = mdc::devm::devmc::methods::MsgPipe3::Output;

    MsgPipe3(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::devm::Uint8List& in)
    {
        return method_->operator()(in);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::devm::Uint8List>> method_;
};
} // namespace methods

class DevmCenterServiceInterfaceProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              MsgPipeQuery(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipeQueryId, proxyAdapter->GetServiceInterfaceInfo()),
              MsgPipeCtrl(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipeCtrlId, proxyAdapter->GetServiceInterfaceInfo()),
              MsgPipeUpdate(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipeUpdateId, proxyAdapter->GetServiceInterfaceInfo()),
              MsgPipe1(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipe1Id, proxyAdapter->GetServiceInterfaceInfo()),
              MsgPipe2(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipe2Id, proxyAdapter->GetServiceInterfaceInfo()),
              MsgPipe3(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipe3Id, proxyAdapter->GetServiceInterfaceInfo()){
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
            initResult = MsgPipeQuery.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = MsgPipeCtrl.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = MsgPipeUpdate.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = MsgPipe1.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = MsgPipe2.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = MsgPipe3.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        methods::MsgPipeQuery GetMsgPipeQuery() noexcept { return std::move(MsgPipeQuery); }
        methods::MsgPipeCtrl GetMsgPipeCtrl() noexcept { return std::move(MsgPipeCtrl); }
        methods::MsgPipeUpdate GetMsgPipeUpdate() noexcept { return std::move(MsgPipeUpdate); }
        methods::MsgPipe1 GetMsgPipe1() noexcept { return std::move(MsgPipe1); }
        methods::MsgPipe2 GetMsgPipe2() noexcept { return std::move(MsgPipe2); }
        methods::MsgPipe3 GetMsgPipe3() noexcept { return std::move(MsgPipe3); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        methods::MsgPipeQuery MsgPipeQuery;
        methods::MsgPipeCtrl MsgPipeCtrl;
        methods::MsgPipeUpdate MsgPipeUpdate;
        methods::MsgPipe1 MsgPipe1;
        methods::MsgPipe2 MsgPipe2;
        methods::MsgPipe3 MsgPipe3;
    };

    virtual ~DevmCenterServiceInterfaceProxy()
    {

    }

    explicit DevmCenterServiceInterfaceProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::devm::devmc::DevmCenterServiceInterface::ServiceIdentifier, handle)),
          MsgPipeQuery(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipeQueryId, proxyAdapter->GetServiceInterfaceInfo()),
          MsgPipeCtrl(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipeCtrlId, proxyAdapter->GetServiceInterfaceInfo()),
          MsgPipeUpdate(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipeUpdateId, proxyAdapter->GetServiceInterfaceInfo()),
          MsgPipe1(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipe1Id, proxyAdapter->GetServiceInterfaceInfo()),
          MsgPipe2(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipe2Id, proxyAdapter->GetServiceInterfaceInfo()),
          MsgPipe3(proxyAdapter->GetProxy(), methods::DevmCenterServiceInterfaceMsgPipe3Id, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultMsgPipeQuery = MsgPipeQuery.GetMethodAdapter()->Initialize();
            ThrowError(resultMsgPipeQuery);
            ara::core::Result<void> resultMsgPipeCtrl = MsgPipeCtrl.GetMethodAdapter()->Initialize();
            ThrowError(resultMsgPipeCtrl);
            ara::core::Result<void> resultMsgPipeUpdate = MsgPipeUpdate.GetMethodAdapter()->Initialize();
            ThrowError(resultMsgPipeUpdate);
            ara::core::Result<void> resultMsgPipe1 = MsgPipe1.GetMethodAdapter()->Initialize();
            ThrowError(resultMsgPipe1);
            ara::core::Result<void> resultMsgPipe2 = MsgPipe2.GetMethodAdapter()->Initialize();
            ThrowError(resultMsgPipe2);
            ara::core::Result<void> resultMsgPipe3 = MsgPipe3.GetMethodAdapter()->Initialize();
            ThrowError(resultMsgPipe3);
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

    DevmCenterServiceInterfaceProxy(const DevmCenterServiceInterfaceProxy&) = delete;
    DevmCenterServiceInterfaceProxy& operator=(const DevmCenterServiceInterfaceProxy&) = delete;

    DevmCenterServiceInterfaceProxy(DevmCenterServiceInterfaceProxy&&) = default;
    DevmCenterServiceInterfaceProxy& operator=(DevmCenterServiceInterfaceProxy&&) = default;
    DevmCenterServiceInterfaceProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          MsgPipeQuery(token.GetMsgPipeQuery()),
          MsgPipeCtrl(token.GetMsgPipeCtrl()),
          MsgPipeUpdate(token.GetMsgPipeUpdate()),
          MsgPipe1(token.GetMsgPipe1()),
          MsgPipe2(token.GetMsgPipe2()),
          MsgPipe3(token.GetMsgPipe3()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::devm::devmc::DevmCenterServiceInterface::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<DevmCenterServiceInterfaceProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::devm::devmc::DevmCenterServiceInterface::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::devm::devmc::DevmCenterServiceInterface::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::devm::devmc::DevmCenterServiceInterface::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::devm::devmc::DevmCenterServiceInterface::ServiceIdentifier, specifier);
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
    methods::MsgPipeQuery MsgPipeQuery;
    methods::MsgPipeCtrl MsgPipeCtrl;
    methods::MsgPipeUpdate MsgPipeUpdate;
    methods::MsgPipe1 MsgPipe1;
    methods::MsgPipe2 MsgPipe2;
    methods::MsgPipe3 MsgPipe3;
};
} // namespace proxy
} // namespace devmc
} // namespace devm
} // namespace mdc

#endif // MDC_DEVM_DEVMC_DEVMCENTERSERVICEINTERFACE_PROXY_H
