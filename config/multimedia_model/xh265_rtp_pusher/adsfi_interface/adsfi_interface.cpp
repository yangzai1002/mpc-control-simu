
#include "adsfi_interface.h"
#include "log/avos_node.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void AdsfiInterface::Init() {
	SetScheduleInfo("message");

	avos::common::AvosNode::Init("xh265_rtp_pusher");

	para_ptr = std::make_shared<ara::adsfi::IdpVideoRTPParameter>();
	para_ptr->push_enabled = 1;
	para_ptr->ssrc = 0;
	para_ptr->peer_port = 8555;
	// 1. 修改：指定合法的组播IP地址（224.0.0.0 ~ 239.255.255.255）
	para_ptr->peer_host = "224.0.0.88"; // 示例组播IP，可根据需求调整
	_last_para = std::make_shared<ara::adsfi::IdpVideoRTPParameter>();
	_last_para->push_enabled = 1;
	_last_para->ssrc = 0;
	_last_para->peer_port = 8555;
	_last_para->peer_host = "224.0.0.88"; // 与上面保持一致
	_ec = std::make_shared<Ec407>();
	_last_data_time = std::make_shared<std::atomic<double>>(0.0);
	if (nullptr == _ec || nullptr == _last_data_time) {
		AERROR << "create ec failed";
		return;
	}
	std::thread([&]() {
		while (true) {
			auto now = app_common::DatetimeUtil::current_second_double();
			auto last = _last_data_time->load();
			if (fabs(now - last) >= 2.0) {
				_ec->ec_add(_ERRORCODE_NO_DATA);
			} else {
				_ec->ec_remove(_ERRORCODE_NO_DATA);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}).detach();
}


void AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpVideoRTPParameter> &ptr) {
	std::cout << "lAdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpVideoRTPParameter>" << std::endl;
	param_deque.push(ptr);
}


void AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame> &ptr) {
	std::cout << "AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame>"  << std::endl;

	// param_deque.tryPop(para_ptr);
	
	std::cout << "AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame>1"  << std::endl;
	_ec->check_freq(ptr->header.time, ptr->fps);
	_last_data_time->store(ptr->header.time);
	std::cout << "AdsfiInterface::Callback(const std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame>2"  << std::endl;
	if (0 == para_ptr->push_enabled) {
		_sender_rtp = nullptr;
		return;
	} else if (_last_para->ssrc != para_ptr->ssrc || _last_para->peer_port != para_ptr->peer_port ||
	           _last_para->peer_host != para_ptr->peer_host) {
		AINFO << "old RTP param, " << _last_para->ssrc << ", " << _last_para->peer_port << ", " << _last_para->peer_host;
		AINFO << "new RTP param, " << para_ptr->ssrc << ", " << para_ptr->peer_port << ", " << para_ptr->peer_host;
		_sender_rtp = nullptr;
	}
	_last_para = para_ptr;

	if (_sender_rtp == nullptr) {
		addrinfo  hints;
		addrinfo* res = nullptr;
		int       status;
		char      ipstr[INET6_ADDRSTRLEN];

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;     // 支持 IPv4 和 IPv6
		// 2. 关键修改：组播基于UDP传输，将 SOCK_STREAM（TCP）改为 SOCK_DGRAM（UDP）
		hints.ai_socktype = SOCK_DGRAM;  // UDP（组播必须使用UDP，不支持TCP）
		hints.ai_protocol = IPPROTO_UDP; // 明确指定UDP协议

		if ((status = getaddrinfo(para_ptr->peer_host.c_str(), nullptr, &hints, &res)) != 0) {
			AERROR << "getaddrinfo error: " << gai_strerror(status);
			_ec->ec_add(_ERRORCODE_ADDRESS_RESOLVE, para_ptr->peer_host);
			return;
		}
		_ec->ec_remove(_ERRORCODE_ADDRESS_RESOLVE);

		std::list<std::string> ips;
		for (auto p = res; p != nullptr; p = p->ai_next) {
			void* addr;
			char* ipver;

			if (p->ai_family != AF_INET) {
				continue;
			}
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			memset(ipstr, 0, sizeof(ipstr));
			inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
			ips.emplace_back(ipstr);
		}

		AINFO << "ipv4 addresses of " << para_ptr->peer_host << ": ";
		for (const auto& ip : ips) {
			AINFO << ip;
		}
		if (ips.empty()) {
			AERROR << "no valid ip address found";
			_ec->ec_add(_ERRORCODE_ADDRESS_RESOLVE, para_ptr->peer_host);
			return;
		}
		_ec->ec_remove(_ERRORCODE_ADDRESS_RESOLVE);
		auto& ip = ips.front();
		AINFO << "sending to " << ip << ":" << para_ptr->peer_port;
		try {
			_sender_rtp =
			    std::make_shared<LocalH265RTPSender>(para_ptr->ssrc, ptr->fps, "0.0.0.0", ip, para_ptr->peer_port);
		} catch (const std::exception& e) {
			AERROR << "ERROR: " << e.what();
			return;
		}
	} else {
		try {
			_sender_rtp->send(ptr->frame_data.data(), ptr->frame_data.size());
			AINFO << "sent " << ptr->frame_data.size() << " bytes";
			AINFO << std::setprecision(15) << Tools::Second() << " 获取的时间: " << ptr->header.time;
			_ec->ec_remove(_ERRORCODE_FORMAT);
			_ec->ec_remove(_ERRORCODE_SEND);
		} catch (const ExceptionParseNalus& e) {
			_ec->ec_add(_ERRORCODE_FORMAT);
		} catch (const std::exception& e) {
			AERROR << "error: " << e.what();
			_ec->ec_add(_ERRORCODE_SEND);
			_sender_rtp.reset();
		}
	}


}


