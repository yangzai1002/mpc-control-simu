#ifndef _LOG_UTILS_H_
#define _LOG_UTILS_H_

#include <iostream>
#include <string>

#include "glog/logging.h"
#include "glog/raw_logging.h"

#define LOG_INFO google::LogMessage(__FILE__, __LINE__, google::INFO).stream()
#define LOG_ERROR google::LogMessage(__FILE__, __LINE__, google::ERROR).stream()
#define LOG_WARN google::LogMessage(__FILE__, __LINE__, google::WARNING).stream()

#endif