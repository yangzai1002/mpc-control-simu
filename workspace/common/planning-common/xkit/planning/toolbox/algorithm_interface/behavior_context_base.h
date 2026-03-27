#ifndef SRC_EXECUTION_BEHAVIORPLAN_BEHAVIORBASE_BEHAVIOR_CONTEXT_BASE_H_
#define SRC_EXECUTION_BEHAVIORPLAN_BEHAVIORBASE_BEHAVIOR_CONTEXT_BASE_H_
#include <string>


namespace avos {
namespace planning {

enum class eBehaviorContextType {
  DEFAULT = 0,
  DUMMY_BEHAVIOR_CONTEXT, //空行为
  ARRIVING_BEHAVIOR,      //自主通行
  FOLLOWING_BEHAVIOR,     //跟随
  TRACKING_BEHAVIOR       //循迹
};

typedef struct BehaviorContextType
{
  eBehaviorContextType type;
  std::string type_str;
  BehaviorContextType() {
    type = eBehaviorContextType::DEFAULT;
    type_str = "defualt";
  }
  void SetDummyBehaviorContext() {
    type = eBehaviorContextType::DUMMY_BEHAVIOR_CONTEXT;
    type_str = "dummy Behavior Context";
  }

  void SetFollowingBehavior() {
    type = eBehaviorContextType::FOLLOWING_BEHAVIOR;
    type_str = "Following Behavior Context";
  }

  void SetTrackingBehavior() {
    type = eBehaviorContextType::TRACKING_BEHAVIOR;
    type_str = "Tracking Behavior Context";
  }

  void SetArrivingBehavior() {
    type = eBehaviorContextType::ARRIVING_BEHAVIOR;
    type_str = "Arriving Behavior Context";
  }

} BehaviorContextType;


class BehaviorContextBase
{
public:
  BehaviorContextBase() {}
  virtual ~BehaviorContextBase() {};

  virtual void PullData() = 0;
  virtual void PushData() = 0;
  virtual void Process() = 0;



  BehaviorContextType behavior_context_type_;

};

}
}

#endif // SRC_EXECUTION_BEHAVIORPLAN_BEHAVIORBASE_BEHAVIOR_CONTEXT_BASE_H_
