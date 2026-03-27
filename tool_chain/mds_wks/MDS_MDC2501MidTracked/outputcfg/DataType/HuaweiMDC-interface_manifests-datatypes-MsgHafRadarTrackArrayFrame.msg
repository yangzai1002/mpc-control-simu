{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarTrackArrayFrame",
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
      "Name": "sensorID",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "seq",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint32_t"
    },
    {
      "Name": "trackList",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarTrackDataVec"
    },
    {
      "Name": "yawRate",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "vehicleSpeed",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    }
  ],
  "IsPlane": "false"
}