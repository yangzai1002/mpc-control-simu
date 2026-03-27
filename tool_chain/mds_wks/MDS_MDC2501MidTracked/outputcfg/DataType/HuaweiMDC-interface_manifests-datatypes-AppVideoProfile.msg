{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/AppVideoProfile",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "front_source",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "output_mode",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "adaptive_mode",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "manual_bps",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "manual_width",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "manual_height",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "remote_control_assist",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "remote_control_alarm",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "split_adaptive",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "manual_split_main",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "manual_split_left",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "manual_split_right",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "receiver_reported_fps",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "receiver_reported_bps",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "receiver_reported_ts",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int64_t"
    },
    {
      "Name": "points",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3fVec"
    },
    {
      "Name": "video_third_person",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "split_area_parames",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/AppVideoSplitParamVec"
    },
    {
      "Name": "auto_control_assist",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    }
  ],
  "IsPlane": "false"
}