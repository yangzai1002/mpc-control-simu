#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msglidarframe.h"
#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"

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

	// MsgHafLocation 回调: /tpimu, /tpododr, /tpliodr
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &ptr);

	// MsgLidarFrame 回调: /tpfront_middle_lidar_points, /tpfront_left_lidar_points, /tpfront_right_lidar_points
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgLidarFrame> &ptr);

	// MsgHafIMU 回调: /tpsensorimu
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafIMU> &ptr);

	// MsgHafWheelSpeedList 回调: /tpwheelspeed
	void Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &ptr);

	// 输出: /tplocalization
	int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &loc_ptr);

	BASE_TEMPLATE_FUNCION

private:

	// MsgHafLocation 缓存
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_tpimu_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_ododr_;
	ThreadSafeBuffer<ara::adsfi::MsgHafLocation> safebuf_liodr_;

	// MsgLidarFrame 缓存
	ThreadSafeBuffer<ara::adsfi::MsgLidarFrame> safebuf_lidar_front_middle_;
	ThreadSafeBuffer<ara::adsfi::MsgLidarFrame> safebuf_lidar_front_left_;
	ThreadSafeBuffer<ara::adsfi::MsgLidarFrame> safebuf_lidar_front_right_;

	// MsgHafIMU 缓存
	ThreadSafeBuffer<ara::adsfi::MsgHafIMU> safebuf_sensorimu_;

	// MsgHafWheelSpeedList 缓存
	ThreadSafeBuffer<ara::adsfi::MsgHafWheelSpeedList> safebuf_wheelspeed_;

	// 创建空定位数据
	void CreateEmptyLocation(std::shared_ptr<ara::adsfi::MsgHafLocation> &out);

};
#endif
