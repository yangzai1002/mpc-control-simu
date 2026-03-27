{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafEgoTrajectory",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "egoTrajectoryHeader",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "trajectoryLength",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "trajectoryPeriod",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "trajectoryPoints",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTrajectoryPointVec"
    },
    {
      "Name": "wayPoints",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafWayPointVec"
    },
    {
      "Name": "isReplanning",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "gear",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "estop",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafEstop"
    },
    {
      "Name": "routingHeader",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "selfLaneid",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "trajectoryType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "targetLaneId",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "turnLight",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "isHeld",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    }
  ],
  "IsPlane": "false"
}