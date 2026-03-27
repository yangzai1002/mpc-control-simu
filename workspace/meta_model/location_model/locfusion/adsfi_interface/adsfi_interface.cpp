#include "adsfi_interface.h"

void AdsfiInterface::Init() {
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<adsfi::LocationInfo> &msg) {
    loc_deque.push(msg);
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<adsfi::LocationInfo> &msg) {
    std::shared_ptr<adsfi::LocationInfo> ptr;
    loc_deque.waitAndPop(ptr);
    *msg = *ptr;
    return 0;
}
