{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTrajectoryPoint",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "wayPoint",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafWayPoint"
    },
    {
      "Name": "speed",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "acc",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "timeRelative",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "deltaAcc",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "steerAngle",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "length",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "risk_level",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    }
  ],
  "IsPlane": "false"
}