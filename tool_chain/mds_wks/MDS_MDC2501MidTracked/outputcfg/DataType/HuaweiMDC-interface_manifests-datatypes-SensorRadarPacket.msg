{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/SensorRadarPacket",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "calibration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonSensorCalibration"
    },
    {
      "Name": "msop_packet",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonUdpPacketVec"
    },
    {
      "Name": "difop_packet",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonUdpPacket"
    }
  ],
  "IsPlane": "false"
}