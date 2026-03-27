{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgImageData",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "timestamp",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTime"
    },
    {
      "Name": "frameID",
      "Type": "/HuaweiMDC/StdCppDataType/String"
    },
    {
      "Name": "width",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "height",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "dataSize",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "bufferType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "imageType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "seq",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "rawData",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpUint8Vec"
    },
    {
      "Name": "mbufData",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int64_t"
    }
  ],
  "IsPlane": "false"
}