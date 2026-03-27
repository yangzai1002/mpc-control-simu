{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/VideoEncodingPerf",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "avg_ms_scaling",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "avg_ms_color_converting",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "avg_ms_encoding",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "avg_ms_sending",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "avg_ms_span_from_driver_to_scale",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "fps",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "avg_ms_span_from_driver_to_send_done",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    }
  ],
  "IsPlane": "false"
}