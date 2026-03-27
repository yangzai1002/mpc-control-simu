
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_businesscommand.h"

#include <string>
#include <memory>
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"

#include "common/time/node_time.h"

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

	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &ptr);
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::BusinessCommand> &ptr);

	int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &planning_result_ptr);

	BASE_TEMPLATE_FUNCION

private:

	ThreadSafeBuffer<ara::adsfi::MsgHafEgoTrajectory> safebuf_pathplan_adjust_;
	ThreadSafeBuffer<ara::adsfi::MsgHafEgoTrajectory> safebuf_pathplan_arrive_;
	ThreadSafeBuffer<ara::adsfi::MsgHafEgoTrajectory> safebuf_pathplan_follow_;
	ThreadSafeBuffer<ara::adsfi::MsgHafEgoTrajectory> safebuf_pathplan_track_;
	ThreadSafeBuffer<ara::adsfi::BusinessCommand> safebuf_bus_command_;

	// 当前业务模式
	int32_t current_business_mode_ = 0;

	// 创建空轨迹
	void CreateEmptyTrajectory(std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> &out);

};
#endif
