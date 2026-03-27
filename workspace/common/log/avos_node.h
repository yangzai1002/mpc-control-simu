/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * History:
 * lbh          2019/03/28    1.0.0        build
 *****************************************************************************/

#ifndef AVOS_COMMON_INIT_H_
#define AVOS_COMMON_INIT_H_

#include <iostream>
#include "log.h"

namespace avos {
namespace common {

class AvosNode {
public:
  static bool Init(const std::string node_name, bool Enable_GLOG_Screen = true,
                   const std::string info_log_file_path = "",
                   const std::string warning_log_file_path = "",
                   const std::string error_log_file_path = "") {
    SetNodeName(node_name);
    google::InitGoogleLogging(node_name.c_str());
    google::SetLogDestination(google::INFO, info_log_file_path.c_str());
    google::SetLogDestination(google::WARNING, warning_log_file_path.c_str());
    google::SetLogDestination(google::ERROR, error_log_file_path.c_str());
    FLAGS_colorlogtostderr = true;
    FLAGS_minloglevel = google::GLOG_INFO;
    FLAGS_stderrthreshold = 3;
    FLAGS_alsologtostderr =  Enable_GLOG_Screen;
    return true;
  }
  static std::string GetNodeName() { return GetNameRef(); }
  static void SetNodeName(const std::string& name) { GetNameRef() = name; }
private:
  static std::string& GetNameRef() {
    static std::string node_name;
    return node_name;
  }
};
}  // namespace avos
}  // namespace common

#endif  // AVOS_COMMON_INIT_H_
