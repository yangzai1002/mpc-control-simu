{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/MsgHafLocation",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafHeader"
    },
    {
      "Name": "llh",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgPoint3D"
    },
    {
      "Name": "pose",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafPoseWithCovariance"
    },
    {
      "Name": "dr_pose",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafPoseWithCovariance"
    },
    {
      "Name": "velocity",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafTwistWithCovariance"
    },
    {
      "Name": "acceleration",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/MsgHafAccelWithCovariance"
    },
    {
      "Name": "v",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "locationState",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint16_t"
    },
    {
      "Name": "gnssState",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint16_t"
    },
    {
      "Name": "odomType",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    }
  ],
  "IsPlane": "false"
}