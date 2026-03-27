{
  "ShortName": "/HuaweiMDC/MdcPlatformServices/PlatformServiceInterface/DataTypes/FmMcuDataHeader",
  "Namespace": "::mdc::fm",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "type",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "version",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t",
      "Padding": "2"
    },
    {
      "Name": "checkSum",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "seqId",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t",
      "Padding": "4"
    },
    {
      "Name": "sendTime",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint64_t"
    }
  ],
  "IsPlane": "true"
}