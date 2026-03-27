
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"

#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_businesscommand.h"



#include "ara/adsfi/impl_type_vehiclelatcontrol.h"
#include "ara/adsfi/impl_type_faultdata.h"


#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

#include "base_control/lat/latcontrol_header.h"
#include "common/time/node_time.h"
#include "custom_stack.h"
#include "controller_node.hpp"



using namespace ara::adsfi;

class AdsfiInterface: public BaseAdsfiInterface
{
public:
	AdsfiInterface() {

	}
	~AdsfiInterface() {

	}

	void Init();

	void DeInit();


	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleInformation> &ptr);
    void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::FaultData> &ptr);

	int Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleActControl> &ptr);

	BASE_TEMPLATE_FUNCION

private:

	ThreadSafeQueue<std::shared_ptr<ara::adsfi::BusinessCommand>> buscommand_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory>> planningresult_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::VehicleInformation>> vehicleinfo_deque;
	ThreadSafeQueue<std::shared_ptr<ara::adsfi::MsgHafLocation>> locationinfo_deque;


	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_buscommand;
	ThreadSafeBuffer<ara::adsfi::MsgHafEgoTrajectory> safebuf_planningresult;
	ThreadSafeBuffer<ara::adsfi::VehicleInformation> safebuf_vehicleinfo;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_locationinfo;
	ThreadSafeBuffer<ara::adsfi::FaultData> safebuf_faultdata;


	std::shared_ptr<control::Controller> Control_ptr;
};
#endif

