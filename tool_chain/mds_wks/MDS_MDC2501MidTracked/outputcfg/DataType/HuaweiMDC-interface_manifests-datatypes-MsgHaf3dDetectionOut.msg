{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHaf3dDetectionOut",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "objectId",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "cls",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "types",
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
      "Name": "coordinate",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "rect",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRect3D"
    },
    {
      "Name": "velocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3f"
    },
    {
      "Name": "contourPoints",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3fVec"
    },
    {
      "Name": "timeCreation",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "acceleration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "blinkerStatus",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "cipvFlag",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "cameraStatus",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    }
  ],
  "IsPlane": "false"
}