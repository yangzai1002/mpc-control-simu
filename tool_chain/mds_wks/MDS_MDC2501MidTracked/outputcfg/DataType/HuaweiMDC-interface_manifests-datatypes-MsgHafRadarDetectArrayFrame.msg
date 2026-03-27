{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarDetectArrayFrame",
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
      "Name": "detectList",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafRadarDetectDataVec"
    },
    {
      "Name": "maxDistanceOverAzimuthList",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "azimuthForMaxDistanceList",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "factorDistanceOverElevationList",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "elevationForFactorMaxDistanceList",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "maxDistanceDueToProgram",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "minDistanceDueToProgram",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    }
  ],
  "IsPlane": "false"
}