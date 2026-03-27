{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/TaskManageStatus",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "action_id",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "task_status",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "task_type",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "task_progress",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    },
    {
      "Name": "distance_to_nearest",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "total_mileage",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "current_mileage",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "progress",
      "Type": "/HuaweiMDC/StdCppBasicDataType/uint8_t"
    }
  ],
  "IsPlane": "false"
}