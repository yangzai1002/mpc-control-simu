{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgLidarFrame",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "timestamp",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "seq",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "frameID",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "pointCloud",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPointXYZIRTVec"
    }
  ],
  "IsPlane": "false"
}