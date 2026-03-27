
#pragma once
#ifndef Protocol_H_
#define Protocol_H_
#include <nlohmann/json.hpp>
#include <typeinfo>

#include "Auto2Manager.h"
#include "FrameCommon.h"
#include "Manager2Auto.h"
#include "../Manager2Chassis.h"

// #define TEST_CLASS protocol_common::Auto2Load
namespace protocol_common {
template <typename PRO_STRUCT>
void showProtocolStructContent(PRO_STRUCT protocol_struct) {
  nlohmann::json protocol_json;
  to_json(protocol_json, protocol_struct);
  // std::cout << "协议内容: \n" << protocol_json.dump(4);
}

};  // namespace protocol_common

#define TEST_CLASS_RECV protocol_common::Load2Auto

#define TEST_CLASS protocol_common::Auto2Load

#endif
