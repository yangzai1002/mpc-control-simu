{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/TrackPath",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "status",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "avoid",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "speed",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "points",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/TrackPointVec"
    }
  ],
  "IsPlane": "false"
}