{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "seq",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int64_t"
    },
    {
      "Name": "time",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "module_name",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "data_name",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "frame_id",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "fault_vec",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonFaultInfoVec"
    },
    {
      "Name": "time_chain_vec",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonTimeChainVec"
    }
  ],
  "IsPlane": "false"
}