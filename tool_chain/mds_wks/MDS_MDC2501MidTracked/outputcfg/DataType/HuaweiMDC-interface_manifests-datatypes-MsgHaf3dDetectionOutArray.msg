{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHaf3dDetectionOutArray",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "timestamp",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "count",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "detectionOut3d",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHaf3dDetectionOutVec"
    },
    {
      "Name": "detectionOut2d",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHaf2dDetectionOutVec"
    },
    {
      "Name": "frameID",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "seq",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "time_chain_vec",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonTimeChainVec"
    }
  ],
  "IsPlane": "false"
}