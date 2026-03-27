{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafFusionOutArray",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "stamp",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "time_chain_vec",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonTimeChainVec"
    },
    {
      "Name": "count",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "fusionOut",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafFusionOutVec"
    },
    {
      "Name": "frameID",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "seq",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    }
  ],
  "IsPlane": "false"
}