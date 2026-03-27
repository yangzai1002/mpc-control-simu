{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/VideoEncodedFrame",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "encoding",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "key_frame",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "frame_size",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "frame_data",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpUint8Vec"
    },
    {
      "Name": "fps",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "pts",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    }
  ],
  "IsPlane": "false"
}