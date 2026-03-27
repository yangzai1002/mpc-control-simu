/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024-2026. All rights reserved.
 */

#ifndef MDC_EX_WDG_QA_PROXY_H
#define MDC_EX_WDG_QA_PROXY_H

#include "ara/com/internal/proxy/proxy_adapter.h"
#include "ara/com/internal/proxy/event_adapter.h"
#include "ara/com/internal/proxy/field_adapter.h"
#include "ara/com/internal/proxy/method_adapter.h"
#include "ara/com/crc_verification.h"
#include "mdc/ex_wdg/qa_common.h"
#include <string>

namespace mdc {
namespace ex_wdg {
namespace proxy {
namespace events {
}

namespace fields {
}

namespace methods {
static constexpr ara::com::internal::EntityId QAAnswerId = 62441U; //Answer_method_hash
static constexpr ara::com::internal::EntityId QAQuesionId = 47070U; //Quesion_method_hash


class Answer {
public:
    using Output = mdc::ex_wdg::methods::Answer::Output;

    Answer(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::ex_wdg::AnswerMsgType>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::ex_wdg::AnswerMsgType& answerPara)
    {
        return method_->operator()(answerPara);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::ex_wdg::AnswerMsgType>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::ex_wdg::AnswerMsgType>> method_;
};

class Quesion {
public:
    using Output = mdc::ex_wdg::methods::Quesion::Output;

    Quesion(const std::shared_ptr<rtf::cm::vcc::Proxy>& proxy, const ara::com::internal::EntityId entityId, const rtf::cm::type::ServiceInterfaceInfo& serviceInterfaceInfo)
        : method_(std::make_shared<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::ex_wdg::QuesionMsgType>>(proxy, entityId, serviceInterfaceInfo)) {}

    ara::core::Future<Output> operator()(const ::mdc::ex_wdg::QuesionMsgType& questionPara)
    {
        return method_->operator()(questionPara);
    }

    ara::com::e2e::SMState GetSMState() const noexcept
    {
        return method_->GetSMState();
    }

    /* Do not use such internal interfaces!!! */
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::ex_wdg::QuesionMsgType>> GetMethodAdapter() const noexcept
    {
        return method_;
    }
private:
    std::shared_ptr<ara::com::internal::proxy::method::MethodAdapter<Output, ::mdc::ex_wdg::QuesionMsgType>> method_;
};
} // namespace methods

class QAProxy {
private:
    std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
public:
    using HandleType = ara::com::HandleType;
    class ConstructionToken {
    public:
        explicit ConstructionToken(const std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> & proxy)
            : proxyAdapter(proxy),
              Answer(proxyAdapter->GetProxy(), methods::QAAnswerId, proxyAdapter->GetServiceInterfaceInfo()),
              Quesion(proxyAdapter->GetProxy(), methods::QAQuesionId, proxyAdapter->GetServiceInterfaceInfo()){
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
            initResult = Answer.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            initResult = Quesion.GetMethodAdapter()->Initialize();
            if (!initResult.HasValue()) {
                return initResult;
            }
            return initResult;
        }
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> GetProxyAdapter()
        {
            return proxyAdapter;
        }
        methods::Answer GetAnswer() noexcept { return std::move(Answer); }
        methods::Quesion GetQuesion() noexcept { return std::move(Quesion); }
    private:
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> proxyAdapter;
        methods::Answer Answer;
        methods::Quesion Quesion;
    };

    virtual ~QAProxy()
    {

    }

    explicit QAProxy(const HandleType &handle)
        : proxyAdapter(std::make_shared<ara::com::internal::proxy::ProxyAdapter>(::mdc::ex_wdg::QA::ServiceIdentifier, handle)),
          Answer(proxyAdapter->GetProxy(), methods::QAAnswerId, proxyAdapter->GetServiceInterfaceInfo()),
          Quesion(proxyAdapter->GetProxy(), methods::QAQuesionId, proxyAdapter->GetServiceInterfaceInfo()){
            ara::core::Result<void> resultAnswer = Answer.GetMethodAdapter()->Initialize();
            ThrowError(resultAnswer);
            ara::core::Result<void> resultQuesion = Quesion.GetMethodAdapter()->Initialize();
            ThrowError(resultQuesion);
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

    QAProxy(const QAProxy&) = delete;
    QAProxy& operator=(const QAProxy&) = delete;

    QAProxy(QAProxy&&) = default;
    QAProxy& operator=(QAProxy&&) = default;
    QAProxy(ConstructionToken&& token) noexcept
        : proxyAdapter(token.GetProxyAdapter()),
          Answer(token.GetAnswer()),
          Quesion(token.GetQuesion()){
        static_cast<void>(std::move(token));
    }

    static ara::core::Result<ConstructionToken> Preconstruct(
        const ara::com::HandleType &handle)
    {
        std::shared_ptr<ara::com::internal::proxy::ProxyAdapter> preProxyAdapter =
            std::make_shared<ara::com::internal::proxy::ProxyAdapter>(
               ::mdc::ex_wdg::QA::ServiceIdentifier, handle);
        ConstructionToken token{std::move(preProxyAdapter)};
        ara::core::Result<void> initResult {token.InitializeAll()};
        if (initResult.HasValue()) {
            return ara::core::Result<ConstructionToken>(std::move(token));
        } else {
            const ara::core::ErrorCode errorcode{initResult.Error()};
            ara::core::Result<QAProxy::ConstructionToken> preResult{errorcode};
            return preResult;
        }
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType>& handler,
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::ex_wdg::QA::ServiceIdentifier, instance);
    }

    static ara::com::FindServiceHandle StartFindService(
        const ara::com::FindServiceHandler<ara::com::internal::proxy::ProxyAdapter::HandleType> handler,
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::StartFindService(handler, ::mdc::ex_wdg::QA::ServiceIdentifier, specifier);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::com::InstanceIdentifier instance)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::ex_wdg::QA::ServiceIdentifier, instance);
    }

    static ara::com::ServiceHandleContainer<ara::com::internal::proxy::ProxyAdapter::HandleType> FindService(
        const ara::core::InstanceSpecifier specifier)
    {
        return ara::com::internal::proxy::ProxyAdapter::FindService(::mdc::ex_wdg::QA::ServiceIdentifier, specifier);
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
    methods::Answer Answer;
    methods::Quesion Quesion;
};
} // namespace proxy
} // namespace ex_wdg
} // namespace mdc

#endif // MDC_EX_WDG_QA_PROXY_H
