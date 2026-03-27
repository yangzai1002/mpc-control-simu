{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/BusinessCommand",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "business_mode",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "business_path",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/BusinessPath"
    },
    {
      "Name": "last_path",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafEgoTrajectory"
    },
    {
      "Name": "remote_ctl",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/BusniessRemoteControl"
    },
    {
      "Name": "param",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/BusinessCommandParameter"
    },
    {
      "Name": "estop_flag",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "record_trigger_flag",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "loc_strategy",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "loc_control_mode",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    }
  ],
  "IsPlane": "false"
}