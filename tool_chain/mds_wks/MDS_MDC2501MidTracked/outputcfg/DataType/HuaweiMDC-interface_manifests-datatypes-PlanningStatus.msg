{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/PlanningStatus",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "command_status",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "current_limit_speed",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "current_set_speed",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "current_set_angle",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "decision_info",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "driving_center_trajectory",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafEgoTrajectory"
    },
    {
      "Name": "driving_left_boundary",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafEgoTrajectory"
    },
    {
      "Name": "driving_right_boundary",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafEgoTrajectory"
    },
    {
      "Name": "risk_level",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    }
  ],
  "IsPlane": "false"
}