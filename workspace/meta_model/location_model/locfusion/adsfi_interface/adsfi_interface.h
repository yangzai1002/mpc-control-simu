
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__
#include <memory>
#include <string>

#include "adsfi/impl_type_locationinfo.h"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
    }
    ~AdsfiInterface() {
    }

    void Init();

    void Callback(const std::string &name, const std::shared_ptr<adsfi::LocationInfo> &msg);

    int Process(const std::string &name, std::shared_ptr<adsfi::LocationInfo> &msg);

private:
    ThreadSafeQueue<std::shared_ptr<adsfi::LocationInfo>> loc_deque;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
