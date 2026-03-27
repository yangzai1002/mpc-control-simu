{
  "ShortName": "/HuaweiMDC/interface_manifests/datatypes/PerceptionSemanticMap",
  "Namespace": "::ara::adsfi",
  "Type": "STRUCTURE",
  "Subelements": [
    {
      "Name": "header",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/CommonHeader"
    },
    {
      "Name": "trigger_stamp",
      "Type": "/HuaweiMDC/StdCppBasicDataType/double"
    },
    {
      "Name": "cell_size",
      "Type": "/HuaweiMDC/StdCppBasicDataType/float"
    },
    {
      "Name": "ultra_results",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/PerceptionUtraDataVec"
    },
    {
      "Name": "semantic_objs",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/PerceptionSemanticObjVec"
    },
    {
      "Name": "terrain_patches",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/PerceptionTerrainPatchVec"
    },
    {
      "Name": "reserved",
      "Type": "/HuaweiMDC/interface_manifests/datatypes/IdpUint8Vec"
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
    }
  ],
  "IsPlane": "false"
}