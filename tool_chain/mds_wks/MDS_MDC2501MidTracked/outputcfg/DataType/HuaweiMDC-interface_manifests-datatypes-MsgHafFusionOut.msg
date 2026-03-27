{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafFusionOut",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "objectID",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "cs",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "confidence",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "existenceProbability",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "rect",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRect3D"
    },
    {
      "Name": "rect_2d",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRect2D"
    },
    {
      "Name": "absRect",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRect3D"
    },
    {
      "Name": "velocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3f"
    },
    {
      "Name": "absVelocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3f"
    },
    {
      "Name": "velocityCovariance",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpDoubleVec"
    },
    {
      "Name": "absVelocityCovariance",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpDoubleVec"
    },
    {
      "Name": "acceleration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "absAcceleration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "accelerationCovariance",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpDoubleVec"
    },
    {
      "Name": "absAccelerationCovariance",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpDoubleVec"
    },
    {
      "Name": "pathPoints",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3DVec"
    },
    {
      "Name": "timeCreation",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "lastUpdatedTime",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "cipvFlag",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "blinkerFlag",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "fusionType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "anchorPoint",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "absAnchorPoint",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    }
  ],
  "IsPlane": "false"
}