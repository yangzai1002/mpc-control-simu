{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/VideoRTPParameter",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "ssrc",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "peer_host",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "peer_port",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "push_enabled",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    }
  ],
  "IsPlane": "false"
}