{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafIMU",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "imuHeader",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "angularVelocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "linearAcceleration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "imuStatus",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint16_t"
    },
    {
      "Name": "temperature",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    }
  ],
  "IsPlane": "false"
}