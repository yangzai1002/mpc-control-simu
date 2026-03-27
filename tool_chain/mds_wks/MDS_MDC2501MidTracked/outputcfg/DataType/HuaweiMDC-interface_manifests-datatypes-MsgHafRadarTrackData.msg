{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarTrackData",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "id",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "idState",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "lifetime",
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
      "Name": "size",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarModeDatainfo"
    },
    {
      "Name": "orient",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarTrackOrient"
    },
    {
      "Name": "yaw",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarTrackYaw"
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
      "Name": "underpassProbability",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "overpassProbability",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "existProbability",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "movProperty",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "trackState",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "trackType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t",
      "Padding": "4"
    },
    {
      "Name": "heading",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    }
  ],
  "IsPlane": "true"
}