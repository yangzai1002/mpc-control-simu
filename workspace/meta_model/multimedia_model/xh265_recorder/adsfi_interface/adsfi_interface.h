
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <arpa/inet.h>
#include <dlfcn.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "H265ThreadedRotatingFileReceiver.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "ara/adsfi/impl_type_msgimagedata.h"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface() {
    }

    ~AdsfiInterface() {
    }

    void Init() override;

    void Callback(const std::string& name, const std::shared_ptr<ara::adsfi::MsgImageData>& ptr);

private:
    std::shared_ptr<H265ThreadedRotatingFileReceiver> _receiver;
};

#endif
