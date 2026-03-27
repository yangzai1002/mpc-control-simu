#ifndef DATAPOOL_INCLUDE_BASE_BEHAVIOR_TYPEDEF_H_
#define DATAPOOL_INCLUDE_BASE_BEHAVIOR_TYPEDEF_H_

#include "xkit/planning/datapool/public_typedef.h"
#include "xkit/planning/datapool/cognition_typedef.h"
#include "xkit/planning/datapool/common_config.h"
#include "xkit/planning/datapool/speedplan_typedef.h"
#include "xkit/planning/toolbox/algorithm_interface/algorithm_base.h"


namespace avos {
namespace planning {

enum class eBehaviorBusyStatus {
  DEFAULT = 0,
  BUSY_STATUS,
  NORMAL_STATUS
};
typedef struct BehaviorBusyStatus
{
  eBehaviorBusyStatus status;
  std::string status_str;
  BehaviorBusyStatus() {
    status = eBehaviorBusyStatus::DEFAULT;
    status_str = "default";
  }

  void SetBusyStatus() {
    status = eBehaviorBusyStatus::BUSY_STATUS;
    status_str = "busy_status";
  }

  void SetNormalStatus() {
    status = eBehaviorBusyStatus::NORMAL_STATUS;
    status_str = "normal_status";
  }
  void Reset() {
    status = eBehaviorBusyStatus::DEFAULT;
    status_str = "default";
  }
} BehaviorBusyStatus;

enum class eBehaviorExecuteStatus {
  DEFAULT = 0,
  EXECUTING ,
  EXE_FAILED,
  EXE_SUCCESS
};

typedef struct BehaviorExecuteStatus {
  eBehaviorExecuteStatus status;
  std::string status_str;
  BehaviorExecuteStatus() {
    status = eBehaviorExecuteStatus::DEFAULT;
    status_str = "Default";
  }
  void Reset() {
    status = eBehaviorExecuteStatus::DEFAULT;
    status_str = "Default";
  }
  void SetExecuting() {
    status = eBehaviorExecuteStatus::EXECUTING;
    status_str = "executing";
  }

  void SetExecutFailed() {
    status = eBehaviorExecuteStatus::EXE_FAILED;
    status_str = "ExecutFailed";
  }

  void SetExecutSuccess() {
    status = eBehaviorExecuteStatus::EXE_SUCCESS;
    status_str = "ExecutSuccess";
  }
} BehaviorExecuteStatus;
}
}

#endif