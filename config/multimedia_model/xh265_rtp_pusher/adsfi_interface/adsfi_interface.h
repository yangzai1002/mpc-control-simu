
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <list>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include "DatetimeUtil.hpp"
#include "config_loader/custom_stack.h"
#include "log/log.h"




#include "ara/adsfi/impl_type_idpvideortpparameter.h"
#include "ara/adsfi/impl_type_idpvideoencodedframe.h"
#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"


#include "Ec407.hpp"
#include "LocalH265RTPSender.hpp"

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init() override;



	void Callback(const std::shared_ptr<ara::adsfi::IdpVideoRTPParameter> &ptr) ;


	void Callback(const std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame> &ptr) ;






private:
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::IdpVideoRTPParameter>> param_deque;
	std::shared_ptr<Ec407> _ec;
	std::shared_ptr<std::atomic<double>> _last_data_time;
	std::shared_ptr<LocalH265RTPSender> _sender_rtp;
	std::shared_ptr<ara::adsfi::IdpVideoRTPParameter> _last_para;

	std::shared_ptr<ara::adsfi::IdpVideoRTPParameter> para_ptr;
public:
	BASE_TEMPLATE_FUNCION
};
#endif








