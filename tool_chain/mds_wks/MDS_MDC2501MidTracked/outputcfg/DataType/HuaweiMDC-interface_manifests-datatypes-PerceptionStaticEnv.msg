{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/PerceptionStaticEnv",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "send_stamp",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "start_stamp",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "cell_size",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "semantic_objs",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/PerceptionSemanticObjVec"
    },
    {
      "Name": "terrain_patch_size",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "terrain_patches",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/PerceptionTerrainPatchVec"
    },
    {
      "Name": "map_width",
      "Type": "/HuaweiMDC/StdCppDataType/Int16"
    },
    {
      "Name": "map_height",
      "Type": "/HuaweiMDC/StdCppDataType/Int16"
    },
    {
      "Name": "vehicle_posx",
      "Type": "/HuaweiMDC/StdCppDataType/Int16"
    },
    {
      "Name": "vehicle_posy",
      "Type": "/HuaweiMDC/StdCppDataType/Int16"
    },
    {
      "Name": "reserved",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpUint8Vec"
    }
  ],
  "IsPlane": "false"
}