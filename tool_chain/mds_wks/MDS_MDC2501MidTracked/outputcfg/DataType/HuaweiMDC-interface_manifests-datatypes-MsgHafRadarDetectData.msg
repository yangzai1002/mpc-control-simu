{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarDetectData",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "id",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "idPair",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "coordinate",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t",
      "Padding": "5"
    },
    {
      "Name": "position",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarModeDatainfo"
    },
    {
      "Name": "velocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarModeDatainfo"
    },
    {
      "Name": "acceleration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarModeDatainfo"
    },
    {
      "Name": "rcs",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "snr",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "existProbability",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "falseProbability",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "movProperty",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "invalidState",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "ambiguity",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t",
      "Padding": "3"
    }
  ],
  "IsPlane": "true"
}