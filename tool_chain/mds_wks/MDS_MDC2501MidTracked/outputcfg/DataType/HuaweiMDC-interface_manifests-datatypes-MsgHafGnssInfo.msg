{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafGnssInfo",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "latitude",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "longitude",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "elevation",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "utmPosition",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "utmZoneNum",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "utmZoneChar",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "heading_valid",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "pos_valid",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "attitude",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "sdPosition",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "sdVelocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "sdAttitude",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "second",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "satUseNum",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "satInViewNum",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "solutionStatus",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint16_t"
    },
    {
      "Name": "positionType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint16_t"
    },
    {
      "Name": "linearVelocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "attitudeDual",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "sdAngleDual",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "baseLineLengthDual",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "solutionStatusDual",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "positionTypeDual",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "solutionSourceDual",
      "Type": "/HuaweiMDC/StdCppBasicDataType/int32_t"
    },
    {
      "Name": "cep68",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "cep95",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "pDop",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "hDop",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "vDop",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    }
  ],
  "IsPlane": "false"
}